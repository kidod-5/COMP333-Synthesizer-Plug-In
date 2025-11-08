# COMP333-Synthesizer-Plug-In
ML backend Music synthesizing tool 

## Issues

### High Priority

### Low Priority

- [ ] Front-end
  - [ ] Waveform visualizer from audio
  - [ ] Overall design and structure
- [ ] Back- to-Front Interface
  - [ ] Working sliders and knobs
    - [ ] Attack
    - [ ] Delay
    - [ ] LFO
    - [ ] Volume
    - [ ] Wave type (sine, square, saw, triangle)
- [ ] Back-end
  - [ ] Simple sound generator which has changeable parameters
- [ ] Other
  - [ ] Identify ML model

### Medium Priority

- [ ] Parameters
  - [ ] Pan
  - [ ] Noise Generator
  - [ ] Filter
  - [ ] LFO
  - [ ] Reverb
- [ ] Front-end
  - [ ] More abstract visualizer
- [ ] Back-end
  - [ ] Preset/Session management

### Low Priority
- [ ] Parameters
  - [ ] MIDI
- [ ] Back-to-Front Interface
  - [ ] Techinal vocab to vernacular mapping (happiness knob changes x,y,z paraameters using a formula, etc)
- [ ] Other
  - [ ] Help assistant (for beginners)
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


