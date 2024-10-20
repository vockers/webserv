import requests

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
