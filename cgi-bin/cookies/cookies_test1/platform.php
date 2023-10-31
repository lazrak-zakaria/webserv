<?php 
  session_start();
    if(!isset($_COOKIE['name']) || (!isset($_COOKIE['userid']) && !isset($_SESSION['color']) && !isset($_SESSION['email'])))
    {
        header("location: login.php");
        exit();
    }
?>
<?php
if ($_SERVER['REQUEST_METHOD'] === "POST")
{
    $input = file_get_contents("php://stdin");
    parse_str($input, $_POST);
    if (isset($_POST['singout'])) {
      setcookie('name', $_COOKIE['name'], time() - 3600);   
      header('location: login.php');
      exit;
    }
    if (isset($_POST['delete']))
    {
      setcookie(session_name(), "", time() - 3600);
      session_unset();
      session_destroy();
      setcookie('name', $_COOKIE['name'], time() - 3600);
      setcookie('userid', $_COOKIE['userid'], time() - 3600);
      header('location: login.php');
      exit;
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
    <style>
      body
      {
        background-color: <?php echo $_SESSION['color']; ?>;
        color: white;
      }
      body, td, th
      {
        color: white;
      }
      td:hover
      {
        color: <?php echo $_SESSION['color']; ?>;
      }
    </style >
</head>
<body style='--bs-bg-opacity: .5;'>
    <div class="container col-12 border rounded mt-3">
  <h1 class=" mt-3 text-center">Welcome, <?php echo $_COOKIE['name']; ?></h1>
  <hr>
  <h4> <?php echo $_COOKIE['name']; ?> </h4>

  <table class="table table-striped table-bordered table-hover">
    <thead class="table-dark">
      <tr>
        <th scope="col">ID</th>
        <th scope="col">Username</th>
        <th scope="col">Email</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td> <?php echo $_SESSION['userid']; ?></td>
        <td> <?php echo $_COOKIE['name']; ?></td>
        <td> <?php echo $_SESSION['email']; ?></td>
      </tr>
    </tbody>
  </table>
      <form method="POST">
      <input type="submit" name="singout" value="Logout">
      <input type="submit" name="delete" value="Delete Account">
      </form>
</body>
</html>