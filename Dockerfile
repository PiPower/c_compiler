FROM ubuntu:25.10

USER root

RUN apt-get update

RUN apt-get -y install wget lsb-release gnupg  software-properties-common build-essential cmake 

WORKDIR /home/ubuntu/src

RUN wget https://apt.llvm.org/llvm.sh 
RUN chmod +x llvm.sh 
RUN ./llvm.sh 22
RUN ln -s /usr/bin/clang-22 /usr/bin/clang

COPY . .

RUN  rm -rf build && mkdir build && cd build && cmake .. && make -j12

