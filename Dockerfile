FROM gcc:latest
RUN apt update && apt install clang-format libmysqlcppconn-dev -y
WORKDIR /project
