int directionPin_L = 4;
int pwmPin_L = 5;

int directionPin_R = 7;
int pwmPin_R = 6;

bool direction_L;
bool direction_R;

int PK_R = 132;
int PK_L = 132;

// sensor pins
int SensorFL_pin = A1;
int SensorML_pin = A2;
int SensorM_pin = A3;
int SensorMR_pin = A4;
int SensorFR_pin = A5;

bool SensorFL;
bool SensorML;
bool SensorM;
bool SensorMR;
bool SensorFR;

enum status {
  NIETS,
  VOORUIT,
  bLINKS,
  bRECHTS,
  KRUISING,
  correctieNaarRECHTS,
  correctieNaarLINKS
};
status Status;

void setup() {
  pinMode(directionPin_L, OUTPUT);
  pinMode(pwmPin_L, OUTPUT);
  pinMode(directionPin_R, OUTPUT);
  pinMode(pwmPin_R, OUTPUT);
  pinMode(SensorFL_pin, INPUT);
  pinMode(SensorML_pin, INPUT);
  pinMode(SensorM_pin, INPUT);
  pinMode(SensorMR_pin, INPUT);
  pinMode(SensorFR_pin, INPUT);

  pinMode(13, OUTPUT);

  display_setup();

  TCCR0B = 1;
  Serial.begin(9600);
}

// FUNCTIES //
//////////////
void neutralise() {
  SensorFL = digitalRead(SensorFL_pin);
  SensorML = digitalRead(SensorML_pin);
  SensorM = digitalRead(SensorM_pin);
  SensorMR = digitalRead(SensorMR_pin);
  SensorFR = digitalRead(SensorFR_pin);
}

void detectState() {
  neutralise();
  if (SensorFL && SensorML && !SensorM && SensorMR && SensorFR) {
      Status = VOORUIT;
  }
  else if (!SensorFL && !SensorML && !SensorM && SensorMR && SensorFR) {
      Status = bLINKS;
  }
  else if (SensorFL && SensorML && !SensorM && !SensorMR && !SensorFR) {
      Status = bRECHTS;
  }
  else if (!SensorFL && !SensorML && !SensorM && !SensorMR && !SensorFR) {
      Status = KRUISING;
  }
  else if (SensorFL && SensorML && !SensorM && !SensorMR && SensorFR) {
      Status = correctieNaarRECHTS;
  }
  else if (SensorFL && SensorML && SensorM && !SensorMR && SensorFR) {
      Status = correctieNaarRECHTS;
  }
  else if (SensorFL && !SensorML && !SensorM && SensorMR && SensorFR) {
      Status = correctieNaarLINKS;
  }
  else if (SensorFL && !SensorML && SensorM && SensorMR && SensorFR) {
      Status = correctieNaarLINKS;
  }
  else if (SensorFL && SensorML && SensorM && SensorMR && SensorFR) {
      Status = NIETS;
  }
  // else {
  //     Status = NIETS;
  //     Serial.println(Status);
  // }
  
  Serial.println(status);
}

//// WIELEN ///
void Linkerwiel_Vooruit() {
  digitalWrite(directionPin_L, HIGH);
  analogWrite(pwmPin_L, PK_L);
}

void Rechterwiel_Vooruit() {
  digitalWrite(directionPin_R, LOW);
  analogWrite(pwmPin_R, PK_R);
}

void Linkerwiel_Achteruit() {
  digitalWrite(directionPin_L, LOW);
  analogWrite(pwmPin_L, PK_L);
}

void Rechterwiel_Achteruit() {
  digitalWrite(directionPin_R, HIGH);
  analogWrite(pwmPin_R, PK_R);
}

void Remmen(bool links, bool rechts) {
  if (links) { analogWrite(pwmPin_L, 0); }
  if (rechts) { analogWrite(pwmPin_R, 0); }
}

//// BEWEGING ///
void Vooruit(int timeout) {
  Linkerwiel_Vooruit();
  Rechterwiel_Vooruit();
  delay(timeout);
}
void Achteruit(int timeout) {
  Linkerwiel_Achteruit();
  Rechterwiel_Achteruit();
  delay(timeout);
}

void Linksaf(int timeout, bool checkSensor) {
  unsigned long turnTimeout = millis() + timeout;
  if (checkSensor) {
      while (!(SensorFL && SensorML && !SensorM && SensorMR && SensorFR) || millis() < turnTimeout) {
        Rechterwiel_Vooruit();
        Linkerwiel_Achteruit();
        neutralise();
      }
  } else {
    while (millis() < turnTimeout) {
      Rechterwiel_Vooruit();
      Linkerwiel_Achteruit();
      neutralise();
    }
  }
  Remmen(true, true);
}
void CorrectieLinks(int timeout, bool checkSensor) {
  unsigned long turnTimeout = millis() + timeout;
  if (checkSensor) {
      while (!(SensorFL && SensorML && !SensorM && SensorMR && SensorFR) || millis() < turnTimeout) {
      Rechterwiel_Vooruit();
      Remmen(true, false);
      neutralise();
    }
  } else {
    while (millis() < turnTimeout) {
      Rechterwiel_Vooruit();
      Remmen(true, false);
      neutralise();
    }
  }
  Remmen(true, true);
}

void Rechtsaf(int timeout, bool checkSensor) {
  unsigned long turnTimeout = millis() + timeout;
  if (checkSensor) {
    while (!(SensorFL && SensorML && !SensorM && SensorMR && SensorFR) || millis() < turnTimeout) {
      Rechterwiel_Achteruit();
      Linkerwiel_Vooruit();
      neutralise();
    }
  } else {
    while (millis() < turnTimeout) {
      Rechterwiel_Achteruit();
      Linkerwiel_Vooruit();
      neutralise();
    }
  }
  Remmen(true, true);
}

void CorrectieRechts(int timeout, bool checkSensor) {
  unsigned long turnTimeout = millis() + timeout;
  if (checkSensor) {
    while (!(SensorFL && SensorML && !SensorM && SensorMR && SensorFR) || millis() < turnTimeout) {
      Remmen(false, true);
      Linkerwiel_Vooruit();
      neutralise();
    }
  } else {
    while (millis() < turnTimeout) {
      Remmen(false, true);
      Linkerwiel_Vooruit();
      neutralise();
    }
  }
  Remmen(true, true);
}

void CheckVooruit(int richting, bool doorgaan) {
  // 0 = links
  // 1 = rechts
  Vooruit(300);
  Remmen(true, true);
  detectState();
  neutralise();
  if (!SensorFL && !SensorML && !SensorM && !SensorMR && !SensorFR) {
    Vooruit(300);
    Remmen(true, true);
    neutralise();
    if (!SensorFL && !SensorML && !SensorM && !SensorMR && !SensorFR) {
      digitalWrite(13, HIGH);
      while (true);
    }
    // while (true) {
    //   for (int i = 1; i < 10; i++) {
    //   digitalWrite(13, HIGH);
    //   delay(1000);
    //   digitalWrite(13, HIGH);
    //   delay(1000);
    //   }
    // }
  } else {
    if (Status == NIETS || !doorgaan) {
      Achteruit(350);
      Remmen(true, true);
      if (richting == 0) {
        Linksaf(600, false);
      } else if (richting == 1) {
        Rechtsaf(600, false);
      }
    } else if (Status == VOORUIT) {
      Vooruit(50);
    }
  }
}

//////////////
unsigned long milliTracker = 0;

//// LOOP ////
//////////////
void loop() {
  unsigned long currentMillis = millis();
  detectState();
  if (Status == VOORUIT) {
    milliTracker = currentMillis;
    Vooruit(5);
  }
  else if (Status == bLINKS) {
    milliTracker = currentMillis;
    delay(100);
    CheckVooruit(0, false);
  }
  else if (Status == bRECHTS) {
    milliTracker = currentMillis;
    delay(100);
    CheckVooruit(1, true);
  }
  else if (Status == correctieNaarLINKS) {
    milliTracker = currentMillis;
    delay(100);
    CorrectieLinks(100, true);
  }
  else if (Status == correctieNaarRECHTS) {
    milliTracker = currentMillis;
    delay(100);
    CorrectieRechts(100, true);
  }
  else if (Status == KRUISING) {
    milliTracker = currentMillis;
    delay(100);
    CheckVooruit(0, false);
  }
  else if (Status == NIETS && ((currentMillis - milliTracker) > 500)) {
    CorrectieRechts(0, true);
  }

}