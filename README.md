# COMP333-Synthesizer-Plug-In
ML backend Music synthesizing tool

Milestone 1: https://docs.google.com/document/d/1Ckj0KGufHaUpQLS1IokM4mh54LC3KgSbybEukBqg-gc/edit?tab=t.0#heading=h.3vjma1ynrl6f
Style guide: https://github.com/kidod-5/COMP333-Synthesizer-Plug-In/blob/main/StyleGuide.md

## User Instructions

We have a very basic version of the plugin available to the public. To install, simply download the .pkg file, and follow the onscreen instructions. This plugin is available only for Mac DAWs.

## Developer Instructions

Note:  Users will need access to the Node/Node Package Manager, Python and JUCE/PROJUCER.  Links to those will be available here (Make sure to follow the instructions for your appropriate operating system when downloading):

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

<!-- 1. Open the ```.projucer``` file in the MLPlugIn folder.
2. Check in the Exporters --> Xcode --> Debug and the Exporters --> Xcode --> Release folders that the path to the libtorch dependency is set up correctly:
   1. In Header Search Paths put:
      1. ./dependencies/libtorch/include
      2. ./dependencies/libtorch/include/torch/csrc/api/include
   2. In Extra Library Search Paths put:
      1. ./dependencies/libtorch/lib
3. Click the button on the top to open the project in Xcode
4. Make sure the following files are in Target --> Build Phases --> Link Binary With LIbraries for both the VST3 and AU targets:
   1. libtorch.dylib
   2. libc10.dylib
   3. libtorch_cpu.dylib
5. In the Xcode file's build settings, for both VST3 and AU, make the library search paths: $(SRCROOT)/dependencies/libtorch/lib -->

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

### Running JUCE:
--


## Contributers
Kido - kidod-5
Samvit - s4mmyb0i
Tariq - TMSW2003


