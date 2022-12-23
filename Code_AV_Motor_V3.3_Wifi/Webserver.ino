String top_head = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><meta charset=\"utf-8\"><title>Cài đặt thiết bị</title><style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style><script> function chon(l,e) { console.log(l); document.getElementById('sid').value = l.innerText || l.textContent; document.getElementById('pas').focus();}</script><script>setInterval(loadDoc,2000);function loadDoc() {var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() {if (this.readyState == 4 && this.status == 200) {document.getElementById(\"webpage\").innerHTML =this.responseText}};xhttp.open(\"GET\", \"/\", true);xhttp.send();}</script></head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
String bot_head = "<div style=\"text-align:right;font-size:12px;\"><hr><a href=\"NoTouchAv\" target=\"_blank\" style=\"color:#66B032;\">Thiết bị điều khiển Touch AV " + Fw + "</a></div> <div class=\"corom-element\" style=\"all: initial;\"> </div></div></body></html>";  // thêm 1 </div> cho webpage


/** Is this an IP? */
boolean isIp(String str) {
  for (int i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

/** IP to String? */
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

void home_page() {
  if (captivePortal()) {  // If caprive portal redirect instead of displaying the page.
    return;
  }
  // scan_wifi();
  String webSite = top_head;
  webSite += "<h2 style=\"text-align:center;\">No-Touch AV - Setup</h2>";
  webSite += "<h3>LAN Status: " + stt_lan + "</h3>";
  webSite += "<h3>Wifi Status: " + stt_wifi + "</h3>";
  webSite += "<form method=\"post\" action=\"/save\">";
  webSite += "<div>Nhập tên wifi cần kết nối</div>";
  webSite += "<div><input id=\"sid\" name=\"ssid\" size=\"30\" placeholder=\"Nhập tên wifi\"></div>";
  webSite += "<div>Nhập mật khẩu wifi cần kết nối</div>";
  webSite += "<div><input id='pas' type = \"password\" name=\"pass\" size=\"50\" placeholder=\"Nhập password wifi\"></div>";
  webSite += "<div><button type='submit'>Save</button></div>";
  webSite += "</form>";
  webSite += "<div>Danh sách Wifi có thể kết nối:</div>";
  webSite += st;
  webSite += "<div><button type=\"radio\" onclick=\"location.href='/updateota'\" />Update Firmware</button></div>";
  webSite += "<div><form action=\"/reset\"><button>Restart</button></form></div>";
  webSite += "<div><form action=\"/factory1\"><button>Factory Reset</button></form></div>";
  webSite += bot_head;
  server.send(200, "text/html", webSite);
}

void update_OTA() {

  String webSite = top_head;
  webSite += "<h2 style=\"text-align:center;\">Update Frimwate OTA</h2>";
  webSite += " <p>Chọn File .bin để nâng cấp chương trình</p>";
  webSite += "<div><form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><div style=\"text-align: center;\"><button type='submit'> UPDATE</button></div></form></div>";

  server.send(200, "text/html", webSite);
}

String scan_wifi() {
  int n = WiFi.scanNetworks();
  Serial.print("Tim hoan tat, So wifi co the ket noi n=");
  Serial.println(n);
  st = "<table><tr><th>Danh sách wifi có thể kết nối</th><th>Chất lượng</th></tr><tbody>";
  if (n == 0) {
    //Serial.println("no networks found");
  } else {
    //Serial.print(n);
    //Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      st += "<tr><td><a href=\"#\" onclick=\"chon(this)\">";
      st += WiFi.SSID(i);
      st += "</a></td><td>";
      st += WiFi.RSSI(i);
      st += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*";
      st += "</td></tr>";
    }
    st += "</tbody></table>";
  }
}

void reset() {
  String content = top_head;
  content += "<h2 style=\"text-align:center;\">No-Touch AV - Khởi động lại</h2>";
  content += "<div>Thiết bị đang được khởi động lại...</div>";
  content += bot_head;
  server.send(200, "text/html", content);
  delay(1000);
  ESP.restart();
}

void factory1() {
  String content = top_head;
  content += "<h2 style=\"text-align:center;\">No-Touch AV - Khôi phục mặc định</h2>";
  content += "<div>Bạn chắc chắn muốn khôi phục mặc định thiết bị. Việc khôi phục mặc định sẽ xóa kết nối wifi thiết bị</div>";
  content += "<div><form action=\"/factory2\"><button>OK</button></form></div>";
  content += "<div><form action=\"/\"><button>Back home</button></form></div>";
  content += bot_head;
  server.send(200, "text/html", content);
}

void factory2() {
  preferences.putString("ssid", "");
  preferences.putString("password", "");
  Serial.println("Khoi phuc mac dinh thanh cong");
  String content = top_head;
  content += "<h2 style=\"text-align:center;\">No-Touch AV - Khôi phục mặc định</h2>";
  content += "<div>Thiết bị đang được khôi phục mặc định. Hãy kết nối lại với wifi của thiết bị để bắt đầu một thiết lập mới</div>";
  content += bot_head;
  server.send(200, "text/html", content);
  WiFi.disconnect();
  delay(1000);
  ESP.restart();
}

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortal() {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(myHostname) + ".local")) {
    Serial.println("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send(302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop();              // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

void handleNotFound() {
  if (captivePortal()) {  // If caprive portal redirect instead of displaying the error page.
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(404, "text/plain", message);
}

void update_done() {
  String content = top_head;
  content += "<h2 style=\"text-align:center;\">No-Touch AV - Update done!</h2>";
  content += "<div>Update firmware thành công!</div>";
  content += "<div><form action=\"/\"><button>Back home</button></form></div>";
  content += bot_head;
  server.send(200, "text/html", content);
}
void update_fail() {
  String content = top_head;
  content += "<h2 style=\"text-align:center;\">No-Touch AV - Update Error!</h2>";
  content += "<div>Update firmware Error!</div>";
  content += "<div><form action=\"/\"><button>Back home</button></form></div>";
  content += bot_head;
  server.send(200, "text/html", content);
}

//điều khiển motor
void upMotor() {
  String content = "Tien len";
  server.send(200, "text/html", content);
  WifiChar = 'u';
  wifiMotor();
}
void downMotor() {
  String content = "Lui xuong";
  server.send(200, "text/html", content);
  WifiChar = 'd';
  wifiMotor();
}
void stopMotor() {
  String content = "Stop";
  server.send(200, "text/html", content);
  WifiChar = 's';
  wifiMotor();
}