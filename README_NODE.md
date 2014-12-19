# How to build and run Node.JS modules

## Linux

### Folders

 - `PYTHON_PATH` - path to python.exe (e.g. /python26/python.exe)
 - `QMINER_ROOT` - qminer root folder (location of qminer's git repository)
 - `NODE_ROOT`   - folder with node source code (needed to build modules)

### Prerequisites

 - Python v2.7.3 is recommended. It was also tested on python 2.6. It doesn't work on python 3.

#### NPM (Node Package Manager)

	sudo apt-get install npm; sudo npm update
	sudo apt-get install uuid-dev

#### Node.JS source code

Modules were tested with Node.JS 0.11.14. Node-gyp can download source code as well, but it is not sure that the versions will match.

	wget https://github.com/joyent/node/archive/v0.11.14.zip
	unzip package.zip -d NODE_ROOT
	cd NODE_ROOT/
	./configure
	make
	sudo make install

#### Node GYP

Build system for Node.JS modules.

	sudo npm install -g node-gyp

#### Mocha and Chai (for testing)

	sudo npm install -g mocha
	sudo npm install -g chai

### Building modules

Safe way:

	node-gyp configure --python PYTHON_PATH --nodedir=NODE_PATH
	node-gyp build --python PYTHON_PATH --nodedir=NODE_PATH

Simple way, when correct version of Python is in path and node-gyp takes care of node's source code:

	node-gyp configure
	node-gyp build

### Running tests

	mocha test/nodejs/


## Windows

### Prerequisites:

 - Python v2.7.3 is recommended. It was also tested on python 2.6. It doesn't work on python 3.
 - Visual studio 2013

(*) Folders
PYTHON_PATH: path to python.exe: e.g. c:\python26\python.exe
QMINER_ROOT qminer root folder
NODE_ROOT folder with node source code

> set PYTHON_PATH=c:\Python27
> set NODE_ROOT=D:\work\code\cpp\node
> set QMINER_ROOT=D:\work\code\cpp\qminer


(*) Install node and build la addon

(1) Node: Download https://github.com/joyent/node/archive/v0.11.14.zip 
- Unzip to NODE_ROOT
> cd %NODE_ROOT%
> set PATH=%PYTHON_PATH%;%PATH%
> vcbuild clean nosign
> vcbuild release x64 nosign

(2) NPM (node package manager)
> set PATH=%NODE_ROOT%\Release;%PATH%
- Download npm: http://nodejs.org/dist/npm/
- Extract npm to %NODE_ROOT%\Release;
> cd %NODE_ROOT%\Release
> npm update

(3) Qminer
> git clone https://github.com/qminer/qminer.git %QMINER_ROOT%

(4) Gyp, build addon
> cd %NODE_ROOT%\Release
> npm install node-gyp -g
> cd %QMINER_ROOT%\src\qminer\nodejs\la
> node-gyp configure build --python %PYTHON_PATH%\python.exe --nodedir=%NODE_ROOT%

HOW TO - Running Mocha with Chai, unit test files and examples

Install Testing Framework:
1. Install mocha: "npm install -g mocha"
2. If you want to use Chai assertion library install chai: "npm install -g chai"
3. For running tests, use a file named test.js
 
NOTE: test-assert.js uses standard Node.js assertion library with fewer testing 
capabilities (Shown below only as an example) Recommended is the use of Chai library with test-chai.js file.
 
4. Run command "mocha" to execute all tests