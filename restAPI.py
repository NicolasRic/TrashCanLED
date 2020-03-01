from flask import Flask, jsonify
import time

app = Flask(__name__)

@app.route('/')
def index():
    return "Hello, world!"

@app.route('/TrashCanLED/next/', methods=['GET'])
def nextDelivery():
    return jsonify([{'R': 0, 'G': 0, 'B': 0}])

if __name__ == '__main__':
    app.run(host="0.0.0.0", debug=True)
