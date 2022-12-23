void loadCredentials() {
  preferences.getString("ssid", ssid, sizeof(ssid));
  preferences.getString("password", password, sizeof(password));
  Serial.println("Recovered credentials:");
  Serial.println(ssid);
  Serial.println(password);
}

/** Store WLAN credentials to Preference */
void saveCredentials() {
  if (server.arg("ssid") != "") {
    server.arg("ssid").toCharArray(ssid, sizeof(ssid) - 1);
    server.arg("pass").toCharArray(password, sizeof(password) - 1);
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    Serial.println("Saved credentials:");
    Serial.println(ssid);
    Serial.println(password);
    connect = true;
  }
  String webSite = top_head;
  webSite += "<h2 style=\"text-align:center;\">No-Touch AV - Connect Wifi</h2>";

  webSite += "<div>Đang kết nối wifi...</div>";
  webSite += "<div><form action=\"/\"><button>Back home</button></form></div>";
  // webSite += st;
  webSite += bot_head;
  server.send(200, "text/html", webSite);
  delay(1000);
  ESP.restart();
}