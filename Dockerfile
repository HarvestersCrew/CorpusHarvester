FROM gcc:latest AS gcc_debug
RUN apt update && apt install clang-format libmysqlcppconn-dev libasio-dev libwebsocketpp-dev libzip-dev -y
WORKDIR /project
RUN mkdir /tmp/stored
