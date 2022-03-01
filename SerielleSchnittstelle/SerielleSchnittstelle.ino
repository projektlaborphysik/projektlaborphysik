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
int pos = 0;

int steps = 100; //Schritte, die in jede Richtung in jedem Loop gemacht werden


int  run = 1; //Gibt an, ob der Motor laufen darf, gegeben durch den Hauptschalter
bool ts = false; //Tasterstatus des Buttons
bool tsold = false; //Tasterstatus des Buttons in der letzten Iteration

int val = 0;
String s = "";

void setup() {
  Serial.begin(9600);
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
  
  digitalWrite(m1, LOW); //Mit der obigen Tabelle und diesen drei Zeilen kann eingestellt werden, was für Schritte (Microsteps) der Motor machen soll
  digitalWrite(m2, LOW);
  digitalWrite(m3, LOW);
}


void Vollschritt() {
  digitalWrite(m1, LOW);
  digitalWrite(m2, LOW);
  digitalWrite(m3, LOW);
}

void Halbschritt() {
  digitalWrite(m1, HIGH);
  digitalWrite(m2, LOW);
  digitalWrite(m3, LOW);
}

void Viertelschritt() {
  digitalWrite(m1, LOW);
  digitalWrite(m2, HIGH);
  digitalWrite(m3, LOW);
}

void Achtelschritt() {
  digitalWrite(m1, HIGH);
  digitalWrite(m2, HIGH);
  digitalWrite(m3, LOW);
}

void Sechzehntelschritt() {
  digitalWrite(m1, HIGH);
  digitalWrite(m2, HIGH);
  digitalWrite(m3, HIGH);
}

void go(int steps) {
  bool allowed = true;
  int i=0;
  k = steps/abs(steps);
  //Andere Vorzeichen als im anderen Programm
  if(k==-1) {
    digitalWrite(dir, HIGH); //Die Drehrichtung wird eingestellt und k daher geändert
  } else {
    digitalWrite(dir, LOW); //Die Drehrichtung wird eingestellt und k daher geändert
  }
  while(allowed && i <= abs(steps)) {
    allowed = check_LR();
    i++;
    if(pos%5==0) {
      Serial.print("Position ");
      Serial.println(pos);  
    }
  }
  Serial.write("Ich bin angekommen");
}


bool check_LR() { //Diese Funktion überprüft, ob die Endschalter gedrückt sind, oder ob sich der Motor noch weiter drehen darf
  bool allowed = false;
  bool tsL = digitalRead(L); //Tasterstatus des linken bzw. rechten Endschalters
  bool tsR = digitalRead(R);
  if (run == 1) { //Darf sich nur drehen, wenn der Hauptschalter das erlaubt
    if(k == 1 && tsL) { //Der Motor darf sich nur nach rechts drehen, wenn der rechte Endschalter es erlaubt
      do_step();
      pos += 1;
      allowed = true;
    } else if(k == -1 && tsR) { //Der Motor darf sich nur nach rechts drehen, wenn der rechte Endschalter es erlaubt
      do_step();
      pos -= 1;
      allowed = true;
    } 
    if(tsL == false) {
      Serial.println("Linker Schalter ist gedrückt");
    }
    if(tsR == false) {
      Serial.println("Rechter Schalter ist gedrückt");
    } 
   }
   return allowed;    
}

String* split(String input, char delimiter) {
  Serial.println(input.length());
  char a[input.length()];
  input.toCharArray(a, input.length());
  //Serial.println("a:");
  //Serial.println(a);
  
  int num_del = 1;
  for(int i=0; i<input.length(); i++) {
    if(a[i] == delimiter) {
      Serial.print(a[i]);
      Serial.print(" : ");
      Serial.println(i);
      num_del += 1;
    }
  }

  int count = 0;
  if(num_del == 1) {
    String parts[3] = {input, "", ""};
  } else {
    String* parts = new String[num_del];
    
    for(int i=0; i<input.length(); i++) {
      if(a[i] != delimiter) {
        parts[count] = parts[count]+a[i]; 
      } else {
        count += 1;
      }
    }
    Serial.print("num_del = ");
    Serial.println(num_del);
    Serial.println(parts[0]);
    Serial.println(parts[1]);
    Serial.println(parts[2]);
    Serial.println("-");
    
    return parts;
  }
}

void do_step() { //Führt einen Schritt (bzw. Microstep) durch
    digitalWrite(ste, HIGH);
    delayMicroseconds(usDelay);
    digitalWrite(ste, LOW);
    delayMicroseconds(usDelay);  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()==0) {
    //Serial.println("nichts");
  } else {
    s = Serial.readString();
    Serial.println(" ");
    Serial.print("s: ");
    Serial.println(s);
    String* a = split(s, ' ');
    
    
    if(a[0] == "n") {
      Serial.println("Neue 0");
      pos = 0;
    }

    if(a[0] == "go") {
      if(a[1] == "to") {
        go(a[2].toInt()-pos);
      } else if(a[1] == "steps") {
        go(a[2].toInt());
      } else if(a[1] == "end") {
        if(a[2] == "L") {
          go(-10 - pos);
        } else if(a[2] == "R") {
          go(150 - pos);
        }
      }
    }

    if(a[0] == "activate") {
      digitalWrite(ena, LOW);
    } else if(a[1] == "deactivate") {
      digitalWrite(ena, HIGH);
    } else if(a[0] == "1") {
      Vollschritt();
    } else if(a[0] == "2") {
      Halbschritt();
    } else if(a[0] == "4") {
      Viertelschritt();
    } else if(a[0] == "8") {
      Achtelschritt();
    } else if(a[0] == "16") {
      Sechzehntelschritt();
    }

    

    
    //Serial.println(val);
    //Serial.println(char(val));
    //Serial.println("---");  
    delay(10);
  }
  

  

  
}
