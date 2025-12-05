# import torch
# import numpy as np
# import sounddevice as sd
# import tkinter as tk

# # ------------------------------
# # 1. Load RAVE model
# # ------------------------------
# model_path = "/Users/sam/Documents/Wesleyan/semester_5/software/COMP333-Synthesizer-Plug-In/RAVE_models/musicnet.ts"
# model = torch.jit.load(model_path)
# model.eval()

# # ------------------------------
# # 2. Settings
# # ------------------------------
# CHANNELS = 16           # input audio channels expected by the model
# BLOCK_SIZE = 16384      # exact block size we determined earlier
# LATENT_CHANNELS = 256   # number of latent dimensions
# LATENT_TIME = 128       # latent time dimension

# # ------------------------------
# # 3. Generate a test audio block
# # ------------------------------
# # You can replace this with actual audio later
# audio_block = torch.randn(1, CHANNELS, BLOCK_SIZE)

# with torch.no_grad():
#     z = model.encoder(audio_block)  # [1, 256, 128]

# # ------------------------------
# # 4. Decode helper
# # ------------------------------
# def decode_rave(z: torch.Tensor) -> torch.Tensor:
#     """
#     Feed latent z to the RAVE decoder safely.
#     z: [batch, channels, time]
#     Returns audio tensor [1, CHANNELS, BLOCK_SIZE]
#     """
#     z_dec = z.transpose(1, 2)  # swap channels <-> time
#     with torch.no_grad():
#         return model.decoder(z_dec)

# # ------------------------------
# # 5. Play audio from latent
# # ------------------------------
# def play_latent(z_tensor: torch.Tensor):
#     audio_out = decode_rave(z_tensor)
#     audio_np = audio_out[0].cpu().numpy()  # shape [channels, block_size]
#     # Mix to mono for playback
#     audio_np = audio_np.mean(axis=0)
#     # Normalize
#     audio_np /= np.max(np.abs(audio_np)) + 1e-9
#     sd.play(audio_np, samplerate=16000)
#     sd.wait()

# # ------------------------------
# # 6. Tkinter GUI for latent sliders
# # ------------------------------
# root = tk.Tk()
# root.title("RAVE Latent Sliders")

# # Dictionary to store slider variables
# sliders = {}

# def update_and_play(*args):
#     # Clone latent
#     z_mod = z.clone()
#     # Apply slider values
#     for dim, var in sliders.items():
#         z_mod[0, dim, :] += var.get()
#     play_latent(z_mod)

# # Create sliders for first 10 latent dimensions
# for i in range(10):
#     var = tk.DoubleVar()
#     var.set(0.0)
#     slider = tk.Scale(root, from_=-3.0, to=3.0, resolution=0.05,
#                       orient=tk.HORIZONTAL, length=300,
#                       label=f"Latent {i}", variable=var, command=update_and_play)
#     slider.pack()
#     sliders[i] = var

# # Initial playback
# update_and_play()

# root.mainloop()















import torch

import math

model_path = "/Users/sam/Documents/Wesleyan/semester_5/software/COMP333-Synthesizer-Plug-In/RAVE_models/musicnet.ts"
model = torch.jit.load(model_path)
model.eval()

# Choose a long power-of-two length so it's divisible by typical factors
TEST_LEN = 65536  # long enough for most models; adjust upward if you like

# Determine channel count expected by the model:
# Many RAVE models expect multi-channel (e.g. 16) inputs at encoder input.
# Try common values; if wrong, set CHANNELS to the one you used to encode before.
for CHANNELS in (16, 1, 2):
    try:
        x = torch.zeros(1, CHANNELS, TEST_LEN)
        with torch.no_grad():
            z = model.encoder(x)
        latent_time = z.shape[-1]
        down_factor = TEST_LEN / latent_time
        if abs(round(down_factor) - down_factor) > 1e-6:
            print(f"[channels={CHANNELS}] Non-integer downsampling factor: {down_factor} (latent_time={latent_time})")
        else:
            down_factor = int(round(down_factor))
            required_input_len = 128 * down_factor
            print(f"[channels={CHANNELS}] encoder -> latent shape: {z.shape}")
            print(f"[channels={CHANNELS}] total downsampling factor = {down_factor}")
            print(f"[channels={CHANNELS}] REQUIRED input length = 128 * {down_factor} = {required_input_len}")
        break
    except Exception as e:
        # encoder failed for this CHANNELS guess — try next
        print(f"[channels={CHANNELS}] encoder run failed: {e}")

audio = torch.randn(1, 16, 16384)

with torch.no_grad():
    z = model.encoder(audio)

print(z.shape)   # MUST BE: [1, 256, 128]
print(z)

z_mod = z.clone()
z_mod[0, 3, :] += 1.0  # example latent tweak

def decode_rave(z: torch.Tensor) -> torch.Tensor:
    """
    Feed latent z to the RAVE decoder safely.
    z: [batch, channels, time]
    """
    z_dec = z.transpose(1, 2)  # swap channels <-> time
    with torch.no_grad():
        return model.decoder(z_dec)

audio_out = decode_rave(z_mod)

print(audio_out.shape)

print(audio)
print(audio_out)