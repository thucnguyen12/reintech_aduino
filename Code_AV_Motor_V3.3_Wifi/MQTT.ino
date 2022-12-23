void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println(msg);
  MQTT_function(msg);
}
// Json data: {"speed":123, "function":"u"}
void MQTT_function(String msg) {
  DynamicJsonDocument doc(512);
  deserializeJson(doc, msg);
  int mqtt_speed = doc["speed"];
  speed_max = mqtt_speed;
  String func = doc["function"];
  mqtt_func = func;
  Serial.print("speed_max: ");
  Serial.println(speed_max);
  Serial.print("mqtt_func: ");
  Serial.println(mqtt_func);
  if (mqtt_func == "up" && digitalRead(SW1) == 1 && dk_mqtt == true) {
    time_run = millis();
    if (stt_direction == 1) {
      step_in = Step_speed_min - step_in;
      stt_direction = 0;
    }
    dk_mqtt = false;
    Motor_run(5);
  } else if (mqtt_func == "down" && digitalRead(SW2) == 1 && dk_mqtt == true) {
    time_run = millis();
    if (stt_direction == 0) {
      step_in = Step_speed_min - step_in;
      stt_direction = 1;
    }
    dk_mqtt = false;
    Motor_run(5);
  } else if (mqtt_func == "stop") {
    digitalWrite(EN_Pin, 1);
    step_in2 = 0;
    dk_mqtt = true;
    // buzz(1);
  }
}

void reconnect() {
  // Loop until we're reconnected
  if (!client.connected() && (unsigned long)(millis() - time_remtqq) > 5000) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "NoTouchAV_" + chipId;
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }
    time_remtqq = millis();
  }
}