usage = '''
simple flask server to test 

requirements
-----------
- python3 https://www.python.org/
- flask https://flask.palletsprojects.com/en/1.1.x/

SETUP
-----
$ pip install flask

USAGE
-----
$ python3 server.py

$ curl -i -X POST -H "Content-Type: application/json" -d '{"t":18}' http://127.0.0.1:5000/data
HTTP/1.0 201 CREATED
Content-Type: application/json
Content-Length: 18
Server: Werkzeug/1.0.0 Python/3.7.6
Date: Sat, 26 Dec 2020 18:13:37 GMT

{
  "msg": "OK"
}

$ curl -i http://127.0.0.1:5000/data                                                          
HTTP/1.0 200 OK
Content-Type: application/json
Content-Length: 34
Server: Werkzeug/1.0.0 Python/3.7.6
Date: Sat, 26 Dec 2020 18:14:37 GMT

[
  {
    "temperature": 18
  }
]

'''

import sys
try:
    from flask import Flask, request, jsonify
except ModuleNotFoundError as e:
    print(e)
    print(usage)
    sys.exit(1)


app = Flask(__name__)

data = []

@app.route("/data", methods=["POST", "GET"])
def data_view():
    if request.method == "POST":
        try:
            d = request.get_json()
        except Exception as e:
            print(e)
            return jsonify({"msg":"error decoding body"}), 400

        t = d.get("t", None)
        if t == None:
            return jsonify({"msg":"no data"}), 400
        try:
            t = int(t)
        except ValueError:
            return jsonify({"msg":"invalid temp"}), 400
        data.append({"temperature":t})
        return jsonify({"msg":"OK"}), 201
    
    return jsonify(data), 200


if __name__ == "__main__":
    #run on all net ifaces
    app.run("0.0.0.0", 5000, debug=True)

        
        
