# COMP333-Synthesizer-Plug-In
ML backend Music synthesizing tool

Milestone 1: https://docs.google.com/document/d/1Ckj0KGufHaUpQLS1IokM4mh54LC3KgSbybEukBqg-gc/edit?tab=t.0#heading=h.3vjma1ynrl6f
Style guide: https://github.com/kidod-5/COMP333-Synthesizer-Plug-In/blob/main/StyleGuide.md

## User Instructions

We have a very basic version of the plugin available to the public. To install, simply download the .pkg file, and follow the onscreen instructions. This plugin is available only for Mac DAWs.

Note for Peer Reviewers: To be able to use this, you must open a DAW. Most Macs come pre installed with GarageBand, and you should be able to access it there under audio effects in your track mixer.
The plugin is stil not ready and it crashes, but we are working on it!

## Developer Instructions

Note: Users will need access to the Node/Node Package Manager, Python and JUCE/PROJUCER.  Links to those will be available here (Make sure to follow the instructions for your appropriate operating system when downloading):
Note: This project can only be built on Apple Silicon Macs.

Python: https://www.python.org/downloads/ 
Node(JS): https://nodejs.org/en/download 
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

### Running Client folder code:

1. Go to https://nodejs.org and install Node.js, download latest version for your operating system. Skip this step if you already have Node.js installed 
2. Cd to client folder
3. Run 'npm install' in terminal to install dependencies 
4. Now run 'npm run dev'
5. You will see a link in your terminal that says something like “http://localhost:5173/“. Copy and paste link into your preferred web browser to see the output.

### Running code in server folder:

1. Cd to server folder
2. Run '. .venv/bin/activate' to activate virtual environment
3. Run 'pip install flask' to install flask
4. Run 'flask run'
5. You should see something like 'Running on http://127.0.0.1:5000'. Copy and paste link into your preferred web browser to see the output.

### Running Plug-in:
1. within the build folder, download the .pkg file containing the build for the synthesizer
2. Opening the package should allow the plug-in to open using a compatible software* (e.g garageband).
3. Once it it open feel free to test out the limited range of options (for now!)

*Note: this software is intended for mac users and may not be directly compatible with other devices as of now.


## Contributers
Kido - kidod-5
Samvit - s4mmyb0i
Tariq - TMSW2003


