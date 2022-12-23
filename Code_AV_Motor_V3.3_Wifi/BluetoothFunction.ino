//Bluetooth Function

void BluetoothInt() {
  String BleName = "BLE AV Motor_" + chipId;
  byte len = BleName.length() + 1;
  char CName[len];
  BleName.toCharArray(CName, len);
  BLEDevice::init(CName);
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_NOTIFY);

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    BLECharacteristic::PROPERTY_WRITE);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void Read_Bluetooth() {
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);                   // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();  // restart advertising
    Serial.println("Disconnect!");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    Serial.println("Connected!");
    //        pTxCharacteristic->setValue(&txValue, 1);
    //        pTxCharacteristic->notify();
    oldDeviceConnected = deviceConnected;
  }
  if (Blechar == 'u' && digitalRead(SW1) == 1 && dk_bluetooth == true) {
    time_run = millis();
    if (stt_direction == 1) {
      step_in = Step_speed_min - step_in;
      stt_direction = 0;
    }
    dk_bluetooth = false;
    Motor_run(3);
  } else if (Blechar == 'd' && digitalRead(SW2) == 1 && dk_bluetooth == true) {
    time_run = millis();
    if (stt_direction == 0) {
      step_in = Step_speed_min - step_in;
      stt_direction = 1;
    }
    dk_bluetooth = false;
    Motor_run(3);
  } else if (Blechar == 's') {
    digitalWrite(EN_Pin, 1);
    step_in2 = 0;
    dk_bluetooth = true;
    // buzz(1);
  }
}
