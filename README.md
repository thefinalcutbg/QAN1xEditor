# QAN1xEditor
An editor for the Yamaha AN1x synthesizer written in C++ and Qt

## Table of Contents
* [General Info](#general-information)
* [Frameworks and Libraries Used](#frameworks-and-libraries-used)
* [Setup](#setup)
* [Project Status](#project-status)
* [Contact](#contact)
* [License](#license)

## General Information

QAN1x is a free open-source software written in C++ and Qt. It allows users to edit most of the voice parameters from the UI and use the computer keyboard and mouse to play notes.

## Frameworks and Libraries Used
- [Qt6 Framework](https://www.qt.io/)
- [QMidi](https://github.com/thomasgeissl/QMidi)
- [RtMidi](https://github.com/thestk/rtmidi)
- [Sqlite3](https://www.sqlite.org/index.html)

## Screenshots
![Alt text](/screenshots/scr0.png?raw=true "Patch Browser")
![Alt text](/screenshots/scr5.png?raw=true "Free EG")
![Alt text](/screenshots/scr1.png?raw=true "Scene Controls")
![Alt text](/screenshots/scr2.png?raw=true "Effects section")
![Alt text](/screenshots/scr3.png?raw=true "Arpeggiator and Sequencer")
![Alt text](/screenshots/scr4.png?raw=true "Control Matrix")

## Setup
First you need to download Qt6 Framework. Then be sure to extract the Include folder from Include.zip (it contains sqLite3, QMidi and RtMidi source files). On Windows use the Visual Studio solution (be sure to install the Qt plugin for Visual Studio). For Linux and MacOS - use the included *.pro file with Qt Creator.

## Contact
Created by [@TheFinalCut](https://github.com/thefinalcutbg) - feel free to contact me!

## License
This project is open source and available under the MIT license

