/* Arduino tutorial - Buzzer / Piezo Speaker
   More info and circuit: http://www.ardumotive.com/how-to-use-a-buzzer-en.html
   Dev: Michalis Vasilakis // Date: 9/6/2015 // www.ardumotive.com */

const int buzzer = A3; //buzzer to arduino pin 9

void setup(){
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
}

void loop(){
  CorrectSound(); // Send 1KHz sound signal...
  delay(1000);        // ...for 1 sec
  WrongSound();     // Stop sound...
  delay(1000);        // ...for 1sec
}

void CorrectSound() {
  tone(buzzer,440*5);
  delay(100);
  tone(buzzer,554*5);
  delay(100);
  tone(buzzer,440*5);
  delay(100);
  tone(buzzer,554*5);
  delay(350);
  noTone(buzzer);
}

void WrongSound() {
  tone(buzzer,784);
  delay(250);
  tone(buzzer,523);
  delay(500);
  noTone(buzzer);
}