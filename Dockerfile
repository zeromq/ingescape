FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends ca-certificates build-essential sudo cmake g++ && apt-get clean

WORKDIR /build
COPY . .
RUN ./install_on_system.sh && ldconfig
