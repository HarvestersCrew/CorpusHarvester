FROM gcc:latest
RUN apt update && apt install clang-format libmysqlcppconn-dev libasio-dev libwebsocketpp-dev -y
WORKDIR /project
RUN mkdir /tmp/stored
