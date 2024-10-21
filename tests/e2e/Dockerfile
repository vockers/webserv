FROM ubuntu:23.10

SHELL ["/bin/bash", "-c"]

# Install necessary dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    python3 \
    python3-pip \
    python3-venv \
    && rm -rf /var/lib/apt/lists/*

# Set up working directory
WORKDIR /app

# Copy the source code to the working directory
COPY ../. .

RUN make all

# Setup Python environment
RUN python3 -m venv venv && \
    source venv/bin/activate && \
    pip install requests pytest requests-raw

RUN chmod +x tests/e2e/run-tests.sh

# Run the tests
CMD ["./tests/e2e/run-tests.sh"]
