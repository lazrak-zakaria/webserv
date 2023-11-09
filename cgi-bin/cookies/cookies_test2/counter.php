<?php
$timeout = 3600;
session_start();

if ($_SERVER['REQUEST_METHOD'] === "POST")
{
    $input = file_get_contents("php://stdin");
    parse_str($input, $_POST);
    $s_s = session_name();
    if(isset( $_COOKIE[ $s_s ] )) {
        setcookie( $s_s, $_COOKIE[ $s_s ], time() + $timeout, '/' );
    
        echo "Session is created for $s_s.<br/>";
    } else {
        echo "Session is expired.<br/>";
    }
    if(isset($_POST['increment'])) { 
        if(isset($_SESSION['counter'])) {
            $_SESSION['counter']++;
        } else {
            $_SESSION['counter'] = 1;
        }
    }
    if(isset($_POST['result'])) { 
        if(isset($_SESSION['counter'])) {
            echo $_SESSION['counter'];}
    }
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Increment Counter Example</title>
</head>
<body>
    <h1>Counter: <?php echo isset($_SESSION['counter']) ? $_SESSION['counter'] : 0; ?></h1>
    <form method="POST">
        <button type="submit" name="increment">Increment Counter</button>
        <button type="submit" name="result">result</button>
    </form>
</body>
</html>
</html>