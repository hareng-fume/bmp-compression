# Grayscale Image Compression App

## Description

## Table of Contents
1. [Installation](#installation)
2. [Launch](#launch)
3. [Usage](#features)
4. [Further-Work](#further-work)

## Installation

1. Clone the repository
   '''bash
   git clone https://github.com/hareng-fume/bmp-compression.git

2. Navigate to the project directory
   cd pocketbook-bmp
   
3. Launch build.sh with arguments in shell script Cygwin, Git Bash terminals

   Usage: ./build.sh [gnu|msvc] [Debug|Release], Release is set by default

   Note: QT_DIR is important to check and change if necessary
         Used Qt version 6.5.3 
         
	 Qt dependencies are deployed by windeployqt.exe. Make sure it exists in $QT_DIR/bin/windeployqt.exe.

   OR:   Open in Qt Creator: File-> Open File or Project... find bmp-compression dir, select CMakeLists.txt.
         In mode selector switch to Projects mode, customize Build & Run and then Build all projects.

## Launch

   To start the application

   If 'OR' option was chosen in 3. find an executable in the binaries output directory that was specified.

   If not, in the root directory next to the ./sources in ./bin folder, depending on the build.sh arguments specified,
   app.exe could be found in one of the nested folders e.g. in ./bin/msvc_release/app.exe
  
	- app.exe --dir <images-dir>
	- app.exe                           (current working directory is treated an <images-dir>)
        - app.exe --dir <images-dir> --test (to launch unit-tests, this works in Debug only)

## Usage
   
   In the application main window, below listed items are arranged sequentially vertically:
	- working-dir absolute address
	- filter by image extension combo-box
	- list-view

   Note: Having resulting *-packed.barch/*-unpacked.bmp file path in the model, it's not gonna be added to the 
         model as a duplicate since its content is overwritten.
	 
## Further-Work

	- Add Error Handling (Exceptions, Logging, etc.)
	- Fix tiny image data shift after image restoration.
