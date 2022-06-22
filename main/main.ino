#define CONVERSIE 64

// VARIABLES
bool display = true;

// motor variables
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

// display
int NUMBERS[] =
{
  0b1111110, // 0
  0b0110000, // 1
  0b1101101, // 2
  0b1111001, // 3
  0b0110011, // 4
  0b1011011, // 5
  0b1011111, // 6
  0b1110000, // 7
  0b1111111, // 8
  0b1111011  // 9
};

int LETTERS[] =
{
    0b0001110, // L
    0b1100110, // R
    0b1000111, // F
    0b0000110  // I
};

// status & tracking
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
int junctions;
bool lastJunction; // true = rechts, false = links
bool junctionTaken;
unsigned long displayTimer;
unsigned long multiplexTimer;
bool programStarted;
bool deadEnd;
unsigned long deadEndTimer;

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
  junctions = 0;
  junctionTaken = false;
  lastJunction = false;
  displayTimer = 0;
  multiplexTimer = 0;
  deadEnd = false;
  deadEndTimer = 0;

  TCCR0B = 1;
}

// FUNCTIES //
//////////////

char x[2];

void updateDisplay()
{
  unsigned long currentTime = (millis() / CONVERSIE);
  int timeDifference = currentTime - displayTimer;
  if (programStarted) {
    if (deadEnd) {
      deadEndTimer = currentTime;
      if (currentTime - deadEndTimer < 2000) {
        // DEAD END
        multiplexTimer = currentTime;
        int multiplexDifference = currentTime - multiplexTimer;
        if (multiplexDifference < 10) {
        // write display 1
        activateDisplay(1);
        writeNumber(LETTERS[4]);
        } else if (multiplexDifference > 20) {
        // update timer
        multiplexTimer = currentTime;
        } else if (multiplexDifference > 10) {
          // write display 2
          activateDisplay(2);
          writeNumber(LETTERS[5]);
        }
      } else {
        deadEnd = false;
      }
    } else {
      if (timeDifference < 1000 || junctions == 0) {
      // write number
      sprintf(x, "%d", junctions);
      if (junctions <= 9) {
        // write display 1
        activateDisplay(1);
        writeNumber(NUMBERS[x[0] - '0']);
      } else {
        // TWEE CIJFERS
        multiplexTimer = currentTime;
        int multiplexDifference = currentTime - multiplexTimer;
        if (multiplexDifference < 10) {
        // write display 1
        activateDisplay(1);
        writeNumber(NUMBERS[x[0] - '0']);
        } else if (multiplexDifference > 20) {
        // update timer
        multiplexTimer = currentTime;
        } else if (multiplexDifference > 10) {
          // write display 2
          activateDisplay(2);
          writeNumber(NUMBERS[x[1] - '0']);
        }
      }
      } else if (timeDifference > 2000) {
        // update timer
        displayTimer = currentTime;
      } else if (timeDifference > 1000) {
        // write letter
        if (lastJunction) {
          // write rechts
          activateDisplay(2);
          writeNumber(LETTERS[1]);
        } else if (!lastJunction) {
          // write links
          activateDisplay(1);
          writeNumber(LETTERS[0]);
        }
      }
    }
  } else {
    for (int i = 5; i > 0; i--) {
      sprintf(x, "%d", i);
      activateDisplay(1);
      writeNumber(NUMBERS[x[0] - '0']);
      delay(64000);
      }
  programStarted = true;
  }
}

void sleep(unsigned long ms)
{
  unsigned long expirationTime = millis() + (ms * CONVERSIE);
  while(expirationTime >= millis())
  {
    updateDisplay();
  }
}

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
  updateDisplay();
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
  sleep(timeout);
}
void Achteruit(int timeout) {
  Linkerwiel_Achteruit();
  Rechterwiel_Achteruit();
  sleep(timeout);
}

void VindPad(int timeout, bool checkSensor) {
  timeout = timeout * 64;
  unsigned long turnTimeout = millis() + timeout;
  while ((Status != VOORUIT && Status != bLINKS && Status != bRECHTS) || millis() < turnTimeout) {
      Rechterwiel_Vooruit();
      Linkerwiel_Achteruit();
      detectState();
  }
  Remmen(true, true);
}

void Linksaf(int timeout, bool force) {
  timeout = (timeout * 64);
  unsigned long turnTimeout = millis() + timeout;
  if (force == true) {
    while (millis() < turnTimeout) {
      if (Status == VOORUIT) {
        break;
      }
      Rechterwiel_Vooruit();
      // Linkerwiel_Achteruit();
      detectState();
    }
  } else {
    while ((millis() < turnTimeout) && (!(Status == VOORUIT) && !(Status == correctieNaarRECHTS) && !(Status == correctieNaarLINKS))) {
      Rechterwiel_Vooruit();
      // Linkerwiel_Achteruit();
      detectState();
    }
  }
  Remmen(true, true);
  junctionTaken = true;
  lastJunction = false;
}
void CorrectieLinks(int timeout, bool checkSensor) {
  timeout = timeout * 64;
  unsigned long turnTimeout = millis() + timeout;
  if (checkSensor) {
      while ((Status != VOORUIT && Status != bLINKS && Status != bRECHTS) || millis() < turnTimeout) {
        Rechterwiel_Vooruit();
        Remmen(true, false);
        detectState();
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

void Rechtsaf(int timeout, bool force) {
  timeout = timeout * 64;
  unsigned long turnTimeout = millis() + timeout;
  if (force == true) {
    while (millis() < turnTimeout) {
      if (Status == VOORUIT) {
        break;
      }
      Rechterwiel_Vooruit();
      // Linkerwiel_Achteruit();
      detectState();
    }
  } else {
    while ((millis() < turnTimeout) && (!(Status == VOORUIT) && !(Status == correctieNaarRECHTS) && !(Status == correctieNaarLINKS))) {
      // Rechterwiel_Achteruit();
      Linkerwiel_Vooruit();
      detectState();
    }
  }
  Remmen(true, true);
  junctionTaken = true;
  lastJunction = true;
}

void CorrectieRechts(int timeout, bool checkSensor) {
  timeout = timeout * 64;
  unsigned long turnTimeout = millis() + timeout;
  if (checkSensor) {
    while ((Status != VOORUIT && Status != bLINKS && Status != bRECHTS) || millis() < turnTimeout) {
      Remmen(false, true);
      Linkerwiel_Vooruit();
      detectState();
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
    Vooruit(200);
    Remmen(true, true);
    neutralise();
    // robot detecteert eindblock na checken
    if (!SensorFL && !SensorML && !SensorM && !SensorMR && !SensorFR) {
      int i = 10;
      unsigned long currentTime = (millis() / CONVERSIE);
      unsigned long expirationTimer = currentTime;
      multiplexTimer = currentTime;
      while (i > -1) {
        ///////////////
        // TEL NAAR 10
        sprintf(x, "%d", i);
        currentTime = (millis() / CONVERSIE);
        int multiplexDifference = currentTime - multiplexTimer;
        if (i <= 9) {
          activateDisplay(1);
          writeNumber(NUMBERS[x[0] - '0']);
        } else {
          if (multiplexDifference < 10) {
          // write display 1
          activateDisplay(1);
          writeNumber(NUMBERS[x[0] - '0']);
          } else if (multiplexDifference > 20) {
          // update timer
          multiplexTimer = currentTime;
          } else if (multiplexDifference > 10) {
            // write display 2
            activateDisplay(2);
            writeNumber(NUMBERS[x[1] - '0']);
          }
          // TEL NAAR 10
          //////////////
        }
        if ((currentTime - expirationTimer) >= 1000) {
        i--;
        expirationTimer = currentTime;
        }
      }
      multiplexTimer = currentTime;
      while (true) {
        currentTime = (millis() / CONVERSIE);
        int multiplexDifference = currentTime - multiplexTimer;
          if (multiplexDifference < 10) {
          // write display 1
          activateDisplay(1);
          writeNumber(LETTERS[2]);
          } else if (multiplexDifference > 20) {
          // update timer
          multiplexTimer = currentTime;
          } else if (multiplexDifference > 10) {
            // write display 2
            activateDisplay(2);
            writeNumber(LETTERS[3]);
          }
      };
    }
  } else {
    // robot vind geen eindblock
    // vindt of geen baan of mag niet doorrijden (omdat bocht links aanwezig is)
    if (Status == NIETS || doorgaan == false) {
      sleep(100);
      Achteruit(400);
      Remmen(true, true);
      if (richting == 1) {
        Rechtsaf(1200, true);
        // Rechtsaf(800, false);
      } else if (richting == 0) {
        Linksaf(1200, true);
        // Linksaf(800, false);
      } else {
        Achteruit(1000);
      }
    } else if (Status == VOORUIT) {
      Vooruit(50);
      junctionTaken = true;
    }
  }
}

//////////////
unsigned long milliTracker = 0;

//// LOOP ////
//////////////
void loop() {
  unsigned long currentMillis = (millis() / 64);
  detectState();
  if (Status == VOORUIT) {
    milliTracker = currentMillis;
    if (junctionTaken) {
      junctionTaken = false;
      junctions++;
    }
    Vooruit(5);
  }
  else if (Status == bLINKS) {
    milliTracker = currentMillis;
    delay(100 * CONVERSIE);
    Linksaf(1200, true);
    // CheckVooruit(0, false);
  }
  else if (Status == bRECHTS) {
    milliTracker = currentMillis;
    delay(100 * CONVERSIE);
    // Rechtsaf(1200, true);
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
  else if (Status == NIETS && ((currentMillis - milliTracker) > 250)) {
    deadEnd = true;
    VindPad(0, true);
  }
}