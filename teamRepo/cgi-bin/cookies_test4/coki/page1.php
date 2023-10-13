<?php 

session_start();

if(!isset($_SESSION['email'])){
	header('location:login.php');
}else{

?>

<!DOCTYPE html>
<html>
<head>
    <title>File Upload</title>
    <style>
        body {
            font-family: Arial, sans-serif;
        }

        .container {
            max-width: 400px;
            margin: 0 auto;
            padding: 20px;
            border: 1px solid #ccc;
            border-radius: 5px;
            background-color: #f9f9f9;
        }

        .form-group {
            margin: 10px 0;
        }

        .form-group label {
            display: block;
            font-weight: bold;
        }

        .file-input {
            width: 100%;
            padding: 10px;
        }

        .btn-submit {
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
        <h2>File Upload Form</h2>
        <form action="upload.php" method="post" enctype="multipart/form-data">
            <div class="form-group">
                <label for="file">Choose a file:</label>
                <input type="file" name="file" id="file" class="file-input" accept=".pdf, .doc, .docx">
            </div>
            <input type="submit" value="Upload" class="btn-submit">
        </form>
    </div>
</body>
</html>


<?php  } ?>

