#include <stdio.h>
#include <stdint.h>

#include <math.h>
#define PI 3.141592

float Travel_Time(float distance, float a, float v, int type){
    // Fonction qui renvoie le temps théorique nécessaire pour parcourir la tâche demandée.

    float Tt = 0; // Le temps total nécessaire à parcourir.

    if (type < 2)
    {
        if (distance > 0.5) // Pour qu'on atteigne la vitesse nominale.
    {
        Tt = 2 + (distance - 0.5)/v; // Voir les calculs sur Matlab + notes dans GitHub.
    }
    else if (distance <= 0.5) // Si on arrive pas à atteindre la vitesse nominale. 
    {
        Tt = 2*sqrt(distance/a); // Voir les calculs sur Matlab + notes dans GitHub.
    }
    }
    else if (type >= 2)
    {
        Tt = 2*sqrt((distance/180)*PI/(a));
    }
    
    return Tt;
}

float profile_position(float a, float v, float Tt, float time_elapsed, float distance){ // Cette fonction retourne la position instantanée.
    // Fonction copiée et développée sur Matlab pour obtenir les graphiques.
    // Le profil de vitesse est trapézoïdal. Cependant pour une certaine distance critique parcourue,
    // le profil de vitesse devient alors triangulaire. Cette distance critique est de 0.5. (Voir Matlab)
    // On sépare l'accélération, de la vitesse nominale et de la déscélération.
    
    // Initialisation.

    float position = 0; // Notre position instantanée. 
    
    float T_nom = v/a; // Le temps qu'il faut au vehicule pour atteindre sa vitesse nominale.

    // Traitement.

    if((distance != 0) && (distance  > 0.5)){ // Ici la distance est plus élevée et le robot atteint sa vitesse nominal. (profil trapèze)
        if (time_elapsed <= T_nom){
            position = a*(pow(time_elapsed,2))/2;
        }
        else if ((T_nom < time_elapsed) && (time_elapsed < (Tt-T_nom))){
            position = a*(pow(T_nom,2))/2 + v*(time_elapsed - T_nom);
        }
        else if (((Tt-T_nom) <= time_elapsed)  && (time_elapsed <= Tt)){
            position = a*(pow(T_nom,2))/2 + v*((Tt - T_nom) - T_nom) + v*(time_elapsed - (Tt - 1)) - a*(pow((time_elapsed - (Tt - T_nom)),2))/2;
        }
    }

    else if ((distance != 0) && (distance  <= 0.5))
    {
        if (time_elapsed <= Tt/2){
            position = a*(pow(time_elapsed,2))/2;
        }
        else if (time_elapsed > Tt/2)
        {
            position = -a*(pow(time_elapsed,2))/2 + a*Tt*time_elapsed - 1/4*a*(pow(Tt,2));
        }
        
    }

    return position;
}

float profile_angulaire(float a, float Tt, float time_elapsed, float angle){ // Cette fonction retourne la déviation angulaire instantanée.
    // Fonction copiée et développée sur Matlab pour obtenir les graphiques.

    // Initialisation.
    
    float theta = a*(pow(time_elapsed,2))/2;

    // Traitement.

    if (angle != 0)
    {
        if (theta <= angle/2)
        {
            return theta;
        }
        else if (theta > angle/2)
        {
            theta = -a*(pow(time_elapsed,2))/2 + a*Tt*time_elapsed -a*(pow(Tt,2))/4;
        } 
        return theta;
    }
    else {
    
    return 0;
    }
    
}