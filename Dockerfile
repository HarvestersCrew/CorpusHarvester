FROM gcc:latest
RUN apt update && apt install clang-format -y
WORKDIR /project
