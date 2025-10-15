#pragma once
#include <portaudio.h>
#include <sndfile.h>
#include <string>
#include <vector>
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>

class AudioRecorder {
public:
    AudioRecorder() : recording(false) {
        Pa_Initialize();
    }

    ~AudioRecorder() {
        Pa_Terminate();
    }

    bool RecordToFile(const std::string& filename, int durationSeconds = 10, int sampleRate = 16000) {
        recording = true;

        PaStream* stream;
        const int framesPerBuffer = 512;
        const int numChannels = 1;
        const PaSampleFormat sampleFormat = paInt16;

        std::vector<short> buffer(sampleRate * durationSeconds);

        PaError err = Pa_OpenDefaultStream(&stream,
                                           numChannels, 0,
                                           sampleFormat,
                                           sampleRate,
                                           framesPerBuffer,
                                           nullptr,
                                           nullptr);
        if (err != paNoError) {
            std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
            return false;
        }

        Pa_StartStream(stream);
        std::cout << "Recording started..." << std::endl;

        size_t framesRecorded = 0;
        auto startTime = std::chrono::steady_clock::now();

        while (recording && framesRecorded < buffer.size()) {
            short temp[framesPerBuffer];
            Pa_ReadStream(stream, temp, framesPerBuffer);

            size_t toCopy = std::min<size_t>(framesPerBuffer, buffer.size() - framesRecorded);
            std::copy(temp, temp + toCopy, buffer.begin() + framesRecorded);
            framesRecorded += toCopy;

            if (std::chrono::steady_clock::now() - startTime >
                std::chrono::seconds(durationSeconds)) break;
        }

        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        recording = false;

        // Write WAV file
        SF_INFO sfinfo;
        sfinfo.channels = numChannels;
        sfinfo.samplerate = sampleRate;
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

        SNDFILE* outfile = sf_open(filename.c_str(), SFM_WRITE, &sfinfo);
        if (!outfile) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            return false;
        }

        sf_write_short(outfile, buffer.data(), framesRecorded);
        sf_close(outfile);

        std::cout << "Recording saved to " << filename << std::endl;
        return true;
    }

    void Stop() {
        recording = false;
    }

private:
    std::atomic<bool> recording;
};
