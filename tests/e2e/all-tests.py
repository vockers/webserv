import requests

BASE_URL = "http://localhost:8080"

def test_index():
	expected_body = '''\
	<!DOCTYPE html>
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
	assert response.headers['Content-Length'] == '159'
