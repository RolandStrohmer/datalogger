#include "DHT.h"

#define DHTPIN 2  // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
#define DHTTYPE DHT11  // DHT 11
DHT dht(DHTPIN, DHTTYPE);
/*
  MySQL Connector/Arduino Example : connect by wifi

  This example demonstrates how to connect to a MySQL server from an
  Arduino using an Arduino-compatible Wifi shield. Note that "compatible"
  means it must conform to the Ethernet class library or be a derivative
  with the same classes and methods.

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
#include <WiFi.h>  // Use this for WiFi instead of Ethernet.h
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress server_addr(192, 168, 0, 11);  // IP of the MySQL *server* here
char user[] = "loggeruser";              // MySQL user login username
char password[] = "secure";              // MySQL user login password

// WiFi card example
char ssid[] = "WLAN_Strohmer";  // your SSID
char pass[] = "TGM123456";      // your SSID Password

char sensorName[] = "TestSensor";
char INSERT_DATA[] = "INSERT INTO `DataLogger`.`Datensammlung`(`Sensor`, `Wert`, `Einheit`) VALUES ('%s','%f','%s');";
char query[128];
WiFiClient client;  // Use this for WiFi instead of EthernetClient
MySQL_Connection conn((Client *)&client);

void setup() {
  Serial.begin(115200);
  dht.begin();
  while (!Serial)
    ;  // wait for serial port to connect. Needed for Leonardo only

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

  Serial.println("Connecting...");
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
  } else
    Serial.println("Connection failed.");
  conn.close();
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C  Heat index: "));
  Serial.print(hic);
  Serial.println(F("°C "));

  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    // Save
    sprintf(query, INSERT_DATA, sensorName, t, "°C");
    Serial.println(query);
    // Execute the query
    cur_mem->execute(query);
    // Note: since there are no results, we do not need to read any data
    // Deleting the cursor also frees up memory used
    delete cur_mem;
    Serial.println("Data recorded.");
  } else {
    Serial.println("Connection failed.");
    conn.close();
  }
  delay(10000);
}
