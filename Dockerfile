FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    g++ \
    gdb \
    cmake \
    git \
    wget \
    libasio-dev \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

RUN wget -q https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp -P /usr/include/nlohmann/

CMD ["/bin/bash"]