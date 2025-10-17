"""
Unit tests for backend logic functions.
"""



from server.app import hello_world, signup, login, dashboard


def hello_world_tests():
    if hello_world() == "<p>Hello, World!</p>":
        print("Test passed: hello_world function works correctly.")
    else:
        print("Test failed: hello_world function does not return expected output.")

def signup_tests():
    if signup() == "<p>Signup Page</p>":
        print("Test passed: signup function works correctly.")
    else:
        print("Test failed: signup function does not return expected output.")

def login_tests():
    if login() == "<p>Login Page</p>":
        print("Test passed: login function works correctly.")
    else:
        print("Test failed: login function does not return expected output.")

def dashboard_tests():
    if dashboard() == "<p>Dashboard Page</p>":
        print("Test passed: dashboard function works correctly.")
    else:
        print("Test failed: dashboard function does not return expected output.")
def run_all_tests():
        
    hello_world_tests()
    signup_tests()
    login_tests()
    dashboard_tests()
