<?php
session_start();
function validate_credential($email, $pass, $id)
{
    $file = fopen("inputfile.txt", 'r');
    if ($file)
    {
        while ($line = fgets($file))
        {
            $comp = explode("\n", $line);
            $components = explode(";", $comp[0]);
            $id_tmp = 'userid=';
            $id_tmp .= $id;
            $email_tmp = 'email=';
            $email_tmp .= $email;
            $pass_tmp = 'password=';
            $pass_tmp .= $pass;
            if ($components[0] === $id_tmp && $components[1] === $email_tmp && $components[2] === $pass_tmp)
            {
                fclose($file);
                return 0;
            }
        }
        fclose($file);
    }

    return 1;
}

if ($_SERVER["REQUEST_METHOD"] === 'POST')
{      

  $input = file_get_contents("php://stdin");
  parse_str($input, $_POST);
    
  if (isset($_POST['signin']))
    {     
            if ($_POST !== null && isset($_POST['email']) && isset($_POST['password']))
            {
                $email = $_POST['email'];
                $password = $_POST['password'];
                $userid = $_COOKIE['userid'];
                if (validate_credential($email, $password, $userid) === 0)
                {
                  header('location: dashboard.php?user_id=' . $_COOKIE['userid']);
                }
                else
                {
                  header('location: register.php');
                }
            }
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
<body class=' bg-secondary text-dark' style='--bs-bg-opacity: .5;'>
<div class="container col-4 border rounded bg-light mt-5" style='--bs-bg-opacity: .5;'>
  <h1 class="text-center">Sign In</h1>
  <hr>
  <form action="" method="post">
    <div class="mb-3">
      <label for="email" class="form-label">Email ID</label>
      <input type="email" class="form-control" name="email" placeholder="Enter your email" autocomplete="off" required>
      <small class="text-muted">Your email is safe with us.</small>
    </div>
    <div class="mb-3">
      <label for="password" class="form-label">Password</label>
      <input type="password" class="form-control" name="password" placeholder="Enter your password" required>
      <small class="text-muted">Do not share your password.</small>
    </div>
    <div class="mb-3">
      <input type="submit" name="signin" class="btn btn-primary">
    </div>
  </form>
</div>