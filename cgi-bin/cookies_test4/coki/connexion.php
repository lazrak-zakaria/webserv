<?php  

session_start();

// $_SESSION['nom'] = $_POST['nom'];
// $_SESSION['prenom'] = $_POST['prenom'];
if ($_SERVER["REQUEST_METHOD"] === 'POST')
{
$input = file_get_contents("php://stdin");
parse_str($input, $_POST);
if (isset($_POST['email']) && isset($_POST['password']) && isset($_SESSION['password']) && isset($_SESSION['password']))
{
	if ($_POST['email'] != $_SESSION['email'] || $_POST['password'] != $_SESSION['password'])
	{
		header('location:register.php');
		exit ;
	}
	else
	{
		$_SESSION['email'] = $_POST['email'];
		$_SESSION['password'] = $_POST['password'];
		
		if(isset($_POST['check'])){
			setcookie('email', $_SESSION['email'], time() + 365*24*3600, null, null, false, true);
			setcookie('password', $_POST['password'], time() + 365*24*3600, null, null, false, true);
		}
		header('location:profil.php');
	}
}
else
{
	header('location:register.php');
}
}


?>