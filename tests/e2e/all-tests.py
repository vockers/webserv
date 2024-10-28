import requests
import requests_raw
import os

BASE_URL = "http://localhost:8080"
BASE_URL_2 = "http://localhost:8081"

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

def test_missing_host():
    req = b"GET / HTTP/1.1\r\n\r\n"

    response = requests_raw.raw(url=BASE_URL, data=req)

    assert response.status_code == 400
    assert response.headers['Content-Type'] == 'text/html'

def test_server_name():
    req_1 = b"GET /name.html HTTP/1.1\r\nHost: localhost1:8080\r\n\r\n"

    response_1 = requests_raw.raw(url=BASE_URL, data=req_1)

    assert response_1.status_code == 200
    assert response_1.text == "name in /1\n"

    req_2 = b"GET /name.html HTTP/1.1\r\nHost: localhost2:8080\r\n\r\n"

    response_2 = requests_raw.raw(url=BASE_URL, data=req_2)

    assert response_2.status_code == 200
    assert response_2.text == "name in /2\n"

def test_default_server():
    req = b"GET /name.html HTTP/1.1\r\nHost: foo\r\n\r\n"

    response = requests_raw.raw(url=BASE_URL, data=req)

    assert response.status_code == 200
    assert response.text == "name in /\n"

def test_second_server():
    response = requests.get(f'{BASE_URL_2}/name.html')

    assert response.status_code == 200
    assert response.text == "name in /\n"

def test_upload():
    files = {'file': ('test.txt', 'hello world\n')}
    response = requests.post(f'{BASE_URL}/upload/', files=files)

    assert response.status_code == 200
    assert response.text == "File uploaded"

    response = requests.get(f'{BASE_URL}/upload/test.txt')

    assert response.status_code == 200
    assert response.text == "hello world\n"

    os.remove('tests/www/upload/test.txt') 

    response = requests.get(f'{BASE_URL}/upload/test.txt')

    assert response.status_code == 404

def test_autoindex():
    expected_body = '''\
<!DOCTYPE html>
<html lang=\"en-US\"><head><meta charset=\"utf-8\" />
    <head>
        <title>Index of /1/</title>
    </head>
    <body>
        <h1>Index of /1/</h1>
        <ul>
            <li><a href="name.html">name.html</a></li>
            <li><a href="hello.html">hello.html</a></li>
        </ul>
        <hr/><p style='text-align: center;'>webserv</p>
    </body>
</html>
'''

    response = requests.get(f'{BASE_URL}/1/')

    assert response.status_code == 200
    assert response.text == expected_body

def test_delete_method():
    response = requests.delete(f'{BASE_URL}/2/')

    assert response.status_code == 200
    assert response.text == "hello in /2\n"
