#!/bin/bash

# Exit on error
set -e

# Config
LIBTORCH_VERSION="2.9.0"   # Make this match the downloaded version
OS="macos"                  # macOS arm64
DEST_DIR="./dependancies/libtorch"

# Check if already downloaded
if [ -d "$DEST_DIR" ]; then
    echo "Libtorch already exists in $DEST_DIR"
    exit 0
fi

# Download URL (make sure this matches your CPU/GPU + OS)
URL="https://download.pytorch.org/libtorch/cpu/libtorch-macos-arm64-${LIBTORCH_VERSION}.zip"

# Temp download file
TMP_ZIP="libtorch.zip"

echo "Downloading libtorch $LIBTORCH_VERSION for $OS..."
curl -L $URL -o $TMP_ZIP

echo "Unzipping..."
mkdir -p dependancies
unzip $TMP_ZIP -d dependancies

echo "Cleaning up..."
rm $TMP_ZIP

echo "Libtorch setup complete. Directory: $DEST_DIR"