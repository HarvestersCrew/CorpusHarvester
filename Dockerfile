FROM gcc:latest

WORKDIR /project
COPY . .
RUN make

ENTRYPOINT [ "bin/harvester" ]
