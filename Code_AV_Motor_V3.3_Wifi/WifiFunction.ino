void WifiAp_Int() {
  WiFi.softAPConfig(apIP, apIP, netMsk);
  // strcpy(softAP_ssid, chipId.c_str());
  softAP_ssid += chipId;
  WiFi.softAP(softAP_ssid.c_str(), softAP_password.c_str());
  delay(100);  // Without delay I've seen the IP address blank
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNSPORT, "*", apIP);

  server.on("/", home_page);
  server.on("/save", saveCredentials);
  server.on("/generate_204", home_page);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/fwlink", home_page);        //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/updateota", update_OTA);
  server.on("/reset", reset);
  server.on("/factory1", factory1);
  server.on("/factory2", factory2);
  server.on("/up", upMotor);
  server.on("/down", downMotor);
  server.on("/stop", stopMotor);
  server.on(
    "/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
    },
    []() {
      HTTPUpload &upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin()) {  //start with max available size
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {  //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
          update_done();
        } else {
          Update.printError(Serial);
          update_fail();
        }
        Serial.setDebugOutput(false);
      } else {
        Serial.printf("Update Failed Unexpectedly (likely broken connection): status=%d\n", upload.status);
        update_fail();
      }
    });
  server.onNotFound(handleNotFound);
  server.begin();  // Web server start
  Serial.println("HTTP server started");
  loadCredentials();           // Load WLAN credentials from network
  connect = strlen(ssid) > 0;  // Request WLAN connect if there is a SSID
  Serial.println(softAP_ssid + " Wifi AP Started");
}

void Wifi_function() {
  // if (Use_Wifi) {
  if (connect) {
    Serial.println("Connect requested");
    connect = false;
    stt_wifi = "Wifi Connect...";
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    // connectWifi();
    lastConnectTry = millis();
  } else {
    int s = WiFi.status();
    // Serial.print(s);
    if (s == 0 && millis() > (lastConnectTry + 30000)) {
      /* If WLAN disconnected and idle try to connect */
      /* Don't set retry time too low as retry interfere the softAP operation */
      connect = true;
    }
    if (status != s) {  // WLAN status change
      Serial.print("Status: ");
      Serial.println(s);
      status = s;
      if (s == WL_CONNECTED) {
        /* Just connected to WLAN */
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        // WiFi.mode(WIFI_STA);
        stt_wifi = "Wifi Connected " + String(ssid);
        // Setup MDNS responder
        if (!MDNS.begin(myHostname.c_str())) {
          Serial.println("Error setting up MDNS responder!");
        } else {
          Serial.println("mDNS responder started");
          // Add service to MDNS-SD
          MDNS.addService("http", "tcp", 80);
        }
        client.setClient(espClient);
        client.setServer(mqtt_server, mqtt_port);
        client.setCallback(callback);
        longbuzz();
      } else if (s == WL_NO_SSID_AVAIL) {
        Serial.println("WL_NO_SSID_AVAIL");
        WiFi.disconnect();
        // WiFi.softAPConfig(apIP, apIP, netMsk);
        // WiFi.softAP(softAP_ssid.c_str(), softAP_password.c_str());
        // WiFi.mode(WIFI_AP_STA);
        // WiFi.softAPConfig(apIP, apIP, netMsk);
        // WiFi.softAP(softAP_ssid.c_str(), softAP_password.c_str());
        // dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
        // dnsServer.start(DNSPORT, "*", apIP);
        // WiFi.disconnect();
      }
    }
    if (s == WL_CONNECTED) {
      //wifi connected function
      if (!client.connected()) {
        reconnect();
      }
      client.loop();
    }
  }
  // }
  // Do work:
  //DNS
  dnsServer.processNextRequest();
  //HTTP
  server.handleClient();
  // Serial.println("Run...");
}

void wifiMotor() {
  // Serial.println("data: ");
  // Serial.println(WifiChar);
  if (WifiChar == 'u' && digitalRead(SW1) == 1 && dk_current == true) {
    time_run = millis();
    if (stt_direction == 1) {
      step_in = Step_speed_min - step_in;
      stt_direction = 0;
    }
    Motor_run(7);
  } else if (WifiChar == 'd' && digitalRead(SW2) == 1 && dk_current == true) {
    time_run = millis();
    if (stt_direction == 0) {
      step_in = Step_speed_min - step_in;
      stt_direction = 1;
    }
    Motor_run(7);
  } else if (WifiChar == 's') {
    Motor_stop();
    step_in2 = 0;
    dk_current = true;
  }
}