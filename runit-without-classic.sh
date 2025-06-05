#!/bin/bash

echo "Compiling without Classic movies..."
g++ -std=c++17 -Wall -Wextra -o movie_rental_no_classic \
    main.cpp \
    movie.cpp \
    comedy.cpp \
    drama.cpp \
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
    echo "Running program without Classic support..."
    ./movie_rental_no_classic
else
    echo "Compilation failed!"
    exit 1
fi