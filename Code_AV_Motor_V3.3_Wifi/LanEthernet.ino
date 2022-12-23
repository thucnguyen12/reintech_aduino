//code Lan Ethernet
void LanInt() {
  if (Lan_Connect) {
    Lan_Connect = false;
    Serial.println("LAN Ethernet Start...");
    if (Ethernet.begin(mymac, 3000, 2000) == 0) {
      Serial.println("Loi DHCP Lan...");
      dk_lan = false;
      // return false;
      // if (Ethernet.begin(mymac, ip, ipdns) == 0) {
      //   Serial.println("Ket noi Lan khong thanh cong!!!");
      //   // return false;
      // } else {
      //   Serial.println("Ket noi thanh cong.");
      //   Serial.print("IP:  ");
      //   Serial.println(Ethernet.localIP());
      //   client.setClient(ApiClient);
      //   client.setServer(mqtt_server, mqtt_port);
      //   client.setCallback(callback);
      //   buzz(1);
      //   delay(300);
      //   longbuzz();
      //   // return true;
      // }
    } else {
      Serial.println("Ket noi thanh cong.");
      Serial.print("IP:  ");
      Serial.println(Ethernet.localIP());
      EthServer.begin();
      Serial.println("Lan server Start");
      client.setClient(ethClient);
      client.setServer(mqtt_server, mqtt_port);
      client.setCallback(callback);
      dk_lan = true;
      buzz(1);
      delay(300);
      longbuzz();
    }
  }
}

bool Lan_Function() {
  if (Ethernet.linkStatus() == LinkON) {
    // Serial.println("Dang cam cong LAN, tat Wifi");
    LanInt();
    lanAPI_loop();
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    return true;
  } else {
    // Serial.println("Khong cam cong lan, dung Wifi");
    Lan_Connect = true;
    dk_lan = false;
    return false;
  }
}
//Lan Service
void lanAPI_loop() {
  // listen for incoming clients
    EthernetClient ApiClient = EthServer.available();

  if (ApiClient) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    String ethData = "";
    while (ApiClient.connected()) {
      if (ApiClient.available()) {
        char c = ApiClient.read();
        Serial.write(c);
        if (ethData.length() < 60) {
          ethData += c;
        }

        if (c == '\n' && currentLineIsBlank) {
          Serial.println("Data nhan duoc:");
          Serial.println(ethData);
          if (ethData.indexOf("/up") > 0) {
            Serial.println("Chay tien");
            ApiClient.println("HTTP/1.1 200 OK");
            ApiClient.println("Content-Type: text/html");
            ApiClient.println("Connection: close");
            ApiClient.println();
            ApiClient.println("Dang chay tien");
            Lanchar = 'u';
            // LanMotor();
            break;
          } else if (ethData.indexOf("/down") > 0) {
            Serial.println("Chay lui");
            
            ApiClient.println("HTTP/1.1 200 OK");
            ApiClient.println("Content-Type: text/html");
            ApiClient.println("Connection: close");
            ApiClient.println();
            ApiClient.println("Dang chay lui");
            Lanchar = 'd';
            // LanMotor();
            break;
          } else if (ethData.indexOf("/stop") > 0) {
            Serial.println("Chay tien");
            
            ApiClient.println("HTTP/1.1 200 OK");
            ApiClient.println("Content-Type: text/html");
            ApiClient.println("Connection: close");
            ApiClient.println();
            ApiClient.println("Stop");
            Lanchar = 's';
            // LanMotor();
            break;
          }
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    ApiClient.stop();
    LanMotor();
    Serial.println("client disconnected");
  }
}


void LanMotor() {
  // Serial.println("data: ");
  // Serial.println(Lanchar);
  if (Lanchar == 'u' && digitalRead(SW1) == 1 && dk_current == true) {
    time_run = millis();
    if (stt_direction == 1) {
      step_in = Step_speed_min - step_in;
      stt_direction = 0;
    }
    Motor_run(6);
  } else if (Lanchar == 'd' && digitalRead(SW2) == 1 && dk_current == true) {
    time_run = millis();
    if (stt_direction == 0) {
      step_in = Step_speed_min - step_in;
      stt_direction = 1;
    }
    Motor_run(6);
  } else if (Lanchar == 's') {
    Motor_stop();
    step_in2 = 0;
    dk_current = true;
  }
}