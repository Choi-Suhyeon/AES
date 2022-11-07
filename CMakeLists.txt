cmake_minimum_required(VERSION 3.23)
project(AES C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(AES
        aes_enc_dec.c
        aes_enc_dec.h
        file_io.c
        file_io.h
        main.c)