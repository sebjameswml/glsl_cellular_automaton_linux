#!/bin/bash
cd headers
g++ -I ../include/ -c *.cpp
cd ..
g++ -I include/ main.cpp glad.c headers/*.o -lGL -lm -lX11 -lpthread -lXi -lXrandr -ldl /usr/lib/x86_64-linux-gnu/libglfw.so -o main
