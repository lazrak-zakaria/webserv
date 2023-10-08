<?php include "header.php" ?>
<?php
session_start();
function my_create_session_id()
{
  session_create_id();
}

function check_file($find)
{
  $handle = fopen("inputfile.txt", "r");
      if ($handle) {
          while (($line = fgets($handle)) !== false) {
              $pars = explode(';', $line);
              if ($pars[1] === $find)
              {
                fclose($handle);
                return 0;
              }
          }
          fclose($handle);
          return 1;
      }
}

function add_user($userid, $email, $pass)
{
  $handle = fopen("inputfile.txt", "a+");
  if ($handle) {
    $tmp = "userid=";
      $tmp .= $userid; 
      $tmp .= ';email=';
      $tmp .= $email;
      $tmp .= ';password=';
      $tmp .= $pass;
      $tmp .= "\n";
      fwrite($handle, $tmp);
      fclose($handle);
      return 1;
  }
}

if ($_SERVER["REQUEST_METHOD"] === 'POST')
{     
  
      $input = file_get_contents("php://stdin");
      parse_str($input, $_POST);

      if (isset($_POST['signup']))
      {
        // $data = json_decode($input, true); // Assuming data is in JSON format

        if ($_POST !== null && isset($_POST['name']) && isset($_POST['email']) && isset($_POST['password']))
        {
            if (check_file($_POST['email'] === 0))
            {
                // duplicated email or password
                // exit();
            }
        }
        $userid = uniqid();
        add_user($userid, $_POST['email'], $_POST['password']);
        setcookie('userid', $userid, time() + 3600);
        setcookie('email', $_POST['email'], time() + 3600);
        setcookie('password', $_POST['password'], time() + 3600);
        setcookie('name', $_POST['name'], time() + 3600);
        $_SESSION['userid'] = $userid;
        $_SESSION['email'] = $_POST['email'];
        $_SESSION['password'] = $_POST['password'];
        $_SESSION['name'] = $_POST['name'];
        header('location: login.php');

        // $_SESSION['name'] = $_POST['name'];
        // $_SESSION['email'] = $_POST['email'];
        // $_SESSION['password'] = $_POST['password'];
      }
      else
          echo "ERROR WHO ARE YOUggg !";
}
else
  echo "ERROR WHO ARE YOU !";
?>


<div class="container col-4 border rounded bg-light mt-5" style='--bs-bg-opacity: .5;'>
  <h1 class="text-center">Sign Up</h1>
  <hr>
  <form action="" method="post">
    <div class="mb-3">
      <label for="name" class="form-label">Username</label>
      <input type="text" class="form-control" name="name" placeholder="Enter your name" autocomplete="off" required>
    </div>
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
      <input type="submit" name="signup" class="btn btn-primary">
    </div>
  </form>
</div>
