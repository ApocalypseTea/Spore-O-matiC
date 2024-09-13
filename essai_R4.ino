#include "RTC.h"
//Variables de stockage des dates et d'incrementation de jours par phase
RTCTime beginningTime;
int nbJoursTotal;
int nbJoursIncub;
int nbJoursPrimordia;
int nbJoursFructi;
int nbJoursInterRecolte;

//Periode de culture en cours
int phase=1;

void setup() {
  Serial.begin(9600);
  RTC.begin();
  
  RTCTime startTime(12, Month::SEPTEMBER, 2024, 10, 38, 00, DayOfWeek::THURSDAY, SaveLight::SAVING_TIME_ACTIVE);

  RTC.setTime(startTime);
  beginningTime = startTime;

}

void loop() {
  RTCTime currentTime;

  // Get current time from RTC
  RTC.getTime(currentTime);

  // Print time (HH/MM/SS)
  Serial.print(currentTime.getHour());
  Serial.print(":");
  Serial.print(currentTime.getMinutes());
  Serial.print(":");
  Serial.println(currentTime.getSeconds());  

  Serial.println(beginningTime.getSeconds());
  nbJoursCulture(currentTime);  

  delay(1000);
}

int nbJoursCulture(RTCTime currentTime){
  //Test réalisé en secondes mais à passer en heures .getHour() pour la fonction finale
  int result=currentTime.getSeconds()-beginningTime.getSeconds();
  Serial.print("La culture dure depuis ");
  Serial.print(result);
  Serial.println(" secondes");
  //A chaque tour, on ajoute une journée (passer la condition à 23 pour la fonction finale)
  if (result == 59) {
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
        Serial.print(" jours.");
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







