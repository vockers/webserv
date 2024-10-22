#!/usr/bin/env python3

# Print HTTP headers required by CGI
print("Content-Type: text/html")
print()  # End of headers

# Print the actual content
print("<html>")
print("<head><title>CGI Test</title></head>")
print("<body>")
print("<h1>Hello from the CGI script!</h1>")
print("</body></html>")