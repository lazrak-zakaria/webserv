<?php  
session_start();

session_unset();  // Vider les variables de sessions 

session_destroy(); // Detruire la session

setcookie('email', 'bla', time() -1, null, null, false, true);
setcookie('password', 'bla', time() - 1, null, null, false, true);
header('location:login.php');


?>