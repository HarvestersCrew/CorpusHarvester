# CorpusHarvester
CS Engineering School Project

## Requirements
- MySql Connection/C++ : `sudo apt install libmysqlcppconn-dev`
- libcurl : `sudo apt install libcurl4-openssl-dev`

## Makefile

### Build and run methods
- `make`: builds all native executables, dependencies need to be resolved on the machine if something goes wrong.
- `make bin/(FOLDER_NAME)`: builds an executable into `bin/(FOLDER_NAME)` by linking the engine and all sources in `FOLDER_NAME`, including a main function.
- `make docker`: starts a set of Docker containers and builds the executables in Docker.
- `make docker/(FOLDER_NAME)` to specify a build.

Both those methods resolve in making an executable in `bin/cli` (or `bin/tests` for the tests). The first one is native to the machine, the second one is running through Docker. You may have to apply `sudo` to both Docker methods, depending on how you usually run it. Finally, the Docker executable passes through all parameters and should behave the same way as the native one.

### Phony methods
- `make clean`: cleans `bin` and `obj` folders.
- `make docker/clean`: cleans `bin` and `obj` folders in Docker and local (launch scripts).
- `make docker/up`: ups the containers.
- `make docker/down`: shutdowns the containers.
- `make docker/bash`: launches a bash shell in the container running the builds.
- `make format`: formats all code to the LLVM standard. Note that any compiling file is also checked and won't pass if not applying to LLVM.

## MySQL
Using Docker, a web interface is running to play with the MySQL server from outside the Docker network, it is available [here](http://localhost:8080).

## How to use the client line interface
Please refer to the documentation in the 'cli' folder.
