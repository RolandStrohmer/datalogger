/*
  MySQL Connector/Arduino Example : connect by wifi using WiFi 101 shield

  This example demonstrates how to connect to a MySQL server from an
  Arduino using using the new WiFi Shield 101 from arduino.cc.
  
  For more information and documentation, visit the wiki:
  https://github.com/ChuckBell/MySQL_Connector_Arduino/wiki.

  INSTRUCTIONS FOR USE

  1) Change the address of the server to the IP address of the MySQL server
  2) Change the user and password to a valid MySQL user and password
  3) Change the SSID and pass to match your WiFi network
  4) Connect a USB cable to your Arduino
  5) Select the correct board and port
  6) Compile and upload the sketch to your Arduino
  7) Once uploaded, open Serial Monitor (use 115200 speed) and observe

  If you do not see messages indicating you have a connection, refer to the
  manual for troubleshooting tips. The most common issues are the server is
  not accessible from the network or the user name and password is incorrect.

  Note: The MAC address can be anything so long as it is unique on your network.

  Created by: Dr. Charles A. Bell
*/
#include <SPI.h>
#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress server_addr(95, 128, 203, 13);  // IP of the MySQL *server* here
char user[] = "loggeruser";                   // MySQL user login username
char password[] = "secret";             // MySQL user login password

char INSERT_DATA[] = "INSERT INTO `DataLogger`.`Datensammlung`(`sensor`, `wert`, `einheit`) VALUES ('%s','%s','%s');";
char query[128];
char temperature[10];


// WiFi card example
char ssid[] = "strohmerwlan";  // your SSID
char pass[] = "TGM123456";  // your SSID Password

WiFiClient client;
MySQL_Connection conn((Client *)&client);

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;  // wait for serial port to connect

  // Begin WiFi section
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();
    WiFi.begin(ssid, pass);
    for (int i = 0; i <= 30; i++) {
      delay(500);
      Serial.print(".");
      if (WiFi.status() == WL_CONNECTED) break;
    }
  }
  // print out info about the connection:
  {
    Serial.println("Connected to network");
    IPAddress ip = WiFi.localIP();
    Serial.print("My IP address is: ");
    Serial.println(ip);
  }
  // End WiFi section

  
}

void loop() {
  Serial.println("Connecting...");
  while (1) {
    if (conn.connect(server_addr, 3306, user, password)) {
      delay(1000);
      MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
      // Save
      sprintf(query, INSERT_DATA, "test sensor", "24.32", "°C");
      Serial.println(query);
      // Execute the query
      cur_mem->execute(query);
      // Note: since there are no results, we do not need to read any data
      // Deleting the cursor also frees up memory used
      delete cur_mem;
      Serial.println("Data recorded.");
    } else
      Serial.println("Connection failed.");
    conn.close();
    delay(30000);
  }
}
