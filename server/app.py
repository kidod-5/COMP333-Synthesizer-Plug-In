from flask import Flask

app = Flask(__name__)   

@app.route("/")
def hello_world():
    return "<p>Hello, World!</p>"

@app.route("/signup")
def signup():
    return "<p>Signup Page</p>"

@app.route("/login")
def login():
    return "<p>Login Page</p>"

@app.route("/dashboard")
def dashboard():
    return "<p>Dashboard Page</p>"

if __name__ == "__main__":
    app.run(debug=True)