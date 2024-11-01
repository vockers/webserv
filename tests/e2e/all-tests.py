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
	response = requests.get(f'{BASE_URL}/not-found')
	assert response.status_code == 404
	assert response.headers['Content-Type'] == 'text/html'
	assert "<h1>404 Not Found</h1>" in response.text
	assert "<p>Oops! Something went wrong.</p>" in response.text

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

    assert response.status_code == 201
    assert "<title>Upload Successful</title>" in response.text
    assert "<h1>File Uploaded Successfully!</h1>" in response.text

    response = requests.get(f'{BASE_URL}/upload/test.txt')

    assert response.status_code == 200
    assert response.text == "hello world\n"

    os.remove('tests/www/upload/test.txt') 

    response = requests.get(f'{BASE_URL}/upload/test.txt')

    assert response.status_code == 404

def test_autoindex():
    response = requests.get(f'{BASE_URL}/1/')
    
    assert response.status_code == 200
    assert response.headers['Content-Type'] == 'text/html'
    assert "<h1>Index of /1/</h1>" in response.text
    assert "<a href=\"/1/name.html\">name.html</a>" in response.text
    assert "<a href=\"/1/hello.html\">hello.html</a>" in response.text

def test_delete_method():
    file_data = {'file': ('test_delete.txt', 'sample content\n')}
    upload_response = requests.post(f'{BASE_URL}/upload/', files=file_data)

    assert upload_response.status_code == 201
    assert "File Uploaded Successfully!" in upload_response.text

    get_response = requests.get(f'{BASE_URL}/upload/test_delete.txt')
    assert get_response.status_code == 200
    assert get_response.text == "sample content\n"

    delete_response = requests.delete(f'{BASE_URL}/upload/test_delete.txt')

    assert delete_response.status_code == 200
    assert "File Deleted Successfully" in delete_response.text

    confirm_deletion_response = requests.get(f'{BASE_URL}/upload/test_delete.txt')
    assert confirm_deletion_response.status_code == 404
    
    delete_not_found_response = requests.delete(f'{BASE_URL}/upload/not_found.txt')
    assert delete_not_found_response.status_code == 404

def test_cgi_body():
	url = f'{BASE_URL}/cgi/body.py'
	data = "hello world"
	response = requests.post(url, data=data)
     
	assert response.status_code == 200
	assert response.text == data

def test_max_body_size():
    req = b"GET /2/hello.html HTTP/1.1\r\nHost: localhost\r\nContent-Length: 11\r\n\r\nhello world"

    response = requests_raw.raw(url=BASE_URL, data=req)

    assert response.status_code == 413

    req = b"GET /2/hello.html HTTP/1.1\r\nHost: localhost\r\nContent-Length: 10\r\n\r\nhello worl"

    response = requests_raw.raw(url=BASE_URL, data=req)

    assert response.status_code == 200

def test_limit_except():
    response = requests.delete(f'{BASE_URL}/3/')

    assert response.status_code == 405

    response = requests.get(f'{BASE_URL}/3/')

    assert response.status_code == 200

def test_redirect():
    response = requests.get(f'{BASE_URL}/redirect')

    assert response.status_code == 200
    assert response.text == "hello from /redirected.html\n"
