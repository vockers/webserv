FROM ubuntu:20.04

EXPOSE 8080

# Add the PPA for newer GCC versions
RUN apt-get update && apt-get install -y software-properties-common \
    && add-apt-repository ppa:ubuntu-toolchain-r/test \
    && apt-get update

# Install necessary packages including the new GCC version
RUN apt-get install -y \
    clang \
    build-essential \
    gcc-13 \
    g++-13 \
    make \
    vim

# Set g++-13 as the default version of g++
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100

# Set the working directory inside the container
WORKDIR /app

# The actual code will be mounted via a volume, so we don't copy it during the build step.
# We'll mount the local directory at runtime.

# You can run 'make' at runtime to build the project if you mount the volume
CMD ["bash"]
