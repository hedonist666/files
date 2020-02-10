#!/bin/bash

if [[ "$1" =~ test* ]]; then 
  echo first
  g++ $1 -fconcepts -std=c++17 && ./a.out
elif [[ "$1" =~ \.js ]]; then
  node -r "$(pwd)/inspect.js"
else 
  g++ $1 -g -fconcepts -std=c++17 && ./a.out
fi
