//**************************************************************************************************************************************************************************
//***************************** Détection et Mesure de la largeur des impulsions d'un signal - Détection et Mesure v2.0 - Carte Nano (com64) *******************************
//**************************************************************************************************************************************************************************
// Auteur : Serge Ducatez 03/2022
//**************************************************************************************************************************************************************************
// https://www.youtube.com/channel/UCyGEFYW18IZKpe4uPgp9b8g/videos
// https://www.facebook.com/profile.php?id=100077558149994
//**************************************************************************************************************************************************************************
// Détection et Mesure :
// Carte Nano.
// La broche D2 est l'entrée du signal à détecter et à mesurer.
// Isolation par optocoupleur 4N35 entre la sortie du signal de la carte Uno (Générateur du signal) et la broche D2 :
// => Inversion de l'impulsion du signal par l'optocoupleur 4N35.
// Détection et du calcul de la durée de l'impulsion du signal :
// Interruption externe INT0.
// Utilisation des registres pour configurer l'interruption externe INT0.
//**************************************************************************************************************************************************************************
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> // Paramètres modifiables
//**************************************************************************************************************************************************************************
#include <avr/pgmspace.h>                                                                                                       // Librairie pour la gestion de la mémoire flash

const int Broche_EntreeSignal =  2;                                                                                             // Broche de l'entrée du signal (D2) (PD2)
const int Broche_LEDVerteTemoin_PresenceSignal = 13;                                                                            // Broche de la LED verte témoin de la présence du signal d'entrée (D13) (PB5)

volatile bool INT0_Front_Descendant = true;                                                                                     // Indicateur de déclenchement de l'interruption externe INT0 sur front montant ou descendant

volatile bool Affichage_DureeSignal = false;                                                                                    // Indicateur d'autorisation d'affichage de la durée de l'impulsion du signal d'entrée

volatile unsigned long DureeImpulsionSignal = 0ul;                                                                              // Durée de l'impulsion du signal d'entrée
volatile unsigned long DateDebutImpulsionSignal = 0ul;                                                                          // Date courante du début de l'impulsion du signal d'entrée

unsigned long DateDebutFlash_LEDVerteTemoin_PresenceSignal = 0ul;                                                               // Date courante du début du flash de la LED verte témoin de la présence du signal d'entrée
unsigned long DureeFlash_LEDVerteTemoin_PresenceSignal = 25ul; // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> // Durée du flash de la LED verte témoin de la présence du signal d'entrée

//**************************************************************************************************************************************************************************
//**************************************************************************************************************************************************************************
void setup()
{
  Serial.begin(9600);                                                                                                           // Démarre la voie série pour la communication avec la console
  
  pinMode(Broche_LEDVerteTemoin_PresenceSignal, OUTPUT);                                                                        // Configure la broche en sortie
  
  pinMode(Broche_EntreeSignal, INPUT);                                                                                          // Configure la broche en entrée
  
  EICRA &= ~(bit(ISC00) | bit(ISC01));                                                                                          // Positionne à "0" les bits "ISC00" et "ISC01" du registre "EICRA" (External Interrupt Control Register A)
  EICRA |= bit(ISC01);                                                                                                          // Positionne à "1" le bit "ISC01" du registre "EICRA" pour déclencher l'interruption externe INT0 sur front descendant
  EIFR = bit(INTF0);                                                                                                            // Positionne à "1" le bit "INTF0" (External Interrupt Flag Register) pour effacer une interruption en suspens
  EIMSK |= bit(INT0);                                                                                                           // Positionne à "1" le bit "INT0" du registre "EIMSK" (External Interrupt Mask Register) pour activer le déclenchement de l'interruption externe sur la broche "D2"
  
  delay(1000);                                                                                                                  // Temporise 1 seconde

//**************************************************************************************************************************************************************************
//**************************************************************************************************************************************************************************
}

void loop()
{
//**************************************************************************************************************************************************************************
  if (Affichage_DureeSignal) // *********************************************************************************************** // Si l'indicateur d'autorisation d'affichage de la durée de l'impulsion du signal d'entrée est activé
  {
    Serial.println(DureeImpulsionSignal); // Débug                                                                              // Affiche la durée de l'impulsion du signal d'entrée à l'état bas en microsecondes (Inverseur optocoupleur 4N35)
    
    Affichage_DureeSignal = false;                                                                                              // Désactive l'indicateur d'autorisation d'affichage de la durée de l'impulsion du signal d'entrée
    DureeImpulsionSignal = 0ul;                                                                                                 // Réinitialise la durée de l'impulsion du signal d'entrée
    PORTB |= 0b00100000;                                                                                                        // Allume la LED verte témoin de la présence du signal d'entrée
    DateDebutFlash_LEDVerteTemoin_PresenceSignal = millis();                                                                    // Mémorise la date courante du début du flash de la LED verte témoin de la présence du signal d'entrée
  }

//**************************************************************************************************************************************************************************
  // ************************************************************************************************************************** // Si la durée du flash de la LED verte témoin de la présence du signal d'entrée est écoulée et Si elle a déjà démmarré
  if (millis() - DateDebutFlash_LEDVerteTemoin_PresenceSignal >= DureeFlash_LEDVerteTemoin_PresenceSignal && DateDebutFlash_LEDVerteTemoin_PresenceSignal != 0)
  {
    PORTB &= 0b11011111;                                                                                                        // Eteint la LED verte témoin de la présence du signal d'entrée
    DateDebutFlash_LEDVerteTemoin_PresenceSignal = 0;                                                                           // Réinitialise la date courante du début du flash de la LED verte témoin de la présence du signal d'entrée
  }

//**************************************************************************************************************************************************************************
//**************************************************************************************************************************************************************************
}

//**************************************************************************************************************************************************************************
//*** Fonction de l'interruption externe INT0 exécutée sur front montant ou descendant sur la broche D2 ********************************************************************
//**************************************************************************************************************************************************************************
ISR (INT0_vect)
{
  if (INT0_Front_Descendant) // *********************************************************************************************** // Si l'interruption externe INT0 est déclenchée sur front descendant
  {
    EICRA &= ~(bit(ISC00) | bit(ISC01));                                                                                        // Positionne à "0" les bits "ISC00" et "ISC01" du registre "EICRA" (External Interrupt Control Register A)
    EICRA |=  bit(ISC00);                                                                                                       // Positionne à "1" le bit "ISC00" du registre "EICRA" pour déclencher l'interruption externe INT0 sur front montant
    
    INT0_Front_Descendant = false;                                                                                              // Définit l'indicateur de déclenchement de la prochaine interruption externe INT0 sur front montant
    DateDebutImpulsionSignal = micros();                                                                                        // Mémorise la date courante du début de l'impulsion du signal d'entrée
  }
  else if (!INT0_Front_Descendant) // ***************************************************************************************** // Si l'interruption externe INT0 est déclenchée par un front montant
  {
    EICRA &= ~(bit(ISC00) | bit(ISC01));                                                                                        // Positionne à "0" les bits "ISC00" et "ISC01" du registre "EICRA" (External Interrupt Control Register A)
    EICRA |=  bit(ISC01);                                                                                                       // Positionne à "1" le bit "ISC01" du registre "EICRA" pour déclencher l'interruption externe INT0 sur front descendant
    
    INT0_Front_Descendant = true;                                                                                               // Définit l'indicateur de déclenchement de la prochaine interruption externe INT0 sur front descendant
    DureeImpulsionSignal = micros() - DateDebutImpulsionSignal;                                                                 // Calcule la durée de l'impulsion du signal d'entrée
    Affichage_DureeSignal = true;                                                                                               // Active l'indicateur d'autorisation d'affichage de la durée de l'impulsion du signal d'entrée
  }
}
