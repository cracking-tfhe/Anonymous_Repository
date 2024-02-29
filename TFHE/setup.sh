#!/bin/bash
apt-get update
apt-get -y install cmake protobuf-compiler
git clone https://github.com/tfhe/tfhe.git
cd tfhe
make
make install
cd ..
apt-get -y install python3
apt-get -y install sagemath
