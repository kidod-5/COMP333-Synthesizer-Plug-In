# COMP333-Synthesizer-Plug-In
ML backend Music synthesizing tool

Style guide: https://github.com/kidod-5/COMP333-Synthesizer-Plug-In/blob/main/StyleGuide.md

## Introduction
Welcome to ML-FX. This project uses the JUCE library in combination with LibTorch and the RAVE family of machine learning models to explore an alternative approach to sound engineering. Our goal was to create a plug-in capable of augmenting audio using a slim, but growing, set of sound-based models, allowing music to be enhanced in ways that keep creative control firmly in the hands of artists. At the same time, the tool aims to automate and speed up sound experimentation, particularly for creators who may not have the resources to develop or modify sounds from scratch.

We prioritized user freedom throughout the design process and emphasize that this tool is meant to complement existing projects rather than function as a standalone or definitive sound. ML-FX is intended to expand creative possibilities, not replace them. Ultimately, we hope this tool helps raise the creative ceiling for musicians and producers and contributes to the future direction of music editing software.

## User Instructions

Downlaod the MLPlugIn-Installer packahge, and follow the onscreen instructions. You're ready to go!

## Developer Instructions

Note: Users will need access to the Node/Node Package Manager, Python and JUCE/PROJUCER.  Links to those will be available here (Make sure to follow the instructions for your appropriate operating system when downloading):
Note: This project can only be built on Apple Silicon Macs.

Python: https://www.python.org/downloads/ 

JUCE: https://juce.com/download/


1. Open the GitHub repository
2. Press the green button that says “Code”
3. Copy HTTPS url to clipboard
4. Open your code editor of choice
5. Open terminal
6. To create a local copy of the repository, type "git clone " and then paste the HTTPS url
7. Now that you have a local repository downloaded,open the folder in your terminal and cd into the folder

### To set up libtorch

1. Run the ```install_dependencies.sh``` script. This will install libtorch in the MLPlugIn/dependencies directory.

### To set up Xcode Project

1. Run ```cmake -B build -G Xcode``` in the MLPlugIn directory. Inside the build folder, you will see the xcode project.


### Running Plug-in:
1. within the build folder, download the .pkg file containing the build for the synthesizer
2. Opening the package should allow the plug-in to open using a compatible software* (e.g garageband).
3. Once it it open feel free to test out the limited range of options (for now!)

*Note: this software is intended for mac users and may not be directly compatible with other devices as of now.


## Contributers
Kido - kidod-5
Samvit - s4mmyb0i
Tariq - TMSW2003


