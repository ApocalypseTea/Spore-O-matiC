#include <DHT.h>

// définition capteur DHT et pin
#define DHTTYPE DHT22
#define DHTPIN 2

// Initialisation capteur
DHT dht(DHTPIN, DHTTYPE);

//définition pins des relais 
int heatPin = 4;
int humidityPin = 8;


//définition de la temperature cible/haute/moyenne
float tempCibleHigh=28;
float tempCibleLow=24;
float tempCibleMoy=26;

void setup() {
  pinMode(heatPin, OUTPUT);
  pinMode(humidityPin, OUTPUT);
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // gestion erreur de lecture
  if(isnan(humidity) || isnan(temperature)){
    Serial.println("Erreur de lecture du capteur DHT");
    return;
  }

  Serial.print("Temp:");
  Serial.println(temperature);

  tempManagement(temperature);

  delay(3000);
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
