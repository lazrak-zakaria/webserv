#!/usr/bin/env python3

import cgi
import cgitb
import hashlib

# Enable CGI traceback in case of errors (disable in production)
cgitb.enable()

# Simulated user database (replace with a real database)
user_db = {
    "user1": "password1",
    "user2": "password2"
}

# Function to check user credentials
def authenticate(username, password):
    if username in user_db and user_db[username] == hashlib.sha256(password.encode()).hexdigest():
        return True
    return False

# Get form data from the CGI request
form = cgi.FieldStorage()

# Check if the form is submitted
if "username" in form and "password" in form:
    username = form["username"].value
    password = form["password"].value

    # Authenticate the user
    if authenticate(username, password):
        print("Content-Type: text/html")
        print()
        print("<html><body>")
        print("<h1>Welcome, {}!</h1>".format(username))
        print("</body></html>")
    else:
        print("Content-Type: text/html")
        print()
        print("<html><body>")
        print("<h1>Invalid username or password.</h1>")
        print("</body></html>")
else:
    # Display the login form
    print("Content-Type: text/html")
    print()
    print("<html><body>")
    print("<h1>Login</h1>")
    print('<form action="login.py" method="post">')
    print('  <label for="username">Username:</label>')
    print('  <input type="text" name="username" required><br><br>')
    print('  <label for="password">Password:</label>')
    print('  <input type="password" name="password" required><br><br>')
    print('  <input type="submit" value="Login">')
    print('</form>')
    print("</body></html>")
