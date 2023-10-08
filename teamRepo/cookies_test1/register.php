#!/usr/bin/php-cgi

<!-- <?php include "header.php" ?> -->

<?php
    session_start();
if ($_SERVER['REQUEST_METHOD'] === 'POST') {

    $input = file_get_contents("php://stdin");
    parse_str($input, $_POST);

    $id = uniqid();
    setcookie('userid', $id, time() + 3600);
    setcookie('name', $_POST['name'], time() + 3600);
    
    $_SESSION['userid'] = $id;
    $_SESSION['email'] = $_POST['email'];
    $_SESSION['color'] = $_POST['color'];
    $_SESSION['name'] = $_POST['name'];
    header('location: platform.php?userid='. $_SESSION['userid']);
    exit();
}
?>

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
        <label for="email" class="form-label">Email ID</label>
        <input type="email" class="form-control" name="email" placeholder="Enter your email" autocomplete="off" required>
        <small class="text-muted">Your email is safe with us.</small>
    </div>
    <div class="mb-3">
        <label for="color" class="form-label">Set Your Favorite Color</label>
        <input type="color" name="color" value="#000000">
    </div>
    <div class="mb-3">
        <input type="submit" name="signin" value="Sign In" class="btn btn-primary">
    </div>
    </form>
</div>