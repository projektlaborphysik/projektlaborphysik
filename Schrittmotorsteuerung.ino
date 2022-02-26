//Definiert die Pins, an denen Motor etc. an den Arduino angeschlossen werden

int dir = 2; //Direction Pin des Schrittmotors
int ste = 3; //Step Pin des Schrittmotors
int ena = 7;  //Enable Pin des Schrittmotors
int m1 = 8;  //Microstep1 Pin des Schrittmotors
int m2 = 9;  //Microstep2 Pin des Schrittmotors
int m3 = 10; //Microstep3 Pin des Schrittmotors
int R = 4; //Pin für den rechten Endschalter
int L = 5; //Pin für den linken Endschalter
int button = 6; //Pin für Hauptschalter


int usDelay = 2000; //Delay zwischen HIGH und LOW für einen Schritt in Mikrosekunden;
int k = 1; //Gibt Richtung an, kann 1 oder -1 sein

int steps = 50; //Schritte, die in jede Richtung in jedem Loop gemacht werden


int  run = 1; //Gibt an, ob der Motor laufen darf, gegeben durch den Hauptschalter
bool ts = false; //Tasterstatus des Buttons
bool tsold = false; //Tasterstatus des Buttons in der letzten Iteration

void setup() {
  pinMode(dir, OUTPUT);  //Die "Motor Steuerungs Pins" werden als Output Pins definiert
  pinMode(ste, OUTPUT); 
  pinMode(ena, OUTPUT);
  
  pinMode(L, INPUT); //Die Schalter Pins werden als Input Pins definiert
  pinMode(R, INPUT);
  pinMode(button, INPUT); 


  digitalWrite(ena, LOW); //Damit der Motor läuft, muss der Enable Pin LOW sein

  
  pinMode(m1, OUTPUT); //Die Microstep Pins werden als Output Pin definiert
  pinMode(m2, OUTPUT); 
  pinMode(m3, OUTPUT);

  //Vollschritt:        0 0 0
  //Halbschritt:        1 0 0
  //Viertelschritt:     0 1 0
  //Achtelschritt:      1 1 0
  //Sechzehntelschritt: 1 1 1
  
  digitalWrite(m1, HIGH); //Mit der obigen Tabelle und diesen drei Zeilen kann eingestellt werden, was für Schritte (Microsteps) der Motor machen soll
  digitalWrite(m2, HIGH);
  digitalWrite(m3, HIGH);
  
}

void check_LR() { //Diese Funktion überprüft, ob die Endschalter gedrückt sind, oder ob sich der Motor noch weiter drehen darf
  bool tsL = digitalRead(L); //Tasterstatus des linken bzw. rechten Endschalters
  bool tsR = digitalRead(R);
  if (run == 1) { //Darf sich nur drehen, wenn der Hauptschalter das erlaubt
    if(k == 1 && tsR) { //Der Motor darf sich nur nach rechts drehen, wenn der rechte Endschalter es erlaubt
      do_step();
    } else if(k == -1 && tsL) { //Der Motor darf sich nur nach rechts drehen, wenn der rechte Endschalter es erlaubt
      do_step();
    }
    
   }    
}

void do_step() { //Führt einen Schritt (bzw. Microstep) durch
    digitalWrite(ste, HIGH);
    delayMicroseconds(usDelay);
    digitalWrite(ste, LOW);
    delayMicroseconds(usDelay);  
}

void loop() {
  ts  = digitalRead(button); //Der Tasterstatus des Hauptschalters wird festgestellt
  if(ts==true && tsold==false) { //Wenn der Taster gerade (im Vergleich zur letzten Überprüfung) geschlossen wurde, dann wird run geändert 
                                 //Der Taster muss also gedrückt werden, um zu ändern, ob der Motor sich drehen darf
                                 //Da der Tasterstatus allerdings vgl.weise selten geprüft wird, muss der Taster lange gedrückt werden
    run *= -1;
  }
  tsold = ts;
  
  digitalWrite(dir, LOW); //Die Drehrichtung wird eingestellt und k daher geändert
  k *= -1;
  for(int i = 0; i < steps; ++i) { //Der Motor macht (wenn er darf) steps Schritte
    test();
  }
  delay(1000); //Warte 1 s
  
  ts  = digitalRead(button); //Der Tasterstatus des Hauptschalters wird festgestellt
  if(ts==true && tsold==false) { //...
    run *= -1;
  }
  tsold = ts;
  
  digitalWrite(dir, HIGH); //Die entgegengesetzte Richtung wird eingestellt 
  k *= -1;
  for(int i = 0; i < steps; ++i) {
    test();
  }
  delay(1000);
   
}
