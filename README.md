# Grayscale Image Compression App

## Description

## Table of Contents
1. [Installation](#installation)
2. [Launch](#launch)
3. [Usage](#features)
4. [Further-Work](#further-work)

## Installation

1. Clone the repository
```
	git clone https://github.com/hareng-fume/bmp-compression.git
```

2. Navigate to the project directory
```
   cd pocketbook-bmp
```

3. Launch <b>build.sh</b> with arguments in shell script Cygwin, Git Bash terminals

   Usage (Release is set by default): 
```
	./build.sh [gnu|msvc] [Debug|Release]
```


4. <b>QT_DIR</b> is important to check and change if necessary (used <b>Qt version 6.5.3</b>)  
         
5. Qt dependencies are <b>deployed by windeployqt.exe</b>. Make sure it exists in <b>$QT_DIR/bin</b>.

   <b>Alternatively: </b>:   
&nbsp;&nbsp;&nbsp;&nbsp;-- Open in Qt Creator: File-> Open File or Project...<br />
&nbsp;&nbsp;&nbsp;&nbsp;-- Find bmp-compression dir and select CMakeLists.txt.<br />
&nbsp;&nbsp;&nbsp;&nbsp;-- Switch to 'Projects' mode in mode selector panel.<br />
&nbsp;&nbsp;&nbsp;&nbsp;-- Customize Build & Run and then Build all projects.<br />

## Launch

&nbsp;&nbsp;&nbsp;&nbsp;To start the application

&nbsp;&nbsp;&nbsp;&nbsp;If 'Alternatively' option was chosen, find an executable in the binaries output directory that was specified.

&nbsp;&nbsp;&nbsp;&nbsp;If not, in the <b>project root directory next to the ./sources in ./bin folder</b>, depending on the build.sh arguments, app.exe could be found in one of the nested folders<br />
&nbsp;&nbsp;&nbsp;&nbsp;<b>e.g. in ./bin/msvc_release/app.exe</b>
  
	- app.exe --dir <images-dir>
	- app.exe                           (current working directory is treated an <images-dir>)
	- app.exe --dir <images-dir> --test (to launch unit-tests, this works in Debug only)

## Usage
   
&nbsp;&nbsp;&nbsp;&nbsp;In the application main window, below listed items are arranged sequentially vertically:<br />
&nbsp;&nbsp;&nbsp;&nbsp;- Working-dir absolute address<br />
&nbsp;&nbsp;&nbsp;&nbsp;- Filter by image extension combo-box<br />
&nbsp;&nbsp;&nbsp;&nbsp;- List-view<br />

&nbsp;&nbsp;&nbsp;&nbsp;***Note:*** Having resulting *-packed.barch/*-unpacked.bmp file path in the model, it's not gonna be added to the 
         model as a duplicate since its content is overwritten.
	 
## Further-Work

	- Add Error Handling (Exceptions, Logging, etc.)
	- Fix tiny image data shift after image restoration.
