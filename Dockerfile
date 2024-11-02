FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    iputils-ping \
    iproute2 \
    net-tools

WORKDIR /app
COPY . .

RUN cmake . && make 