int cathode[8] = {9, A2, A3, 6, 2, 7, 11, 12};
int anode[8] = {5, 10, 4, 8, A0, 3, A1, A4};

void setup() {

  for (int i = 0; i < 8; i++) {
    pinMode(anode[i], OUTPUT);
    digitalWrite(anode[i], LOW);
  }

  for (int i = 0; i < 8; i++) {
    pinMode(cathode[i], OUTPUT);
    digitalWrite(cathode[i], HIGH);
  }

}

void loop() {

  for ( int i = 0; i < 8; i++) {

    for ( int j = 0; j < 8; j++) {

      digitalWrite(anode[j], HIGH);
      digitalWrite(cathode[i], LOW);
      delay(100);
      digitalWrite(anode[j], LOW);
      digitalWrite(cathode[i], HIGH);

    }

  }

}
