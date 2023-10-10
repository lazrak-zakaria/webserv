<?php
// Start a session
session_start();

// Check if the user is logged in
if (!isset($_SESSION["user_id"])) {
    header("Location: login.html"); // Redirect to the login page
    exit();
}

// Display the dashboard content for logged-in users
echo "Welcome to the Dashboard!";

// You can add more content or functionality here
?>
