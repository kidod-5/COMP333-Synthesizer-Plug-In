#!/usr/bin/env python3
import os
import sys
import pathlib
import urllib.request
import urllib.error

DOWNLOAD_URL = "https://play.forum.ircam.fr/rave-vst-api/get_model/vintage"
DEST_DIR = "/Library/Application Support/dynamicsounds/MLPlugIn/models"


def ensure_destination():
    dest_path = pathlib.Path(DEST_DIR)
    try:
        dest_path.mkdir(parents=True, exist_ok=True)
    except PermissionError:
        print(f"Permission denied creating {DEST_DIR}.")
        print("On macOS, run this script with sudo, e.g.:")
        print(f"  sudo {sys.executable} {sys.argv[0]}")
        sys.exit(1)
    return dest_path


def download_file(url, dest_path):
    # Use a sensible default filename if the URL provides none
    default_name = "vintage.gin"
    basename = os.path.basename(url.split("?", 1)[0]) or default_name
    target = dest_path / basename

    print(f"Downloading:\n  {url}\nTo:\n  {target}")
    try:
        with urllib.request.urlopen(url) as resp, open(target, "wb") as f:
            chunk = resp.read(8192)
            while chunk:
                f.write(chunk)
                chunk = resp.read(8192)
    except urllib.error.URLError as e:
        print(f"Download failed: {e}")
        sys.exit(1)

    print("Download completed.")
    return target


def main():
    dest_path = ensure_destination()
    downloaded = download_file(DOWNLOAD_URL, dest_path)
    print(f"Saved model to: {downloaded}")


if __name__ == "__main__":
    main()
