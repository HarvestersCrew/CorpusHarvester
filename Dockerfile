FROM gcc:latest

WORKDIR /project
RUN PATH="/project/bin:$PATH"
RUN export PATH

CMD [ "bash" ]
