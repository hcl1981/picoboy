//Beispiel zur Tonausgabe auf dem Picoboy

# define SPK 15

void setup(){
  for (int i = 1; i < 6; i++) {   // Zählschleife: i nimmt nacheinander die Werte von 1 bis 5 an
    tone(SPK, i* 1000);           // Ton einschalten - Frequenz: i * 1000 Hz
    delay(100);                   // 100 Millisekunden pausieren
    noTone(SPK);                  // Ton abschalten
    delay(200);                   // 200 Millisekunden pausieren
  }

  delay(500);                     // 500 Millisekunden pausieren

  tone(SPK, 4000, 150);           // Ton ausgeben - Frequenz: 4000 Hz, Dauer: 150 Millisekunden

                                  //Beachte: Die Tonausgabe blockiert den Picobiy nicht.
                                  //Das Programm läuft also ohne Pause weiter
}

void loop(){}
