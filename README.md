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

Both those methods resolve in making an executable in `bin/cli` (or `bin/tests` for the tests or `bin/server` for the server). The first one is native to the machine, the second one is running through Docker. You may have to apply `sudo` to both Docker methods, depending on how you usually run it. Finally, the Docker executable passes through all parameters and should behave the same way as the native one.

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
Please refer to the documentation in 'doc/cli.md'.

## Web client
The web client is made with Node in Vue.js. If you wish to run it, you have the possibility to build it yourself (or fire up the dev server) if you have Node installed, or through Docker.

### Node installed
Go into the web folder, type `npm install` and `npx vue-cli-service build`. You then have a `dist/` folder you can distribute via a HTTP server. For the convenience, a dev package is also given with the packages. Run it with `npx dist/ -p 8080 --proxy "http://localhost:8080?"` (by adapting the two ports if necessary). You'll need to have the server running, either via Docker or natively.

### Full docker package
If you have Docker installed, just run `docker-compose -f docker-compose.yml -f docker-compose.demo.yml up` to have the full suite started. The web client is on the port `8081`.
