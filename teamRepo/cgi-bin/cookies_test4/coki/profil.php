<?php 

session_start();

if(!isset($_SESSION['email'])){
	header('location:login.php');
}else{

?>

<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>Profil</title>
	<link rel="stylesheet" href="CSS/profil.css">
</head>

<body>

	<h1>Profil</h1>

	<h2>
		Hello 
		<span style="color: red;"> 
			<?php 
				//echo $_SESSION['nom'] . ' ' . $_SESSION['prenom']; 
			?> 
		</span>
	</h2>


	<h2>
		Your email: 
		<span style="color: red;"> 
			<?php
				if (isset($_COOKIE['email']))
				{
					echo 'this email from cookies: ';
					echo $_COOKIE['email'];
				}
				else
				{
					echo 'this email from sessions: ';
					echo $_SESSION['email'];
				}
			?> 
		</span>
	</h2>


	<ul>
		<li><a href="page1.php">upload file</a></li>
		<li><a href="page2.php">Name Submission Form</a></li>
		<li><a href="page3.php">Video</a></li>
	</ul>

	<a href="deconnexion.php" class="deconnexion">Sign out</a>
	<a href="deleteAccount.php" class="deconnexion">delete account</a>
	
</body>
</html>
<?php  } ?>

