import httplib

conn = httplib.HTTPConnection('127.0.0.1', 8081)

conn.request("POST", "/bimaple/index/commit", "")
response = conn.getresponse()
print response.read()

