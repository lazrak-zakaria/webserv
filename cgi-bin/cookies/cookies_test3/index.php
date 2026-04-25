<?php include "header.php" ?>

<style>
  .auth-illustration {
    width: 100%;
    max-width: 300px;
    height: 220px;
    object-fit: contain;
    margin: 0 auto 1rem;
    display: block;
  }
</style>

<h2 class="text-center mt-3">LOGIN System with PHP Session</h2>
<hr>

<div class="container">
  <div class="row">

    <div class="col">
      <div class="card text-center">
        <div class="card-header">
          <h3 class="card-title">Login</h3>
        </div>
        <div class="card-body">
          <a href="login.php"><img src="images/login-illustration.svg" alt="login illustration" class="auth-illustration"></a>
          <p class="card-text text-muted">Never share your password with anyone.</p>
          <a href="login.php" class="btn btn-primary"> Sign In</a>
        </div>
      </div>
    </div>

    <div class="col">
      <div class="card text-center">
        <div class="card-header">
          <h3 class="card-title">Register</h3>
        </div>
        <div class="card-body">
          <a href="register.php"><img src="images/register-illustration.svg" alt="register illustration" class="auth-illustration"></a>
          <p class="card-text text-muted">Always use different password for different site.</p>
          <a href="register.php" class="btn btn-primary"> Sign Up </a>
        </div>
      </div>
    </div>

  </div>
</div>