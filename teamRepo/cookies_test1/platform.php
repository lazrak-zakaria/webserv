<?php include "header.php"?>
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
      exit();
    }
    if (isset($_POST['delete']))
    {
      session_destroy();
      setcookie('name', $_POST['name'], time() - 3600);
      setcookie('userid', $_POST['id'], time() - 3600);
      header('location: login.php');
      exit();
    }
    
}
?>


<?php
  session_start();


?>

<!-- <h4> <?php echo $_SESSION['name']; ?> </h4>
<tbody>
      <tr>
        <td> <?php echo $_SESSION['id']; ?></td>
        <td> <?php echo $_SESSION['name']; ?></td>
        <td> <?php echo $_SESSION['email']; ?></td>
      </tr>
    </tbody> -->


<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>platform</title>
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
<body style="background-color: <?php echo $_SESSION['color']; ?>">
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