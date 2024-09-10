unsigned long timeStampDernierCycleVentilation=0;
int fanPin = 4;

long tempsVentilation = 45000; //durée de la ventilation = 45 secondes

//Variable qui sera modifiée par le changement de phase de culture
int nombreEchangeAirCible = 60;
long duree = 0; //durée entre 2cycles de ventilation


void setup() {
  pinMode(fanPin, OUTPUT);  
  Serial.begin(9600);
}

void loop() {
 
  calculIntervalleVentilation(); //par la suite le calcul de la variable se fera uniquement lors des changements de phases
  Serial.println(duree);
  gestionVentilo();  
 
  delay(2000);
}


//Fonctions
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

int calculIntervalleVentilation(){
//temps entre chaque début de ventilation
//On divise 1 heure par le nombre d'echanges
	duree = 60 / nombreEchangeAirCible;
//On multiplie par 60 000 pour obtenir des millisecondes
	duree *= 60000;	
}

