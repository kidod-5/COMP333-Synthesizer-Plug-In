"""
Generates a major triad which is controllable by a slider.
Trying out things and building comfort with these tools.
Issues:
- Cracking/breaking sound when changing freqs
"""


import numpy as np
import sounddevice as sd
import threading
import tkinter as tk
from typing import Any

SAMPLE_RATE: int = 44100
amplitude: float = 0.2
running: bool = True

# Shared variable for frequency
frequency: float = 440.0


def audio_thread() -> None:
    """Background audio thread that generates sound in real-time."""
    global frequency, running
    phase: float = 0.0
    blocksize: int = 1024  # small block for smooth control

    def callback(outdata: np.ndarray, frames: int, time: Any, status: sd.CallbackFlags) -> None:
        """Audio callback function that fills the output buffer."""
        nonlocal phase
        t: np.ndarray = (np.arange(frames) + phase) / SAMPLE_RATE

        # Create simple harmonic-rich waveform
        wave1: np.ndarray = np.sin(2 * np.pi * frequency * t)
        wave2: np.ndarray = np.sin(2 * np.pi * (frequency * 5 / 4) * t)
        wave3: np.ndarray = np.sin(2 * np.pi * (frequency * 3 / 2) * t)

        out: np.ndarray = amplitude * (wave1 + 0.5 * wave2 + 0.8 * wave3) / 3
        outdata[:] = out.reshape(-1, 1)
        phase += frames

    with sd.OutputStream(
        callback=callback,
        samplerate=SAMPLE_RATE,
        channels=1,
        blocksize=blocksize
    ):
        while running:
            sd.sleep(100)


def on_slider_change(val: str) -> None:
    """Update frequency from GUI slider value."""
    global frequency
    frequency = float(val)


def on_close() -> None:
    """Stop audio thread and close GUI."""
    global running
    running = False
    root.destroy()


# --- GUI ---
root: tk.Tk = tk.Tk()
root.title("Simple Synth Knob")

slider: tk.Scale = tk.Scale(
    root,
    from_=20,
    to=1000,
    orient='horizontal',
    length=300,
    label='Frequency (Hz)',
    command=on_slider_change
)
slider.set(440)
slider.pack(padx=20, pady=20)

root.protocol("WM_DELETE_WINDOW", on_close)

# Start audio thread
thread: threading.Thread = threading.Thread(target=audio_thread, daemon=True)
thread.start()

root.mainloop()







# import numpy as np
# import sounddevice as sd

# frequency = 440      # base frequency (Hz)
# duration = 5.0       # seconds
# SAMPLE_RATE = 44100  # samples per second

# t = np.linspace(0, duration, int(SAMPLE_RATE * duration), endpoint=False)

# # Fundamental frequency
# wave1 = np.sin(2 * np.pi * frequency * t)
# wave2 = np.sin(2 * np.pi * (frequency * 5/4) * t)
# wave3 = np.sin(2 * np.pi * (frequency * 3/2) * t)
# wave4 = np.sin(2 * np.pi * (frequency * 9/5) * t)
# wave5 = np.sin(2 * np.pi * (frequency * 256/135) * t)

# # Combine the two waves and normalize to avoid clipping
# wave = 0.2 * (wave1 + (0.5 * wave2) + (0.8 * wave3) + (0.5 * wave4) + (0.9 * wave5)) / 3

# # wave1 = np.sin(2 * np.pi * frequency * t)
# # wave2 = np.sin(2 * np.pi * (frequency * 3/2) * t)

# # wave = 0.2 * ((0.5 * wave1) + (0.5 * wave2)) / 3

# sd.play(wave, SAMPLE_RATE)
# sd.wait()