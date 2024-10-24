#!/usr/bin/env python3

import os

# Print all environment variables
for key, value in os.environ.items():
    print(f"{key}: {value}")
