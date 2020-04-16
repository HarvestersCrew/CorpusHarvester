FROM node:alpine
WORKDIR /project
RUN npm install http-server -g
ADD package.json package.json
ADD package-lock.json package-lock.json
RUN npm install
ADD . .
RUN npx vue-cli-service build
RUN mkdir /web && cp -r dist/* /web
WORKDIR /web
RUN rm -rf /project
CMD http-server . -p 8080 --proxy "http://localhost:8080?"
