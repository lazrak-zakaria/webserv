<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>LearnToCode</title>
	<link rel="stylesheet" href="CSS/login.css">
</head>
<body>

	

        <form method="POST" action="connexion.php">

        	<h1>Connection page:</h1>
        	       	
        

        		<input type="text" name="email" placeholder="Email" value="<?php if(isset($_COOKIE['email'])) echo $_COOKIE['email'];?>"><br><br>       	
        		
        	    <input type="password" name="password" placeholder="Password" value="<?php if(isset($_COOKIE['password'])) echo $_COOKIE['password'];?>"><br><br>        	
        	
        		<input type="checkbox" name="check" id="check">
        		<label for="check">se souvenir de moi</label><br><br>
        	         		
        		
                <td align="center"><br><input type="submit" value="Sign in">
                 	

        </form>

</body>
</html>