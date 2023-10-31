# Import modules for CGI handling
import cgi

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
first_name = form.getvalue('first_name')
last_name = form.getvalue('last_name')

# Print the Content-Type header
print("Content-type: text/html\n")

# HTML structure with improved design
print("<!DOCTYPE html>")
print("<html>")
print("<head>")
print("<title>Hello - CGI Program</title>")
print("<style>")
print("  body {")
print("    font-family: Arial, sans-serif;")
print("    text-align: center;")
print("    background-color: #f2f2f2;")
print("  }")
print("  .container {")
print("    max-width: 400px;")
print("    margin: 0 auto;")
print("    padding: 20px;")
print("    background-color: #fff;")
print("    box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2);")
print("  }")
print("</style>")
print("</head>")
print("<body>")
print("<div class='container'>")
print("<h2>Hello, {} {}</h2>".format(first_name, last_name))
print("</div>")
print("</body>")
print("</html>")
