// Librairies.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Les fichiers externes.

#include "xc.h"
#include "adc.h"
#include "configuration.h"
#include "profile.h"

#define PI 3.141592

// La régulation.

// Général.

volatile float time_elapsed = 0; // Le temps écoulé au moment où les moteurs s'enclenchent.

float parametre; // C'est pour vérifier si on bien reçu un message de l'UART1.

volatile float Tt; // Cela représente le temps total parcouru. (Travel-Time).

float distance; // Représente la distance total que le robot doit parcourir en translation.

float angle; // Représente l'angle total que le robot doit parcourir en rotation.

int occupied = 0; // Pour dire au Robot si il est occupé avec une tâche déja ou pas du tout.

volatile int type;

// REGULATION.

int sign_position;

int sign_angle;

// Pour la régulation de translation. 

float E = 0.134; // L'empattement.

float R = 0.082; // Rayon des roues. 

volatile float Lref; // La consigne de translation.

float Position_Error; // L'erreur entre la commande réglée et la consigne de translation.

float dc; // Duty Cycle.

float Lmess; // C'est la commande réglée de translation.

float kp_l = 3.69; // C'est le gain du régulateur proportionnel de la boucle de translation.

// Pour la régulation angulaire.

float Aref; // La consigne angulaire. Elle est exprimée en degrée. 

float Amess; // La commande réglée angulaire.

float Angular_Error; // L'erreur entre la commande réglée et la consigne angulaire.

float kp_r = 0.0043149775; // C'est le gain du régulateur proportionnel de la boucle angulaire. !!!!!!!!!!!!!!!!!!!!!!!!!!!

// ------------------------------

// Les capteurs. 

int dir1; // C'est pour indiquer la direction des roues. 0 est considéré comme en avant. MOTEUR DE GAUCHE.

int dir2; // Egalement. Le sens trigonométrique est le sens positif. 1 est considéré comme en arrière. MOTEUR DE DROITE.

volatile float Pos_Inst = 0;

volatile float Ang_Inst = 0;

// Les moteurs. 

float a = 0.5; // L'accélération. Limité à 0.5 pour que les mouvements ne soient pas trop brusque.

float a_ang = 0.5/(2*0.134); // L'accélération angulaire.

float v = 0.5; // La vitesse nominale du robot.

int signe;

int swap_moteur1;

int swap_moteur2;


int main(void){

    // INITIALISATION. 

    // dsPIC.

    frcPll40MHzConfig();
    TRISBbits.TRISB12 = 0;  // Configure RB15 as a digital output pin. (Nos directions)
    TRISBbits.TRISB11 = 0;  // Configure RB14 as a digital output pin.
    
        /* Configuration du Peripheral Pin Select (PPS) pour connecter le signal
     * Rx de l'UART1 ? RB6/RP6 et le signal Tx ? RB7/RP7 */
	_U1RXR = 6;    // U1RX -> RP6
	_RP7R = 3;     // RP7 -> U1Tx

    T1CONbits.TON = 0; // Disable Timer
    T1CONbits.TCS = 0; // Select internal instruction cycle clock
    T1CONbits.TGATE = 0; // Disable Gated Timer mode
    T1CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
    TMR1 = 0x00; // Clear timer register
    PR1 = 39999; // Load the period value.
    _T1IE = 1; // Enable Timer1 interrupt.


    // -------------------------------

    // l'UART1.

   // Configuration de l'UART1 avec un format de trame 8N1, ? s
    U1MODEbits.PDSEL = 0;       // 8 bits, pas de parit? 57600 bits/
    U1MODEbits.STSEL = 0;       // 1 stop bit
    /* L'UART peut fonctionner ? 2 vitesses diff?rentes. Le mode "high speed" 
     * est plus sensible au bruit et ne doit donc ?tre utilis? que pour
     * des d?bits trop rapides pour le mode standard 
     * En mode standard, le d?bit est donn? par :
     * baud rate = FCY / (16*(U1BRG+1) 
     * => U1BRG = (3.685MHz / (16*57.6kHz)) - 1  =  2.998*/
    U1MODEbits.BRGH = 0; // 1 (4x baud clock, High-speed mode)
    U1BRG = 42; // On jongle entre 3 et 49
    
    U1MODEbits.UARTEN = 1;      // on active l'UART
    U1STAbits.UTXEN = 1;        // on active l'?mission

    // Deplacement. 

    setup();
    
    // -------------------------------

    // TRAITEMENT.

    while (1){ // On va vérifier en premier si l'UART1 a reçu un octet avec lequel on peut travailler. Code reprit de l'OSCILLOSCOPE.
        if(U1STAbits.URXDA){
            type = U1RXREG;
            //printf("type");
            if ((0 <= type) && (type < 4))
            {
                while (!U1STAbits.URXDA) {} // On attend que l'UART soit à nouveau disponible.
                parametre = U1RXREG;
                occupied = 1;
            }
        }

    
    // Les 4 conditions : Avancer, Reculer, Tourner à gauche, Tourner à droite.

    // La translation. ----------------------------
    
        if(occupied == 1){
            // Le robot est en train d'effectuer une tâche.
            if (type == 0){ // Avancer.
            
            distance = fabs(parametre/100);  
            angle = 0; // On veut que le robot aille tout droit et ne dévie pas.
            Tt =  Travel_Time(distance,a,v,type); // Obtient le temps du trajet théorique.

            signe = 1;
            swap_moteur1 = 1;
            swap_moteur2 = 1;
            swap(swap_moteur1,swap_moteur2);

            T1CONbits.TON = 1; // Start Timer1.
            }
            else if (type == 1){ // Reculer.

            distance = fabs(parametre/100);
            angle = 0;
            Tt =  Travel_Time(distance,a,v,type);
            
            signe = 1;
            swap_moteur1 = -1;
            swap_moteur2 = -1;
            swap(swap_moteur1,swap_moteur2);

            T1CONbits.TON = 1; // Start Timer1.
            } 

            // La rotation. -------------------------------
            
            else if (type == 2){ // Tourner à droite.

            angle = parametre;
            distance = 0;
            Tt = Travel_Time(angle,a_ang,v,type);

            signe = 1;
            swap_moteur1 = 1;
            swap_moteur2 = -1;
            swap(swap_moteur1,swap_moteur2);

            T1CONbits.TON = 1; // Start Timer1.
            }
            
            else if (type == 3){ // Tourner à gauche.

            angle = parametre;
            distance = 0;
            Tt = Travel_Time(angle,a_ang,v,type);

            signe = 1;
            swap_moteur1 = -1;
            swap_moteur2 = 1;
            swap(0,0);

            T1CONbits.TON = 1; // Start Timer1.
            }
        }
    }
}

// Code repris du Github sur les Timers.

/* Timer1 ISR
 * Called automatically when T1IF is set.
 * NEVER CALL AN ISR ANYWHERE IN THE CODE
 */
void __attribute__((interrupt, auto_psv))_T1Interrupt(void) { // Ici se situe notre boucle de régulation.
    // ISR code does the same things that the main loop did in polling
    _T1IF = 0; // Rabaisse le Flag (image boite aux lettres).
    // _LATB15 = !_LATB15; Provient de l'exemple sur GitHub.

    // BOUCLE DE REGULATION.

    // Il faut que la grandeur de commande augment par petit pas.

    time_elapsed = time_elapsed + 0.001;
    Lref = signe*profile_position(a,v,Tt,time_elapsed,distance);
    Aref = signe*profile_angulaire(a_ang,Tt,time_elapsed,(angle/180)*PI); // On calcule tout en radian.

    // Les informations des capteurs. 

    Pos_Inst = (2*PI/1440)*((swap_moteur1*(int)POS1CNT) + (swap_moteur2*(int)POS2CNT))*(R/2); // Entrer le code reçu des capteurs.      
    Ang_Inst = (2*PI/1440)*((swap_moteur2*(int)POS2CNT) - (swap_moteur1*(int)POS1CNT))*(R/E); // Entrer le code reçu des capteurs.

    // Les erreurs. 

    Position_Error = Lref - Pos_Inst;
    Angular_Error = Aref - Ang_Inst;

    // Modifier ensuite le OCR. 

    float OC1 = kp_l*Position_Error - kp_r*Angular_Error;  // moteur gauche
    float OC2 = kp_r*Angular_Error + kp_l*Position_Error; // moteur droit 
    
    if((OC1 > 0) && (OC2 > 0)){ // Avance
        LATBbits.LATB12 = 0; // Moteur de gauche (le moteur 1). c'est les dir 1 et 2
        LATBbits.LATB11 = 0; // Moteur de droit (le moteur 2).
    }
    else if((OC1 < 0) && (OC2 < 0)){ // Recule
        LATBbits.LATB12 = 1; // Moteur de gauche (le moteur 1)
        LATBbits.LATB11 = 1; // Moteur de droit (le moteur 2)
    }
    else if((OC1 > 0) && (OC2 < 0)){ // Droite
        LATBbits.LATB12 = 0; // Moteur de gauche (le moteur 1)
        LATBbits.LATB11 = 1; // Moteur de droit (le moteur 2)
    }
    else if((OC1 < 0) && (OC2 > 0)){ // Gauche
        LATBbits.LATB12 = 1; // Moteur de gauche (le moteur 1)
        LATBbits.LATB11 = 0; // Moteur de droit (le moteur 2)
   }
    
    OC1RS = fabs(OC1)*PR2;
    OC2RS = fabs(OC2)*PR3;

    // Pour éviter le paradoxe d'Achille et de la tortue (Zénon d'Elée). On considère le point d'arrivée comme la condition suivante.

    if ((time_elapsed > Tt) && (Position_Error < 0.01) && (Angular_Error < 0.01))
    {
        // Eteindre les Moteurs.
        TRISBbits.TRISB8= 0 ;
        _LATB8=1;
        OC1RS = 0;
        OC2RS = 0;

        // Eteindre le Timer.
 
        T1CONbits.TON = 0;
        TMR1 = 0;

        // Reinitialiser les variables.
        
        occupied = 0;
        parametre = 0;
        distance = 0;
        a_ang = 0;
        Tt = 0;
        time_elapsed = 0;
        dir1 = 0;
        dir2 = 0;
        type = 0;
        Lref = 0;
        Aref = 0;
        Pos_Inst = 0;
        Ang_Inst = 0;
        Position_Error = 0;
        Angular_Error = 0;

        LATBbits.LATB15 = 0;    
        LATBbits.LATB14 = 0;
    }
}