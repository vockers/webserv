FROM ubuntu:22.04

# Install necessary dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    libgtest-dev \
    googletest

# Set up working directory
WORKDIR /app

# Copy the source code to the working directory
COPY ../. .

# Compile the source code
RUN clang++ -std=c++20 -I./include -o test \
    src/config/Config.cpp \
    src/config/Lexer.cpp \
    src/config/Parser.cpp \
    tests/config/config_tests.cpp \
    tests/config/lexer_tests.cpp \
    tests/config/parser_tests.cpp \
    -lgtest -lgtest_main -pthread

# Run the tests
CMD ["./test"]
