<?php session_start();?>
<?php include "header.php" ?>
<?php
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
        echo "May name not correct\n";
        exit();
    }
    else
    {
        echo "No Such Information About This User\n";
        exit();
    }
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
    <input type="submit" name="signin" value="Sign In" class="btn btn-primary">
    </form>
    <form action="" method="post">
        <input type="submit" name="register" value="register" class="btn btn-primary">
    </form>
  </div>