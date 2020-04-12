FROM gcc:latest
RUN apt update && apt install clang-format libmysqlcppconn-dev libasio-dev libwebsocketpp-dev libzip-dev -y
WORKDIR /project
RUN mkdir /tmp/stored
