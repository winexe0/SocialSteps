#include "SpeakText.h"
#ifdef __WXMSW__
#else
#include <piper.h>
#include <SDL2/SDL.h>
extern piper_synthesizer *speech;
#endif
std::atomic<bool> speakTextCancelFlag{false};
std::atomic<bool> speechRunning{false};
std::thread speakTextThread;
void SpeakText(wxString text, bool waitForPrevious)
{
    text.Replace("*", " "); // Replace newlines with spaces for better speech synthesis
#ifdef __WXMSW__
    wxString command = "wsay.exe \"" + text + "\"";
    wxExecute(command);
    /*wxString command = "PowerShell -Command \"Add-Type -AssemblyName System.Speech; "
                       "$speak = New-Object System.Speech.Synthesis.SpeechSynthesizer; "
                       "$speak.Speak('" + text + "')\"";*/
#elif __WXOSX__
    wxString command = "say \"" + text + "\"";
    wxExecute(command);
#else
    if (waitForPrevious) {
        // Wait for previous speech to finish
        if (speakTextThread.joinable()) {
            speakTextThread.join();
        }
        while (speechRunning) {
            wxMilliSleep(50);
            wxTheApp->Yield();  // Let the UI process events, so it doesn't hang
        }
    } else {
        // Cancel any currently running speech
        speakTextCancelFlag = true;
        if (speakTextThread.joinable()) {
            speakTextThread.join();
        }
        speakTextCancelFlag = false;
    }
    speechRunning = true;
    speakTextThread = std::thread([text] {
        if (!speech) {
            speechRunning = false;
            return;
        }
        std::vector<float> audio;
        int res = piper_synthesize_start(speech, text.mb_str(), nullptr);
        if (res != PIPER_OK) return;
        piper_audio_chunk chunk;
        while (!speakTextCancelFlag) {
            res = piper_synthesize_next(speech, &chunk);
            if (res != PIPER_OK && res != PIPER_DONE) break;
            audio.insert(audio.end(), chunk.samples, chunk.samples + chunk.num_samples);
            if (chunk.is_last || res == PIPER_DONE) break;
        }
        if (speakTextCancelFlag) return;
        SDL_AudioSpec spec;
        spec.freq = 22050;
        spec.format = AUDIO_F32SYS;
        spec.channels = 1;
        spec.samples = 1024;
        spec.callback = nullptr;
        spec.userdata = nullptr;
        if (SDL_Init(SDL_INIT_AUDIO) < 0) return;
        if (SDL_OpenAudio(&spec, nullptr) < 0) {
            SDL_Quit();
            return;
        }
        SDL_QueueAudio(1, audio.data(), static_cast<Uint32>(audio.size() * sizeof(float)));
        SDL_PauseAudio(0);
        Uint32 ms = static_cast<Uint32>((audio.size() * 1000) / spec.freq);
        Uint32 elapsed = 0;
        while (elapsed < ms && !speakTextCancelFlag) {
            SDL_Delay(50);
            elapsed += 50;
        }
        SDL_CloseAudio();
        SDL_Quit();
        speechRunning = false;
    });
#endif
}