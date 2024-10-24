import requests
import requests_raw

BASE_URL = "http://localhost:8080"

def test_index():
	expected_body = '''\
<!doctype html>
<html lang="en-US">
    <head>
        <meta charset="utf-8" />
        <title>Index</title>
    </head>
    <body>
        <a href="hi.txt">hi.txt</a>
    </body>
</html>
'''

	response = requests.get(f'{BASE_URL}/')

	assert response.status_code == 200
	assert response.text == expected_body
	assert response.headers['Content-Type'] == 'text/html'
	assert response.headers['Content-Length'] == '188'

def test_favicon():
    response = requests.get(f'{BASE_URL}/favicon.ico')

    assert response.status_code == 200
    assert response.headers['Content-Type'] == 'image/x-icon'

def test_not_found():
    expected_body = '''\
<!DOCTYPE html>
<html lang="en-US"><head><meta charset="utf-8" />
    <head>
        <title>404 Not Found</title>
    </head>
    <body>
        <h1 align="center">404 Not Found</h1>
        <p align="center">webserv</p>
    </body>
</html>
'''
    response = requests.get(f'{BASE_URL}/not-found')

    assert response.status_code == 404
    assert response.text == expected_body
    assert response.headers['Content-Type'] == 'text/html'

def test_not_implemented():
    response = requests.head(f'{BASE_URL}/')

    assert response.status_code == 501
    assert response.headers['Content-Type'] == 'text/html'

def test_http_version_not_supported():
    req = b"GET / HTTP/1.2\r\nHost: localhost:8080\r\n\r\n"

    response = requests_raw.raw(url=BASE_URL, data=req)

    assert response.status_code == 505
    assert response.headers['Content-Type'] == 'text/html'

def test_bad_request():
    req = b"GET / \r\nHost: localhost:8080\r\n\r\n"

    response = requests_raw.raw(url=BASE_URL, data=req)

    assert response.status_code == 400
    assert response.headers['Content-Type'] == 'text/html'
    
def test_cgi_env():
	expected_body = '''\
HTTP_HOST: localhost:8080
HTTP_USER_AGENT: python-requests/2.32.3
HTTP_ACCEPT_ENCODING: gzip, deflate
SERVER_PROTOCOL: HTTP/1.1
QUERY_STRING: 
HTTP_ACCEPT: */*
REQUEST_URI: /cgi/env.py
HTTP_CONNECTION: keep-alive
REQUEST_METHOD: GET
LC_CTYPE: C.UTF-8
'''

	response = requests.get(f'{BASE_URL}/cgi/env.py')

	assert response.status_code == 200
	assert response.text == expected_body
	