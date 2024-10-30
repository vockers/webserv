#!/usr/bin/env python3

import cgi
import os
import sys
import cgitb
cgitb.enable()

body = sys.stdin.read()

#read request headers from the server
# headers_env = dict(os.environ)
# print(headers_env)

print(body, end='')