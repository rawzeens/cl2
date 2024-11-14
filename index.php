<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no"> <!-- Updated viewport meta tag -->
  <title>RFID Data</title>
  <link href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css" rel="stylesheet">
</head>
<body class="bg-light">
<div class="modal" id="nricModal" tabindex="-1" aria-labelledby="nricModalLabel" aria-hidden="true" data-bs-backdrop="static" data-bs-keyboard="false">
  <div class="modal-dialog">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title" id="nricModalLabel">Enter Your NRIC</h5>
      </div>
      <div class="modal-body">
        <input type="text" class="form-control" id="nricInput" placeholder="Enter NRIC">
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-primary" id="saveNricBtn">Save NRIC</button>
      </div>
    </div>
  </div>
</div>
  <div class="container mt-4">
    
    <h1 class="text-center mb-4">Current RFID Data</h1>
    <div class="border border-secondary rounded p-4 bg-white mb-4">
      <h2 class="text-center mb-3">Current Data for Today</h2>
      <p class="mb-0">

      <?php
        $servername = "";
        $username = "";
        $password = "";
        $dbname = "";
      $conn = new mysqli($servername, $username, $password, $dbname);

      if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
      }

      $today = date("Y-m-d");
      $sql_current = "SELECT gate, location FROM rfid_data WHERE date = '$today'";
      $result_current = $conn->query($sql_current);

      if ($result_current->num_rows > 0) {
        echo "<ul>";
        while ($row = $result_current->fetch_assoc()) {
          echo "<li>Gate: " . $row["gate"] . ", Location: " . $row["location"] . "</li>";
        }
        echo "</ul>";
      } else {
        echo "No data available for today.";
      }
      ?>
      </p>
    </div>

    <hr>

    <div class="border border-secondary rounded p-4 bg-white">
      <h2 class="text-center mb-3">History Data</h2>
      <p class="mb-0">

      <?php

$nric = "your_nric";
      $sql_history = "SELECT gate, location, date FROM rfid_data WHERE nric = '$nric' AND date < '$today'";
      $result_history = $conn->query($sql_history);

      if ($result_history->num_rows > 0) {
        echo "<ul>";
        while ($row = $result_history->fetch_assoc()) {
          echo "<li>Gate: " . $row["gate"] . ", Location: " . $row["location"] . ", Date: " . $row["date"] . "</li>";
        }
        echo "</ul>";
      } else {
        echo "No historical data available.";
      }

      $conn->close(); 
      ?>
      </p>
    </div>
  </div>

  <script src="https://cdn.jsdelivr.net/npm/@popperjs/core@2.11.6/dist/umd/popper.min.js"></script>
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.min.js"></script>
  <!-- jQuery -->
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>

  <script>
    $(document).ready(function() {
      $('#nricModal').modal('show'); 

      $('#saveNricBtn').click(function() {
        var nric = $('#nricInput').val();
        $.ajax({
          type: 'POST',
          url: 'save_nric_to_session.php', 
          data: { nric: nric },
          success: function(response) {
            $('#nricModal').modal('hide'); 
          },
          error: function() {
            alert('Error saving NRIC to session.');
          }
        });
      });
    });
  </script>
</body>
</html>
