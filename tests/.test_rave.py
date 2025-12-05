import torch
import soundfile as sf
import numpy as np

model_path = "../RAVE_models/nasa.ts"
device = "cuda" if torch.cuda.is_available() else "cpu"

model = torch.jit.load(model_path, map_location=device)
model.eval().to(device)
rave = model._rave  # raw RAVE core

# Load audio
audio_path = "/Users/sam/Music/Logic/Project 1/Audio Files/njys type beat.aif"
x_np, sr = sf.read(audio_path)

if sr != 48000:
    raise ValueError("RAVE requires 48kHz input")

if x_np.ndim > 1:
    x_np = x_np.mean(axis=1)

x = torch.tensor(x_np, dtype=torch.float32)[None,None].to(device)

batch_size = 16  # match the cached_conv batch used at export

# x: [1,1,N]
num_samples = x.shape[-1]

# Pad audio so number of frames is multiple of batch_size
frame_size = 8192
hop_size = frame_size

num_frames = (num_samples + frame_size - 1) // frame_size
total_samples = num_frames * frame_size
pad_length = total_samples - num_samples
x_padded = torch.nn.functional.pad(x, (0, pad_length))

# Unfold into frames
frames = x_padded.unfold(-1, frame_size, hop_size).squeeze(1)
frames = frames.permute(1,0,2).reshape(-1,1,frame_size)  # [num_frames,1,frame_size]

processed_frames = []

with torch.no_grad():
    for i in range(0, len(frames), batch_size):
        batch = frames[i:i+batch_size]

        # Repeat last frames to fill batch if smaller
        if batch.shape[0] < batch_size:
            repeat_count = batch_size - batch.shape[0]
            batch = torch.cat([batch, batch[:repeat_count]], dim=0)

        # Encode → decode → synth
        z = rave.encoder(batch)
        z = z * 0.5  # optional latent manipulation
        feat = rave.decoder(z)
        out = rave.synth(feat)

        # Keep only original frames (ignore repeats)
        out = out[:min(batch_size, frames.shape[0]-i)]
        processed_frames.append(out)

# Concatenate frames and remove padding
y_out = torch.cat(processed_frames, dim=-1)
y_out = y_out[:, :, :num_samples].squeeze().cpu().numpy()
sf.write("processed_audio.wav", y_out, 48000)


# import torch


# import soundfile as sf
# import torch.nn.functional as F
# import numpy as np

# # ----------------------------------------------
# # 1. Load TorchScript RAVE model
# # ----------------------------------------------
# model_path = "../RAVE_models/vintage.ts"
# device = "cuda" if torch.cuda.is_available() else "cpu"

# model = torch.jit.load(model_path, map_location=device)
# model.eval().to(device)

# print(model)

# print("model loaded")

# # ----------------------------------------------
# # 2. Load audio (must be 48 kHz!)
# # ----------------------------------------------
# audio_path = "/Users/sam/Music/Logic/Project 1/Audio Files/njys type beat.aif"
# y, sr = sf.read(audio_path)

# print("audio loaded")

# if sr != 48000:
#     raise ValueError("RAVE pretrained models require 48 kHz input.")

# # Mono
# if len(y.shape) > 1:
#     y = y.mean(axis=1)

# # Convert to tensor [1,1,N]
# x = torch.tensor(y, dtype=torch.float32).unsqueeze(0).unsqueeze(0).to(device)

# print("audio converted")

# # ----------------------------------------------
# # 3. Encode whole audio into latents
# # ----------------------------------------------
# with torch.no_grad():
#     z = model.encode(x)   # latent representation

# # OPTIONAL latent modification
# z = z * 0.5

# # ----------------------------------------------
# # 4. Decode back to audio
# # ----------------------------------------------
# with torch.no_grad():
#     y_out = model.decode(z)  # reconstructed audio

# y_out = y_out.squeeze().cpu().numpy()


# print("y_out dtype:", y_out.dtype)
# print("y_out shape:", y_out.shape)
# print("Has NaNs:", np.isnan(y_out).any())
# print("Has Infs:", np.isinf(y_out).any())
# print("Min:", y_out.min() if y_out.size > 0 else None)
# print("Max:", y_out.max() if y_out.size > 0 else None)


# # ----------------------------------------------
# # 5. Save result
# # ----------------------------------------------
# sf.write("processed_audio.wav", y_out, sr)
# print("Done! Output saved to processed_audio.wav")