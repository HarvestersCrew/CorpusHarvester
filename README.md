# CorpusHarvester

<p align="center">
    <img src="img/moissonneuse.png"/>
</p>

CorpusHarvester is a tool suite used to easily and quickly retrieve and create corpus of data from different websites. It can download from many default and user-described public APIs (Twitter, Flickr...) and export them as an archive. It can be used as a CLI tool to automate process, or as a web app for a more intuitive usage.

Currently, it is composed of four modules:
- Engine: C++, it is the core of the application which does the heavy lifting.
- CLI: C++, it builds against the engine to provide an easy-to-use command line interface.
- Server: C++, it uses websockets to offer an asynchronous interface to use the engine.
- Web client: Node / Vue.js, it connects to the server to offer a friendly user interface.

Two `docker-compose` files are provided to help setup the CorpusHarvester. The default one will build everything and run an image of MariaDB for the database, a container with the server running and the CLI available, and a container with the web client. The `devel` one is there to simplify development, assure that the setup is the same across the contributors and storing object files to avoid rebuilding everything each time.

## Requirements
These requirements are only needed where you plan to build the project. If using Docker, the provided Dockerfiles will take care of that.

### Engine libraries
- MySQL Connection/C++: `apt install libmysqlcppconn-dev`, connection to the MariaDB server
- libcurl: `apt install libcurl4-openssl-dev`, download of files
- libzip: `apt install libzip-dev`, extracting corpus

### Server libraries
- websocketpp: `apt install libwebsocketpp-dev`, expose websockets
- libasio: `apt install libasio-dev`, dependency of `websocketpp`
- seasocks: [Github](https://github.com/mattgodbolt/seasocks), HTTP server to distribute files

### Web client
Node needs to be installed to build the web client. A simple HTTP server can be used to serve those compiled files.

### Database
You'll need to run your own MariaDB / MySQL server if you do not use the bundled Docker version.
Using Docker, a web interface is running to play with the MySQL server from outside the Docker network, it is available at [localhost:8080](http://localhost:8080) by default.

### Others
- clang-format : `apt install clang-format`, used to lint the code at compile time (we are using LLVM format)

## Docker usage

### Production
First download the web git submodule with these commands: `git submodule init` and `git submodule update`.

Run `docker-compose build` then `docker-compose up` to have the full non-development suite started. The web client is on the port `8080`. Afterhand, do not forget to run `docker-compose down` to free the resources.

### Development
See the Makefile part below. It will start a MariaDB container, an Adminer container at [localhost:8080](http://localhost:8080) to browse the database and a development container to build and launch the executables in. No development container is provided for the web client.

## Makefile

### Build methods
- `make`: builds all native executables, dependencies need to be resolved on the machine if something goes wrong.
- `make bin/(EXEC_NAME)`: builds an executable into `bin/(EXEC_NAME)` by linking the engine and all sources in `EXEC_NAME`, including a main function.
- `make docker`: starts a set of Docker containers and builds the executables in Docker.
- `make docker/(EXEC_NAME)` to specify a build.

Valid `EXEC_NAME` are:
- `cli` to build the CLI client
- `server` to build the server
- `tests` to build the test suite

Both the classic `make` and `make docker` methods resolve in making an executable in the `bin` folder. The first one is native to the machine, the second one is running through Docker. You may have to apply `sudo` to both Docker methods, depending on how you usually run it. The Docker executable are simply shell scripts passing through all parameters and should behave the same way as the native one in interface.

### Phony methods
- `make clean`: cleans `bin` and `obj` folders.
- `make docker/clean`: cleans `bin` and `obj` folders in Docker and local (launch scripts).
- `make docker/up`: ups the containers.
- `make docker/down`: shutdowns the containers.
- `make docker/bash`: launches a bash shell in the container running the builds.
- `make docker/movejson`: copies the API JSONs to the `/tmp/stored` folder (the default CorpusHarvester folder) in the Docker container.
- `make format`: formats all code to the LLVM standard. Note that any compiling file is also checked and won't pass if not applying to LLVM.

## How to use the command line interface
Please refer to the documentation in [doc/cli.md](doc/cli.md).

## Add your own APIs
The Harvester will look for API descritpion files in the `apis` subfolder of its storage root. By default we include a few of them which can be found in [data/apis/](data/apis/) and are copied to the application folder at compile time. To make your own, you can take inspiration from those existing and use the documentation written in [doc/api_json.md](doc/api_json.md).
