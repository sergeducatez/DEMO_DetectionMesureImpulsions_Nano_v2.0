# DEMO_DetectionMesureImpulsions_Nano_v2.0
// Détection et Mesure :
// Carte Nano.
// La broche D2 est l'entrée du signal à détecter et à mesurer.
// Isolation par optocoupleur 4N35 entre la sortie du signal de la carte Uno (Générateur du signal) et la broche D2 :
// => Inversion de l'impulsion du signal par l'optocoupleur 4N35.
// Détection et du calcul de la durée de l'impulsion du signal :
// Interruption externe INT0.
// Utilisation des registres pour configurer l'interruption externe INT0.
