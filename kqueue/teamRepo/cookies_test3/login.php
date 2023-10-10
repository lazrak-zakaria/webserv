<!-- <?php session_start(); ?> -->
<!-- <?php include "header.php" ?> -->
<?php

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
                // $username = $data['username'];
                $email = $_POST['email'];
                $password = $_POST['password'];
                // if($_COOKIE['email'] === $email && $_COOKIE['password'] === $password)
                // {
                //     header('location: dashboard.php?user_id=' . $_SESSION['name']);
                // }
                // else
                // {
                //   echo "ddddddddddddddddddddd\n";
                //   // header('location: register.php');
                // }
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
  else
    echo "ERROR WHO ARE YOU !";
?>

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