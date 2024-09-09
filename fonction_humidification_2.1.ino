#include <DHT.h> 

// définition capteur DHT et pin
#define DHTTYPE DHT22
#define DHTPIN 2

  
//Création Objet Capteur
DHT dht(DHTPIN, DHTTYPE); 

//Attribution pins des relais
int heatPin = 4;
int humidityPin = 8;   

//définition de l'humidité cible/haute/moyenne
float humiCibleHigh=28;
float humiCibleLow=24;
float humiCibleMoy=26;

void setup() {
 pinMode(heatPin, OUTPUT);
 pinMode(humidityPin, OUTPUT);
 Serial.begin(9600);
 dht.begin();
}
 

void loop() {
// Recupération des données capteur
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

// gestion erreur de lecture
  if(isnan(humidity) || isnan(temperature)){
    Serial.println("Erreur de lecture du capteur DHT");
    return;
  }

//Affichage des données capteur
  Serial.print("Hum:");
  Serial.println(humidity);

  humiManagement(humidity);

  delay(3000);
}
  
  
//Fonction Allumage de l'humidificateur
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