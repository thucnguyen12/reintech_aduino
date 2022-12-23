void check_Sensor()
{
  if (digitalRead(SEN_IN) == 1 && (dk_sensor == true))
  {
    delay(300); //Chống nhiễu Sensor
    if (digitalRead(SEN_IN) == 1)
    {
      dk_sensor = false;
      time_run = millis();
      Motor_run(1);
    }
    else if (digitalRead(SW1) == 1 && (digitalRead(SW2) == 1))
    {
      stt_direction = !stt_direction;
      step_in = Step_speed_min - step_in;
      if (stt_direction == 0)
      {
        buzz(3);
      }
      else
      {
        buzz(2);
      }
    }
  }
  if (digitalRead(SEN_IN) == 0 && dk_sensor == false)
  {
    dk_sensor = true;
  }
}
