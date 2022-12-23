void readserial() {
  if (Serial.available()) {
    inchar = Serial.read();
    if (inchar == 'u' && digitalRead(SW1) == 1 && dk_serial == true) {
      Serial.println("Chay tien - serial");
      time_run = millis();
      if (stt_direction == 1) {
        step_in = Step_speed_min - step_in;
        stt_direction = 0;
      }
      dk_serial = false;
      Motor_run(4);
    } else if (inchar == 'd' && digitalRead(SW2) == 1 && dk_serial == true) {
      Serial.println("Chay lui - serial");
      time_run = millis();
      if (stt_direction == 0) {
        step_in = Step_speed_min - step_in;
        stt_direction = 1;
      }
      //      stt_direction = 1;
      dk_serial = false;
      Motor_run(4);
    } else if (inchar == 's') {
      digitalWrite(EN_Pin, 1);
      step_in2 = 0;
      dk_serial = true;
      // buzz(1);
    }
  }
}
