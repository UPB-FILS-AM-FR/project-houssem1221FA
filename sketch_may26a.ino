#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <NewPing.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo barriere;

// --- CONFIGURATION DES BROCHES (PINS) ---
const int brocheServo = 9;

// Capteur Entrée (Barrière)
#define TRIG_ENTREE 12
#define ECHO_ENTREE 13

// Capteur Place 1
#define TRIG_P1 2
#define ECHO_P1 3
const int ledVerteP1 = 4;
const int ledRougeP1 = 8; // Changé à 8 pour libérer la broche 5

// Capteur Place 2
#define TRIG_P2 5
#define ECHO_P2 6
const int ledVerteP2 = 7;
const int ledRougeP2 = 10;

// Configuration de NewPing (Distance max 100 cm)
NewPing sonarEntree(TRIG_ENTREE, ECHO_ENTREE, 100);
NewPing sonarP1(TRIG_P1, ECHO_P1, 100);
NewPing sonarP2(TRIG_P2, ECHO_P2, 100);

bool voitureEnAttente = false;
int anciennesPlacesLibres = -1;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Smart Parking ");

  barriere.attach(brocheServo);
  barriere.write(0); // Barrière fermée au départ

  pinMode(ledVerteP1, OUTPUT);
  pinMode(ledRougeP1, OUTPUT);
  pinMode(ledVerteP2, OUTPUT);
  pinMode(ledRougeP2, OUTPUT);
}

void loop() {
  int placesLibres = 0;

  // Lecture des distances en centimètres
  int d1 = sonarP1.ping_cm();
  int d2 = sonarP2.ping_cm();
  int distEntree = sonarEntree.ping_cm();

  // --- LOGIQUE PLACE 1 (Seuil à 10 cm) ---
  if (d1 > 0 && d1 < 10) { // Si distance entre 1 et 10cm -> Voiture présente
    digitalWrite(ledVerteP1, LOW);
    digitalWrite(ledRougeP1, HIGH);
  } else {                 // Place libre
    digitalWrite(ledVerteP1, HIGH);
    digitalWrite(ledRougeP1, LOW);
    placesLibres++;
  }

  // --- LOGIQUE PLACE 2 (Seuil à 10 cm) ---
  if (d2 > 0 && d2 < 10) { // Si distance entre 1 et 10cm -> Voiture présente
    digitalWrite(ledVerteP2, LOW);
    digitalWrite(ledRougeP2, HIGH);
  } else {                 // Place libre
    digitalWrite(ledVerteP2, HIGH);
    digitalWrite(ledRougeP2, LOW);
    placesLibres++;
  }

// --- MISE À JOUR ÉCRAN LCD ---
  if (placesLibres != anciennesPlacesLibres) {
    lcd.setCursor(0, 1); // Se positionner sur la deuxième ligne
    
    // Étape cruciale : on efface d'abord TOUTE la ligne du bas avec 16 espaces
    lcd.print("                "); 
    
    lcd.setCursor(0, 1); // On se re-positionne au début de la ligne vide
    
    if (placesLibres > 0) {
      lcd.print("Places vides: ");
      lcd.print(placesLibres); // Affiche le chiffre (ex: 1 ou 2)
    } else {
      lcd.print("Parking COMPLET!");
    }
    anciennesPlacesLibres = placesLibres;
  }
  // --- GESTION DU SERVOMOTEUR (BARRIÈRE) ---
  if (distEntree > 0 && distEntree < 10 && !voitureEnAttente) {
    voitureEnAttente = true;
    if (placesLibres > 0) {
      lcd.clear();
      lcd.print("Welcome! Entering");
      barriere.write(90); // Ouvre la barrière
      delay(3000);        // Attend 3 secondes
      barriere.write(0);  // Referme la barrière
      lcd.clear();
      lcd.print("  Smart Parking ");
      anciennesPlacesLibres = -1; // Force la mise à jour de l'affichage
    }
  }

  if (distEntree == 0 || distEntree >= 10) {
    voitureEnAttente = false;
  }

  delay(200);
}