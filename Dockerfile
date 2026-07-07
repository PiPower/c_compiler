FROM ubuntu:24.04

USER root

RUN apt-get update

RUN apt-get -y install wget lsb-release gnupg  software-properties-common build-essential cmake nano

WORKDIR /home/ubuntu/src

RUN wget https://apt.llvm.org/llvm.sh 
RUN chmod +x llvm.sh 
RUN ./llvm.sh 22
RUN ln -s /usr/bin/clang-22 /usr/bin/clang
RUN ln -s /usr/bin/opt-22 /usr/bin/opt

COPY . .

RUN mkdir build && cd build && cmake .. && make -j12

RUN chmod +x ./compile_test.sh
#RUN ./compile_test.sh