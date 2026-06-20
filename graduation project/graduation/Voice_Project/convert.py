import wave

# Settings matching your ESP32 microphone exactly
RAW_FILE = 'stored_audio/live_stream.raw'  # Change path if yours is named differently
OUTPUT_WAV = 'stored_audio/voice_output.wav'
CHANNELS = 1          # Mono
SAMPLE_WIDTH = 2      # 16-bit PCM (2 bytes)
SAMPLE_RATE = 8000  # 8 kHz sample rate
try:
    with open(RAW_FILE, 'rb') as raw_f:
        raw_data = raw_f.read()
        
    with wave.open(OUTPUT_WAV, 'wb') as wav_f:
        wav_f.setnchannels(CHANNELS)
        wav_f.setsampwidth(SAMPLE_WIDTH)
        wav_f.setframerate(SAMPLE_RATE)
        wav_f.writeframes(raw_data)
        
    print(f"🎉 Success! Beautiful WAV file created at: {OUTPUT_WAV}")
    print("Go to that folder, right-click the file, and play it using your normal Windows Media Player!")
except FileNotFoundError:
    print(f"❌ Error: Could not find '{RAW_FILE}'. Check your folder name!")