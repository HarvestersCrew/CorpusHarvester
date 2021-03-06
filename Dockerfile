FROM gcc:latest AS gcc_debug
RUN apt update && apt install gdb valgrind cmake clang-format libmysqlcppconn-dev libasio-dev libwebsocketpp-dev libzip-dev -y

WORKDIR /seasocks
RUN git clone https://github.com/mattgodbolt/seasocks.git . && mkdir -p build
WORKDIR /seasocks/build
RUN cmake .. && make -j8 && make install
RUN ldconfig

WORKDIR /project
RUN rm -rf /seasocks

FROM gcc_debug AS gcc_build
WORKDIR /project
COPY cli cli
COPY engine engine
COPY server server
COPY makefile makefile
RUN make clean && make bin/server && make bin/cli
RUN mv bin/cli /usr/local/bin/harvester_cli
RUN mv bin/server /usr/local/bin/harvester_server
RUN rm -rf *
ADD data /tmp/stored
CMD harvester_server
