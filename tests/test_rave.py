import torch
import soundfile as sf
import torch.nn.functional as F

# -----------------------
# 1. Load the TorchScript RAVE model
# -----------------------
model_path = "../RAVE_models/vintage.ts"
device = "cuda" if torch.cuda.is_available() else "cpu"

model = torch.jit.load(model_path, map_location=device)
model.eval().to(device)

rave = model._rave  # internal rave submodule

# -----------------------
# 2. Load your audio
# -----------------------
audio_path = "/Users/sam/Music/Logic/Project 1/Audio Files/njys type beat.aif"
y, sr = sf.read(audio_path)

# Ensure mono
if len(y.shape) > 1:
    y = y.mean(axis=1)

# Convert to torch tensor [batch, channels, samples]
x = torch.tensor(y, dtype=torch.float32).unsqueeze(0).unsqueeze(0).to(device)

# -----------------------
# 3. Slice audio into frames
# -----------------------
frame_size = 8192
hop_size = frame_size  # no overlap for simplicity
batch_size = 16        # required by cached_conv

# Pad audio so total frames is multiple of batch_size
num_frames = (x.shape[-1] + frame_size - 1) // frame_size
total_samples = num_frames * frame_size
pad_length = total_samples - x.shape[-1]
x_padded = F.pad(x, (0, pad_length))

# Unfold into frames: shape [num_frames, 1, frame_size]
frames = x_padded.unfold(-1, frame_size, hop_size).squeeze(1)  # [batch, num_frames, frame_size]
frames = frames.permute(1, 0, 2).reshape(-1, 1, frame_size)    # [num_frames, 1, frame_size]

# -----------------------
# 4. Process frames in batches of 16
# -----------------------
processed_frames = []

for i in range(0, len(frames), batch_size):
    batch = frames[i:i+batch_size]

    # If batch < 16, repeat to fill batch
    if batch.shape[0] < batch_size:
        repeat_count = batch_size - batch.shape[0]
        batch = torch.cat([batch, batch[:repeat_count]], dim=0)

    with torch.no_grad():
        # Encode to latent
        latent = rave.encoder(batch)

        # OPTIONAL: modify latent here
        # Example: scale latent by 0.5
        latent = latent * 0.5

        # Decode audio
        reconstructed = rave.forward(batch)

        # Keep only original frames (ignore repeated ones)
        reconstructed = reconstructed[:min(batch_size, len(batch))]
        processed_frames.append(reconstructed)

# -----------------------
# 5. Combine frames back into waveform
# -----------------------
processed_audio = torch.cat(processed_frames, dim=-1)
processed_audio = processed_audio[:, :, :x.shape[-1]]  # remove padding

print("Processed audio shape:", processed_audio.shape)

# -----------------------
# 6. Save to file
# -----------------------
sf.write("processed_audio.wav", processed_audio.squeeze().cpu().numpy(), sr)



# import torch
# import numpy as np
# import soundfile as sf
# import torch

# # -----------------------
# # 1. Load your TorchScript model
# # -----------------------
# model_path = "../RAVE_models/vintage.ts"
# device = "cuda" if torch.cuda.is_available() else "cpu"

# model = torch.jit.load(model_path, map_location=device)
# model.eval().to(device)

# rave = model._rave  # internal RAVE submodule

# # -----------------------
# # 2. Prepare your audio
# # -----------------------
# audio_path = "/Users/sam/Music/Logic/Project 1/Audio Files/njys type beat.aif"
# y, sr = sf.read(audio_path)  # y is numpy array, sr = sample rate

# # Ensure it's mono
# if len(y.shape) > 1:
#     y = y.mean(axis=1)

# # Convert to torch tensor: [batch, channels, samples]
# x = torch.tensor(y, dtype=torch.float32).unsqueeze(0).unsqueeze(0).to(device)

# # If your audio sample rate isn't 48kHz, resample it first
# # e.g., using torchaudio or librosa

# # -----------------------
# # 3. Slice audio into frames
# # -----------------------
# frame_size = 8192        # typical RAVE frame size
# hop_size = frame_size    # no overlap; plugin may use overlap
# batch_size = 16          # must match cached_conv internal batch/cache

# # pad audio so it divides evenly into frames
# num_frames = (x.shape[-1] + frame_size - 1) // frame_size
# pad_length = num_frames * frame_size - x.shape[-1]
# x_padded = torch.nn.functional.pad(x, (0, pad_length))

# # unfold into frames: shape [num_frames, 1, frame_size]
# # x_padded shape: [batch, channels, samples] => [1,1,samples]
# frames = x_padded.unfold(-1, frame_size, hop_size)  # shape [1, 1, num_frames, frame_size]

# # reorder: move num_frames to batch dimension
# frames = frames.permute(2, 1, 0, 3).reshape(-1, 1, frame_size)

# # -----------------------
# # 4. Process frames in batches of size 16
# # -----------------------
# processed_frames = []

# for i in range(0, len(frames), batch_size):
#     batch = frames[i:i+batch_size]

#     # If last batch is smaller than batch_size, pad by repeating
#     if batch.shape[0] < batch_size:
#         pad_count = batch_size - batch.shape[0]
#         batch = torch.cat([batch, batch[:pad_count]], dim=0)

#     with torch.no_grad():
#         # Get latent
#         latent = rave.encoder(batch)  # shape [batch_size, latent_channels, latent_frames]

#         # Optional: manipulate latent here
#         # Example: scale latent by 0.5
#         latent = latent * 0.5

#         # Decode back to audio
#         reconstructed = rave.forward(batch)  # shape [batch_size, 1, frame_size]
#         processed_frames.append(reconstructed[:batch.shape[0]])  # remove padding if added

# # -----------------------
# # 5. Combine frames back into waveform
# # -----------------------
# processed_audio = torch.cat(processed_frames, dim=-1)
# processed_audio = processed_audio[:, :, :x.shape[-1]]  # remove extra padding

# print("Processed audio shape:", processed_audio.shape)