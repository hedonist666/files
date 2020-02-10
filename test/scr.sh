#!/bin/bash

if [[ "$1" =~ test* ]]; then 
  g++ $1 -fconcepts -std=c++17 && ./a.out
else 
  g++ $1 -g -fconcepts -std=c++17 && ./a.out
fi
