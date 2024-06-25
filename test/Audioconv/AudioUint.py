import wave
import numpy as np

# Replace 'your_file.wav' with the path to your .wav file if it's not in the same directory
wav_filename = 'Audio.wav'
output_filename = 'audio_buffer.bin'

# Read the .wav file
with wave.open(wav_filename, 'rb') as wav_file:
    # Extract raw audio data
    n_channels, sampwidth, framerate, n_frames, comptype, compname = wav_file.getparams()
    frames = wav_file.readframes(n_frames)

# Convert to numpy array and then to uint8 format
audio_data = np.frombuffer(frames, dtype=np.int16)  # Assuming 16-bit PCM
audio_data = audio_data.astype(np.uint8)

# Save the buffer to a binary file
audio_data.tofile(output_filename)

print(f'Audio data has been written to {output_filename}')