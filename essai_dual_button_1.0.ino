//Branchement des connexions
int button1=3;
int button2=3;
int button3=5;
int button4=5;
int button5=7;
int button6=7;
//Etat des boutons au demarrage 1=HIGH, 0=LOW
bool state1=1;
bool state2=1;
bool state3=1;
bool state4=1;
bool state5=1;
bool state6=1;

void setup() {
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  pinMode(button5, INPUT);
  pinMode(button6, INPUT);
}

void loop() {
  state1=digitalRead(button1);
  state2=digitalRead(button2);
  state3=digitalRead(button3);
  state4=digitalRead(button4);
  state5=digitalRead(button5);
  state6=digitalRead(button6);

  if (state1 == 0){
    activationButton1();
  }
  if (state2 == 0){
    activationButton2();
  }
  if (state3 == 0){
    activationButton3();
  }
  if (state4 == 0){
    activationButton4();
  }
  if (state5 == 0){
    activationButton5();
  }
  if (state6 == 0){
    activationButton6();
  }

  delay(1000);
}
/////////Mes Fonctions par phase
void activationButton1(){
  //Modification des parametres cibles de culture
  Serial.print("Incubation");
		//Temperature
    tempCibleHigh=24;
    tempCibleLow=24;
    tempCibleMoy=24;
		//Hygrometrie
    humiCibleHigh=100;
    humiCibleLow=95;
    humiCibleMoy=97.5;
    //Ventilation
		nombreEchangeAirCible = 1;
    //Duree
		maxDaysPerPhase = 21;
    miniDaysPerPhase = 12;
    //Phase
    phase = 1;
}

void activationButton2(){
  Serial.print("Primordia");
		//Temperature
    tempCibleHigh=15;
    tempCibleLow=10;
    tempCibleMoy=12.5;
		//Hygrometrie
    humiCibleHigh=100;
    humiCibleLow=95;
    humiCibleMoy=97.5;
    //Ventilation
		nombreEchangeAirCible = 6;
    //Duree
		maxDaysPerPhase = 5;
    miniDaysPerPhase = 3;
    //Phase
    phase = 2;
}

void activationButton3(){
  Serial.print("Fructification");
		//Temperature
    tempCibleHigh=21;
    tempCibleLow=10;
    tempCibleMoy=15.5;
		//Hygrometrie
    humiCibleHigh=85;
    humiCibleLow=80;
    humiCibleMoy=82.5;
    //Ventilation
		nombreEchangeAirCible = 6;
    //Duree
		maxDaysPerPhase = 7;
    miniDaysPerPhase = 4;
    //Phase
    phase = 3;
}

void activationButton4(){
  Serial.print("Gestion par serveur");
}


void activationButton5(){
  Serial.print("Periode Inter-Récolte");
  phase = 4;
  //Lancement de chrono decompte des jours avec alerte J+7 (reprise culture possible) et a J14(fin de periode inter recolte)
}


void activationButton6(){
  Serial.print("Veille");
  digitalWrite(heatPin, LOW);
  digitalWrite(humidityPin, LOW);
  digitalWrite(fanPin, LOW);
  phase = 5;
}


