<?php  
session_start();

session_unset();  // Vider les variables de sessions 

session_destroy(); // Detruire la session

header('location:login.php');



?>