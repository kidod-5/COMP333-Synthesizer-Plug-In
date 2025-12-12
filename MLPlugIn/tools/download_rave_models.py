import os
import re
import requests
from pathlib import Path

BASE_URL = "https://play.forum.ircam.fr/rave-vst-api/get_model/"

MODELS = [
    "sol_full",
    "sol_ordinario_fast",
    "musicnet",
    "isis",
    "vintage",
    "percussion",
    "nasa",
    "darbouka_onnx",
    "VCTK",
]

TARGET_DIR = Path("/Library/Application Support/dynamicsounds/MLPlugin/models")
TARGET_DIR.mkdir(parents=True, exist_ok=True)

print("Saving to:", TARGET_DIR.resolve())

def filename_from_headers(headers, fallback):
    cd = headers.get("Content-Disposition", "")
    m = re.search(r'filename="([^"]+)"', cd)
    if m:
        return m.group(1)
    m = re.search(r"filename=([^;]+)", cd)
    if m:
        return m.group(1).strip().strip('"')
    return fallback

for model in MODELS:
    url = BASE_URL + model
    print(f"\nDownloading {model} -> {url}")

    r = requests.get(url, stream=True, timeout=60)
    print("Status:", r.status_code, "| Content-Type:", r.headers.get("Content-Type"))

    if r.status_code != 200:
        print("Failed:", model)
        continue

    fname = filename_from_headers(r.headers, f"{model}.ts")
    out_path = TARGET_DIR / fname

    # Skip if already downloaded
    if out_path.exists():
        existing_size = out_path.stat().st_size
        print(f"✔ Already downloaded: {fname} ({existing_size / (1024**2):.1f} MB)")
        continue

    bytes_written = 0
    with open(out_path, "wb") as f:
        for chunk in r.iter_content(chunk_size=1024 * 1024):
            if not chunk:
                continue
            f.write(chunk)
            bytes_written += len(chunk)

    print("Saved to:", out_path.resolve())
    print("Bytes written:", bytes_written)

    if bytes_written < 1024 * 100:
        print("WARNING: file is very small; may be an error page, not a model.")

print("\nDone.")
