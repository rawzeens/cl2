<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "cl2";

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}

$gate = $_GET['gate'];
$location = $_GET['location'];
$name = $_GET['name'];
$rfid = $_GET['rfid'];
$nric = $_GET['nric'];

$sql = "INSERT INTO cl2_rfid_data (gate, location, luggage_cname , rfid , nric) VALUES ('$gate', '$location', '$name', '$rfid', '$nric')";

if ($conn->query($sql) === TRUE) {
  echo "RFID data inserted successfully";
} else {
  echo "Error: " . $sql . "<br>" . $conn->error;
}

$conn->close();
?>
