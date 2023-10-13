<?php
session_start();
if (isset($_COOKIE['userid']) && isset($_COOKIE['name']) && isset($_SESSION['userid']) && isset($_SESSION['email']) && isset($_SESSION['color']))
{
    if ($_COOKIE['userid'] === $_SESSION['userid'] && $_COOKIE['name'] === $_SESSION['name'])
    {
        header('location: platform.php?userid='. $_SESSION['userid']);
        exit();
    }
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $input = file_get_contents("php://stdin");
    parse_str($input, $_POST);
    if (isset($_POST['register']))
    {
        header('location: register.php');
        exit();
    }
    if (isset($_COOKIE['userid'])&& isset($_SESSION['userid']) && isset($_SESSION['email']) && isset($_SESSION['color']))
    {
        if ($_COOKIE['userid'] === $_SESSION['userid'] && $_SESSION['name'] === $_POST['name'])
        {
            setcookie('name', $_POST['name'], time() + 3600);
            header('location: platform.php?userid='. $_SESSION['userid']);
            exit();
        }
        echo "<script>alert('Wrong UserName -');</script>";
    }
    else
    {
        echo "<script>alert('No information about you');</script>";
    }
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Login System</title>

    <!-- Bootstrap CSS -->
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-KyZXEAg3QhqLMpG8r+8fhAXLRk2vvoC2f3B09zVXn8CA5QIVfZOJ3BCsw2P0p/We" crossorigin="anonymous">

    <!-- Font Awesome Icons -->
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">

</head>
<body style='--bs-bg-opacity: .5;'>
<div class="container col-4 border rounded bg-light mt-5" style='--bs-bg-opacity: .5;'>
    <h1 class="text-center">Sign In</h1>
    <hr>
    <form action="" method="post">
    <div class="mb-3">
        <label for="username" class="form-label">UserName</label>
        <input type="text" class="form-control" name="name" placeholder="Enter your UserName" autocomplete="off" required>
        <small class="text-muted">Your UserName is safe with us.</small>
    </div>
    <div class="mb-3">
    <input type="submit" name="signin" value="Sign In" class="btn btn-primary">
    </form>
    </div>
    <div class="mb-3">
    <form action="" method="post">
        <input type="submit" name="register" value="register" class="btn btn-primary">
    </form>
    </div>
</div>