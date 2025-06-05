#!/bin/bash

echo "Compiling without Comedy movies..."
g++ -std=c++17 -Wall -Wextra -o movie_rental_no_comedy \
    main.cpp \
    movie.cpp \
    drama.cpp \
    classic.cpp \
    movie_factory.cpp \
    customer.cpp \
    transaction.cpp \
    borrow_command.cpp \
    return_command.cpp \
    inventory_command.cpp \
    history_command.cpp \
    command_factory.cpp \
    store.cpp

if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo "Running program without Comedy support..."
    ./movie_rental_no_comedy
else
    echo "Compilation failed!"
    exit 1
fi