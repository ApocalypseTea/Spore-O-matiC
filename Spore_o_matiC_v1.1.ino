//Gestion de la date et jours de culture
#include "RTC.h"

//Librairie du capteur temp&hum
#include <DHT.h>

//Librairies de l'écran
#include <Arduino.h>
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//définition capteur DHT et pin
#define DHTTYPE DHT22
#define DHTPIN 2

//Constructeur pour l'ecran
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);    //Software I2C

// Initialisation capteur temp & hum
DHT dht(DHTPIN, DHTTYPE);

//Branchement des connexions
//duals buttons
int button1=6;
int button2=7;
int button3=8;
int button4=9;

//Relais
int heatPin = 3;
int humidityPin = 5;
int fanPin = 4;

//Etat des boutons au demarrage 1=HIGH, 0=LOW
bool state1=1;
bool state2=1;
bool state3=1;
bool state4=1;

//Variables de stockage des dates et d'incrementation de jours par phase
RTCTime beginningTime;
int nbJoursTotal;
int nbJoursIncub;
int nbJoursPrimordia;
int nbJoursFructi;
int nbJoursInterRecolte;
int phase;
int maxDaysPerPhase;
int miniDaysPerPhase;

//Variables de culture
float tempCibleHigh;
float tempCibleLow;
float tempCibleMoy;
float temperature;

float humiCibleHigh;
float humiCibleLow;
float humiCibleMoy;
float humidity;

//Variables de gestion de la ventilation
unsigned long timeStampDernierCycleVentilation=0;
long tempsVentilation = 45000; //durée de la ventilation = 45 secondes
int nombreEchangeAirCible;
long duree = 0; //durée entre 2 cycles de ventilation

void setup() {
  //Fonction Serial
  Serial.begin(9600);

  //Dual Buttons
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);

  //Relais
  pinMode(heatPin, OUTPUT);
  pinMode(humidityPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  dht.begin();

  //Horloge  
  RTC.begin();
    //METTRE L'HORLOGE A LA DATE DU JOUR DE LA COMPILATION
  RTCTime startTime(27, Month::SEPTEMBER, 2024, 18, 38, 00, DayOfWeek::FRIDAY, SaveLight::SAVING_TIME_ACTIVE);
  RTC.setTime(startTime);
  beginningTime = startTime;

  //Ecran
  u8g2.begin();
}

void loop() {
  //Surveillance de l'activation des boutons
  buttonRead();

  //Mise à jour de l'heure
  RTCTime currentTime;
  RTC.getTime(currentTime);

  //Affichage date et nombre jours de culture (En CONSOLE)
  afficheLaDate(currentTime);
  nbJoursCulture(currentTime);

  //Gestion de la temperature et humidite
  suiviCapteur();
  tempManagement(temperature);
  humiManagement(humidity);

  //Gestion de la ventilation
  gestionVentilo();

  //Gestion de l'affichage sur ECRAN
  affichageOLED();
  
  //Toutes les secondes recommencer le processus de surveillance/gestion/affichage
  delay(1000);
}

//////////////////////////////FONCTIONS DU PROGRAMME//////////////////////////////////////////
//Activation des boutons et modification des phases de culture
void buttonRead(){
  state1=digitalRead(button1);
  state2=digitalRead(button2);
  state3=digitalRead(button3);
  state4=digitalRead(button4);

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
}

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
  calculIntervalleVentilation();
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
  calculIntervalleVentilation();
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
  calculIntervalleVentilation();
  //Duree
	maxDaysPerPhase = 7;
  miniDaysPerPhase = 4;
  //Phase
  phase = 3;
}

void activationButton4(){
  Serial.print("Veille");
  digitalWrite(heatPin, LOW);
  digitalWrite(humidityPin, LOW);
  digitalWrite(fanPin, LOW);
  phase = 6;
}

//Affichage date et nombre de jours de culture en CONSOLE (+ serveur en V2)
int nbJoursCulture(RTCTime currentTime){
  //Test réalisé en secondes mais à passer en heures .getHour() pour la fonction finale
  int result=currentTime.getHour()-beginningTime.getHour();
  Serial.print("La culture dure depuis ");
  Serial.print(result);
  Serial.println(" heures");
  //A chaque tour, on ajoute une journée (passer la condition à 23 pour la fonction finale)
  if (result == 23) {
    nbJoursTotal++;
    switch (phase){
      //Incrementation des compteur selon phase de culture
      case 1:
        nbJoursIncub++;
        Serial.print("La période d'incubation dure depuis ");
        Serial.print(nbJoursIncub);
        Serial.println(" jours.");
      break;
      case 2:
        nbJoursPrimordia++;
        Serial.print("La période de primordia dure depuis ");
        Serial.print(nbJoursPrimordia);
        Serial.println(" jours.");
      break;
      case 3:
        nbJoursFructi++;
        Serial.print("La période de fructification dure depuis ");
        Serial.print(nbJoursFructi);
        Serial.print(" jours.");
      break;
      //Periode Inter Recolte
      case 4:
        nbJoursInterRecolte++;
        Serial.print("La période inter récolte dure depuis ");
        Serial.print(nbJoursInterRecolte);
        Serial.print(" jours/14");
        //rappelCulture();
      break;
      //Mode veille, remise à zero des compteurs
      case 5:
        nbJoursIncub=0;
        nbJoursPrimordia=0;
        nbJoursFructi=0;
        nbJoursInterRecolte=0;
        nbJoursTotal=0;
        Serial.print("Compteurs à 0");        
      break;
      
      default:
        //envoi un message d'erreur sur l'ecran
        Serial.print("ERREUR DECOMPTE DE JOURS"); 
      break;
    }
  }
  return result;
}

//Affichage de la date en CONSOLE (+ serveur en V2)
void afficheLaDate(RTCTime currentTime){  
  // Print out date (DD/MM//YYYY)
  Serial.print(currentTime.getDayOfMonth());
  Serial.print("/");
  Serial.print(Month2int(currentTime.getMonth()));
  Serial.print("/");
  Serial.print(currentTime.getYear());
  Serial.print(" - ");
  
  // Print time (HH/MM/SS)
  Serial.print(currentTime.getHour());
  Serial.print(":");
  Serial.print(currentTime.getMinutes());
  Serial.print(":");
  Serial.println(currentTime.getSeconds());
}

/* NON UTILISE CAR PAS ASSEZ DE BOUTONS - A GARDER POUR GESTION PAR SERVEUR ULTERIEURE
void rappelCulture(){
  if (nbJoursInterRecolte == 7) {
    Serial.println("J7 : Reprise culture ok");
  } else if (nbJoursInterRecolte == 14){
    Serial.println("J14 : fin de période inter-récolte");
  }
}*/

//Gestion de la temperature
//Recuperation et Affichage des données EN CONSOLE
void suiviCapteur(){
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  // gestion erreur de lecture
  if(isnan(humidity) || isnan(temperature)){
    Serial.println("Erreur de lecture du capteur DHT");
    return;
  }

  Serial.print("Temp:");
  Serial.println(temperature);
  Serial.print("Humi:");
  Serial.println(humidity);
}

void tempManagement(float temperature){
  Serial.println("lancement de fonction");
  if(temperature < tempCibleLow && digitalRead(heatPin) == LOW){
      digitalWrite(heatPin, HIGH);
      Serial.println("Il fait trop froid");
      Serial.println("Coussin en marche");
        
  } else if (temperature > tempCibleMoy && digitalRead(heatPin) == HIGH){
      digitalWrite(heatPin, LOW);
      Serial.println("Il fait trop chaud");
      Serial.println("Coussin à l'arret");
  }
}

void humiManagement(float humidity){
  Serial.println("test lancement de fonction");

  //Si l'humidité mesuree est plus basse que la cible basse et que l'humidificateur est à l'arret -> Allume le 
  if(humidity < humiCibleLow && digitalRead(humidityPin) == LOW){
    digitalWrite(humidityPin, HIGH);
    Serial.println("Il fait trop sec");
    Serial.println("Vapeur en marche");

  //Si l'humidité est plus haute que la cible moyenne et que l'humidificateur est allumé -> Eteins le
  } else if (humidity > humiCibleMoy && digitalRead(humidityPin) == HIGH){
    digitalWrite(humidityPin, LOW);
    Serial.println("Il va faire trop humide");
    Serial.println("Vapeur à l'arret");
  }
}

//Gestion de la ventilation
void calculIntervalleVentilation(){
  //temps entre chaque début de ventilation
  //On divise 1 heure par le nombre d'echanges
	duree = 60 / nombreEchangeAirCible;
  //On multiplie par 60 000 pour obtenir des millisecondes
	duree *= 60000;	
}

void gestionVentilo(){

  //Si le ventilateur est allume
  if (digitalRead(fanPin) == HIGH){

    //Verifier si le delai de fonctionnement est atteint
    if(millis() - timeStampDernierCycleVentilation >= tempsVentilation){
      digitalWrite (fanPin, LOW);      
    }
  //Si le ventilateur est eteint
  } else if (digitalRead(fanPin) == LOW){
    //verifier si le delai d'attente entre 2 ventilation est atteint (= delai entre 2 ventilations)
    if(millis() - timeStampDernierCycleVentilation >= duree){
        digitalWrite (fanPin, HIGH);
        //enregistrer le dernier fonctionnement du ventilateur
        timeStampDernierCycleVentilation = millis();
    }
  } 
}

//Affichage sur ECRAN OLED des données selon la phase de culture programmée par les boutons
void affichageOLED(){
  switch (phase) {
  case 1:  
    u8g2.clearBuffer();                  
    u8g2.setFont(u8g2_font_mademoiselle_mel_tr);
    u8g2.drawStr(7,10,"Spore-O-matiC");    
    u8g2.setFont(u8g2_font_7x13_tf);
    u8g2.drawStr(0,25,"Incubation");
    u8g2.drawStr(0,38,"Temp:");
    u8g2.setCursor(40,38);
    u8g2.print(temperature);  
    u8g2.drawStr(0,52,"Hum:");
    u8g2.setCursor(35,52);
    u8g2.print(humidity);
    u8g2.drawStr(0,64,"Total J"); 
    u8g2.setCursor(40,64);
    u8g2.print(nbJoursTotal);
    u8g2.drawStr(50,64,"/Incub J"); 
    u8g2.setCursor(90,64);
    u8g2.print(nbJoursIncub);
    u8g2.sendBuffer();
    break;

  case 2:  
    u8g2.clearBuffer();                  
    u8g2.setFont(u8g2_font_mademoiselle_mel_tr);
    u8g2.drawStr(7,10,"Spore-O-matiC");    
    u8g2.setFont(u8g2_font_7x13_tf);
    u8g2.drawStr(0,25,"Primordia");
    u8g2.drawStr(0,38,"Temp:");
    u8g2.setCursor(40,38);
    u8g2.print(temperature);  
    u8g2.drawStr(0,52,"Hum:");
    u8g2.setCursor(35,52);
    u8g2.print(humidity);
    u8g2.drawStr(0,64,"Total J"); 
    u8g2.setCursor(40,64);
    u8g2.print(nbJoursTotal);
    u8g2.sendBuffer();
    u8g2.drawStr(50,64,"/Prim J"); 
    u8g2.setCursor(90,64);
    u8g2.print(nbJoursPrimordia);
    u8g2.sendBuffer();
    break;

  case 3:  
    u8g2.clearBuffer();                  
    u8g2.setFont(u8g2_font_mademoiselle_mel_tr);
    u8g2.drawStr(7,10,"Spore-O-matiC");    
    u8g2.setFont(u8g2_font_7x13_tf);
    u8g2.drawStr(0,25,"Fructification");
    u8g2.drawStr(0,38,"Temp:");
    u8g2.setCursor(40,38);
    u8g2.print(temperature);  
    u8g2.drawStr(0,52,"Hum:");
    u8g2.setCursor(35,52);
    u8g2.print(humidity);
    u8g2.drawStr(0,64,"Total J"); 
    u8g2.setCursor(40,64);
    u8g2.print(nbJoursTotal);
    u8g2.drawStr(50,64,"/Fructi J"); 
    u8g2.setCursor(90,64);
    u8g2.print(nbJoursFructi);
    u8g2.sendBuffer();
    break;

  /*case 4:  
    u8g2.clearBuffer();                  
    u8g2.setFont(u8g2_font_mademoiselle_mel_tr);
    u8g2.drawStr(7,10,"Spore-O-matiC");    
    u8g2.setFont(u8g2_font_7x13_tf);
    u8g2.drawStr(0,25,"SERVEUR");    
    u8g2.sendBuffer();
    break;

  case 5:  
    u8g2.clearBuffer();                  
    u8g2.setFont(u8g2_font_mademoiselle_mel_tr);
    u8g2.drawStr(7,10,"Spore-O-matiC");    
    u8g2.setFont(u8g2_font_7x13_tf);
    u8g2.drawStr(0,25,"Inter-Recolte");
    
    u8g2.sendBuffer();
    break;
    */

  case 6:  
    u8g2.clearBuffer();                   
    u8g2.setFont(u8g2_font_mademoiselle_mel_tr);
    u8g2.drawStr(7,10,"Spore-O-matiC");    
    u8g2.setFont(u8g2_font_osb35_tf);
    u8g2.drawStr(0,55,"Veille");
    u8g2.sendBuffer();
    break;
  
  default:
    u8g2.clearBuffer();                  
    u8g2.setFont(u8g2_font_mademoiselle_mel_tr);
    u8g2.drawStr(7,10,"Spore-O-matiC");    
    u8g2.setFont(u8g2_font_osb35_tf);
    u8g2.drawStr(0,55,"Erreur Phase");
    u8g2.sendBuffer();
    break;
  }  
}



