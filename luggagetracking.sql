CREATE TABLE cl2_rfid_data (
  id INT AUTO_INCREMENT PRIMARY KEY,
  gate VARCHAR(50) NOT NULL,
  location VARCHAR(50) NOT NULL,
  luggage_cname VARCHAR(50) NOT NULL,
  nric VARCHAR(50) NOT NULL,
  rfid VARCHAR(50) NOT NULL,
 date DATE DEFAULT CURRENT_DATE,  
  time TIME DEFAULT CURRENT_TIME()  
);
