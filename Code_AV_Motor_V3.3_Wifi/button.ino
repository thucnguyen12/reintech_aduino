void check_button() {
  if (digitalRead(Button1) == 0 && digitalRead(SW2) == 1 && dk_button == true) {
    time_run = millis();
    if (stt_direction == 0) {
      step_in = Step_speed_min - step_in;
      stt_direction = 1;
    }
    dk_button = false;
    Motor_run(2);
  }
  if (digitalRead(Button1) == 1 && dk_button == false) {
    step_in2 = 0;
    dk_current = true;
  }
  if (digitalRead(Button2) == 0 && digitalRead(SW1) == 1 && dk_button == true) {
    time_run = millis();
    if (stt_direction == 1) {
      step_in = Step_speed_min - step_in;
      stt_direction = 0;
    }
    dk_button = false;
    Motor_run(2);
  }
  if (digitalRead(Button2) == 1 && dk_button == false) {
    step_in2 = 0;
    dk_button = true;
  }
}
