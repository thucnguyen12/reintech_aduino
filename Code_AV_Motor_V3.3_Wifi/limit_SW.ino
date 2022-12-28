void checkLM_SW(bool stt) {
  if (stt) {
    if (digitalRead(SW2) == 0) {
      if (digitalRead(SW2) == 0) {
        Serial.println("Cham SW2!");
        for (int i = 0; i < 800; i++) {
          digitalWrite(STEP, HIGH);
          delayMicroseconds(speed_min);
          digitalWrite(STEP, LOW);
          delayMicroseconds(speed_min);
        }
        Serial.print("step_in: ");
        Serial.println(step_in);
        //        Serial.print("step_out: ");
        //        Serial.println(step_out);
        Motor_stop();
        stt_direction = 0;
        step_in = 0;
        step_in2 = 0;
        stt_LM = false;
        dk_mqtt = true;
      }
    }
  } else {
    if (digitalRead(SW1) == 0) {
      if (digitalRead(SW1) == 0) {
        Serial.println("Cham SW1!");
        check_current_for_direction = CHECK_OUT_CRASH;
        Serial.print("step_in: ");
        Serial.println(step_in);
        //        Serial.print("step_out: ");
        //        Serial.println(step_out);
        Motor_stop();
        stt_direction = 1;
        step_in = 0;
        step_in2 = 0;
        stt_LM = false;
        dk_mqtt = true;
      }
    }
  }
}