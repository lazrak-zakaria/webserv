<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>LearnToCode</title>
	<link rel="stylesheet" href="CSS/login.css">
</head>
<body>

	

        <form method="POST" action="connexion.php">

        	<h1>Bienvenu dans la page de connexion:</h1>
        	       	
        		
        		<!-- <input type="text" name="nom" placeholder="Votre Nom"><br><br>

        		<input type="text" name="prenom" placeholder="Votre Prenom"><br><br> -->

        		<input type="text" name="email" placeholder="Votre Email" value="<?php if(isset($_COOKIE['email'])) echo $_COOKIE['email'];?>"><br><br>       	
        		
        	    <input type="password" name="password" placeholder="Votre Mot de passe" value="<?php if(isset($_COOKIE['password'])) echo $_COOKIE['password'];?>"><br><br>        	
        	
        		<input type="checkbox" name="check" id="check">
        		<label for="check">se souvenir de moi</label><br><br>
        	         		
        		
                <td align="center"><br><input type="submit" value="Se Connecter">
                 	

        </form>

</body>
</html>