# Spore-O-matiC
Projet Arduino d'automatisation de culture de champignons

# Materiel utilisé
## Partie Automatisation (au 09/09/24)
- Arduino UNO R4 WiFi + Base Shield v2.1 (Seeed Studio)
- Temperature & Humidity Sensor pro v1.3 Grove (Seeed Studio)
- Grove Relay (Seeed Studio)
- ~~Grove 6 positions DIP Switch (Seeed Studio)~~ -> LIBRAIRIE NON COMPATIBLE avec Uno R4
- Grove OLED Yellow&Blue Display 0.96 (Seeed Studio)
## Partie Culture
- Chambre de culture 120x60x180cm
- Ventilateur Noctua 5V
- Tapis de culture chauffants

# Fonctions d'automatisation attendues
## v1 - Version en cours (09/24)
- Maintien de la température selon capteur
- Maintien de l'hygrometrie selon capteur
- Gestion de la ventilation sur un timing precis
- Modification de la phase de culture et ses parametres par switch physique
- Affichage des mesures environnementales et des données de culture en cours
## v2
- Serveur HTTP pour pilotage via WiFi
- WatchDog en cas de pb de fonctionnement
- HeartBeat en cas de coupure de courant avec un 2eme arduino
## v3
- Suivi Camera pour timelapse de photo de croissance
- Capteur de CO2 avec adaptation de la ventilation
- Gestion de la luminosité (capteur + relai)
- Gestion de niveau d'eau dans l'humidificateur avec alerte

