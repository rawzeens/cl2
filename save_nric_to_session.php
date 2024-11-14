<?php
session_start();

if(isset($_POST['nric'])) {
    $_SESSION['nric'] = $_POST['nric'];
    
    echo "NRIC saved to session successfully!";
} else {
    echo "Failed to save NRIC to session. NRIC value not received.";
}
?>
