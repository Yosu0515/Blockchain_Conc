cmake_minimum_required(VERSION 3.9)
project(Blockchain_Conc)

set(CMAKE_CXX_STANDARD 11)

add_executable(Blockchain_Conc main.cpp Sources/Block.cpp Sources/Blockchain.cpp Headers/Block.h Headers/TransactionData.h Headers/Blockchain.h Headers/Threadpool.h)