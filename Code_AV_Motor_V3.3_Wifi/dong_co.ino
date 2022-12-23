void toc_do() {
  if (stt_LM) {
    sped = speed_min;
  } else {
    //    long t_run = millis() - time_run;
    if (step_in2 < Step_up_down) {
      sped = speed_start - (step_in2 / 10) * Accelera;
    } else if (step_in < Step_speed_down) {
      sped = speed_max;
      if (stt_direction) {
        check_current_for_direction = CHECK_OUT_CRASH;
        check_current(Adc_delta1);
      }
    } else if (step_in < Step_speed_min) {
      sped = speed_max + ((step_in - Step_speed_down) / 10) * Accelera;
      if (stt_direction) {
        check_current_for_direction = CHECK_IN_CRASH;
        check_current2(Adc_delta2);
      }
    } else if (step_in > Step_speed_min) {
      sped = speed_min;
      check_current_for_direction = CHECK_IN_CRASH;
      check_current2(Adc_delta2);
    }
  }
}

void Motor_run(byte stt) {    // 1 là ra, 0 là chạy vào
  digitalWrite(EN_Pin, LOW);  // kích hoạt
  digitalWrite(DIR, stt_direction);
  dk_run = true;
  time_step = micros();
  //  Serial.println("Bat dau chay");
  while (dk_run) {
    toc_do();
    if (micros() - time_step >= sped) {
      step_in++;
      step_in2++;
      stt_step = !stt_step;
      digitalWrite(STEP, stt_step);
      time_step = micros();
    }
    checkLM_SW(stt_direction);
    switch (stt) {
      case 1:  // sensor
        if (digitalRead(SEN_IN) == 0) {
          dk_run = false;
          step_in2 = 0;
        }
        break;
      case 2:  //nút ấn
        if (digitalRead(Button1) == 1 && digitalRead(Button2) == 1) {
          dk_run = false;
          step_in2 = 0;
        }
        break;
      case 3:  // bluetooth
        if (Blechar == 's') {
          digitalWrite(EN_Pin, 1);
          dk_run = false;
          step_in2 = 0;
        }
        break;
      case 4:  // serial
        if (Serial.available()) {
          inchar = Serial.read();
          if (inchar == 's') {
            digitalWrite(EN_Pin, 1);
            dk_run = false;
            dk_serial = true;
            step_in2 = 0;
          }
        }
        break;
      case 5:  // MQTT
        client.loop();
        if (mqtt_func == "stop") {
          digitalWrite(EN_Pin, 1);
          dk_run = false;
          dk_mqtt = true;
          step_in2 = 0;
        }
        break;
      case 6:  //Lan local control
        lanAPI_loop();    
        if (Lanchar == 's') {
          digitalWrite(EN_Pin, 1);
          dk_run = false;
          step_in2 = 0;
        }
        break;
      case 7:  //Wifi local control
        server.handleClient(); 
        if (WifiChar == 's') {
          digitalWrite(EN_Pin, 1);
          dk_run = false;
          step_in2 = 0;
        }
        break;
      default:
        break;
    }
  }
  Serial.println("Dung dong co");
  Motor_stop();
}

void Motor_stop() {
  digitalWrite(EN_Pin, 1);
  digitalWrite(STEP, 0);

  dk_run = false;
  if (stt_direction == 1) {
    Serial.println(Step_speed_min - step_in);
    ;
  } else {
    Serial.println(step_in);
  }
}

void Motor_stop_in_isr() {
  digitalWrite(EN_Pin, 1);
  digitalWrite(STEP, 0);
}

void check_current(float deltaAdc) {
  if (millis() - time_cur > 2) {

    // sumVal = 0;
    // Val_array[k] = analogRead(CURRENT_SENSOR);//doc du lieu camr bien
    // k++;

    // if (k > 99)
    // {
    //   k = 0;
    // }
    // if (first_time_adc)
    // {
    //     for (int i = 0; i < 100; i++)
    //     {
    //       Val_array[i] =  Val_array[k];
    //     }
    //     first_time_adc = false;
    // }
    

    // for (int i = 0; i < 100; i++)
    // {
    //   sumVal = sumVal + Val_array[k];
    // }
    
    // Serial.print("val " + String(k) + ": ");
    // Serial.println(Val_array[k]);
    
    
    // if (k > 9) {
      // Val_current = sumVal / 100;
      // k = 0;
      // sumVal = 0;
      // Serial.println("Val_current: " + String(Val_current));
      // float delta = (Val_current-Old_val)/Old_val;
      // Serial.println("delta: " + String(delta));
      // Serial.println("Old_val1: " + String(Old_val1));
      // Serial.println("Old_val2: " + String(Old_val2));
      Serial.println("OUT CRASH Val_current: " + String(Val_current));
      
      if (!there_is_out_crash)//((Old_val1 > 800) && (Old_val2 >= Old_val1 * deltaAdc) && (Val_current >= Old_val2 * deltaAdc)) {
      {
        time_cur = millis();
        return;  
      }
        dk_current = false;
        //        Motor_stop();
        // start motor again
        // digitalWrite(EN_Pin, LOW);  // kích hoạt
        // digitalWrite(DIR, stt_direction);

        Lanchar = 's';
        WifiChar = 's';
        dk_run = false;
        Serial.println("impact delta");
        Serial.println("Old_val1: " + String(Old_val1_make_crash));
        Serial.println("Old_val2: " + String(Old_val2_make_crash));
        Serial.println("Val_current: " + String(Val_current_make_crash));
        delay(100);
        digitalWrite(DIR, 0);
        longbuzz();
        stt_direction = 0;
        step_in2 = 0;
        step_in = Step_speed_min - step_in;
        for (int i = 0; i < 2600; i++) {
          digitalWrite(STEP, HIGH);
          delayMicroseconds(speed_min);
          step_in++;
          digitalWrite(STEP, LOW);
          delayMicroseconds(speed_min);
          step_in++;
        }
        there_is_out_crash = false;
      
      // Old_val1 = Old_val2;
      // Old_val2 = Val_current;
    // }
    time_cur = millis();
  }
}

void check_current2(float deltaAdc) {
  if (millis() - time_cur > 2) {
    // sumVal = 0;
    // Val_array[k] = analogRead(CURRENT_SENSOR);//doc du lieu camr bien
    // k++;

    // if (k > 99)
    // {
    //   k = 0;
    // }

    // if (first_time_adc)
    // {
    //     for (int i = 0; i < 100; i++)
    //     {
    //       Val_array[i] =  Val_array[k];
    //     }
    //     first_time_adc = false;
    // }
    
    // for (int i = 0; i < 100; i++)
    // {
    //   sumVal = sumVal + Val_array[k];
    // }
    // // if (k > 9) {
    //   Val_current = sumVal / 100;
      // k = 0;
      // sumVal = 0;
      // Serial.println("Val_current: " + String(Val_current));
      // float delta = (Val_current-Old_val)/Old_val;
      // Serial.println("delta: " + String(delta));
      Serial.println("IN CRASH Val_current: " + String(Val_current));
      // Serial.println("Old_val1: " + String(Old_val1));
      // Serial.println("Old_val2: " + String(Old_val2));
      //Serial.println("Val_current: " + String(Val_current));
      if (!there_is_in_crash)//(((Old_val1 > 800) && (Old_val2 >= Old_val1 * deltaAdc) && (Val_current >= Old_val2 * deltaAdc)) || (Val_current > Adc_max)) {
      {
        time_cur = millis();
        return;
      }
        // dk_current = false;
        // //chạy thêm 1 đoạn
        // for (int i = 0; i < 500; i++) {
        //   digitalWrite(STEP, HIGH);
        //   delayMicroseconds(speed_min);
        //   digitalWrite(STEP, LOW);
        //   delayMicroseconds(speed_min);
        // }
        //        Motor_stop();
        dk_current = false;
        // start motor again
        // digitalWrite(EN_Pin, LOW);  // kích hoạt
        // digitalWrite(DIR, stt_direction);

        Lanchar = 's';
        WifiChar = 's';
        dk_run = false;
        Serial.println("impact delta");
        Serial.println("Old_val1: " + String(Old_val1_make_crash));
        Serial.println("Old_val2: " + String(Old_val2_make_crash));
        Serial.println("Val_current: " + String(Val_current_make_crash));
        delay(100);
        digitalWrite(DIR, 0);
        // longbuzz();
        stt_direction = 0;
        step_in2 = 0;
        step_in = Step_speed_min - step_in;
        there_is_in_crash = false;
      
      // Old_val1 = Old_val2;
      // Old_val2 = Val_current;
    // }
    time_cur = millis();
  }
}

// void check_current2() {
//   // Serial.println(analogRead(CURRENT_SENSOR));
//   //  if (millis() - time_cur > 10) {
//   Val_array[k] = analogRead(CURRENT_SENSOR);
//   k++;
//   if (k > 4) {
//     Val_current = (Val_array[0] + Val_array[1] + Val_array[2] + Val_array[3] + Val_array[4]) / 5;
//     k = 0;
//          Serial.println(Val_current);
//     if (Val_current >= Adc_max) {
//       dk_current = false;
//       Lanchar = 's';
//       WifiChar = 's';
//       Motor_stop();
//       stt_direction = 0;
//       step_in2 = 0;
//       step_in = Step_speed_min - step_in;
//       dk_run = false;
//       Serial.println("impact adcmax");
//       Serial.println("Val_current: " + String(Val_current));
//     }
//     Old_val = Val_current;
//   }
//   //    time_cur = millis();
//   //  }
// }