<?php 

session_start();

if(!isset($_SESSION['email'])){
	header('location:login.php');
}else{

?>

<!DOCTYPE html>
<html>
<head>
    <title>Name Submission Form</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f3f3f3;
        }

        .container {
            max-width: 400px;
            margin: 0 auto;
            padding: 20px;
            background-color: #ffffff;
            border: 1px solid #ccc;
            border-radius: 5px;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
        }

        .form-group {
            margin: 10px 0;
			padding-right: 20px;
        }

        .form-group label {
            display: block;
            font-weight: bold;
        }

        .form-input {
            width: 100%;
            padding: 10px;
            border: 1px solid #ccc;
            border-radius: 5px;
        }

        .submit-button {
            background-color: #007bff;
            color: #fff;
            padding: 10px 20px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }

    </style>
</head>
<body>
    <div class="container">
        <h2>Name Submission Form</h2>
        <form action="/redir/hello_get.py" method="get">
            <div class="form-group">
                <label for="first_name">First Name:</label>
                <input type="text" name="first_name" id="first_name" class="form-input" required>
            </div>
            <div class="form-group">
                <label for="last_name">Last Name:</label>
                <input type="text" name="last_name" id="last_name" class="form-input" required>
            </div>
            <input type="submit" value="Submit" class="submit-button">
        </form>
    </div>
</body>
</html>


<?php  } ?>

