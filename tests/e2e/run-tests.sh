#!/bin/bash

./webserv ./tests/e2e.conf > /dev/null 2>&1 &

source venv/bin/activate

pytest tests/all-tests.py
