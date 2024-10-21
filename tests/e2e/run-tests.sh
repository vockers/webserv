#!/bin/bash

./webserv ./tests/conf/e2e.conf > /dev/null 2>&1 &

source venv/bin/activate

pytest tests/e2e/all-tests.py
