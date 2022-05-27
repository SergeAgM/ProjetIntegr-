
#include <stdio.h>
#include <stdlib.h>

#include "xc.h"
#include "adc.h"
#include "filtre.h"

#define treshold 100 // A vérifier.
#define Echantillions 15


int main(void) {

    // INITIALISATION.

    // Variables.

    unsigned i;
    int16_t sample;
    
    int32_t outputFilter1; // Contient les anciennes sorties filtrées.
    int32_t outputFilter0; // Contient les anciennes sorties filtrées.
    
    int detLow = 0; // 
    int detHigh = 0; // 
    
    int32_t maximum1100 = 0;
    int32_t maximum900 = 0;

    int16_t Counter = 0;
    int16_t message; // C'est pour obtenir les 10 de données de la trame si un trame correcte a été identifiée. 

    // dsPic. 

    frcPll40MHzConfig(); // Fait monter la fréquence de travail à 40Mhz.
    
    // Configuration de L'ADC pour utilisation en polling sur AN0
    adcInit(ADC_TIMER3_SAMPLING);
    TMR3 = 0;
    T3CONbits.TON = 1; // on démarre le timer3
    
    // !!
    PR3 = 2665; // T= 1/15000 = (PR1+1)/40MHz => PR1+1 = 2666. Celle d'avant : T= 200?s = (PR1+1)/3.685MHz => PR1+1 = 737.
    _TRISB15 = 0; // Activation de la sortie 15.

    // Uart.

    /* Configuration du Peripheral Pin Select (PPS) pour connecter le signal
     * Rx de l'UART1 ? RB6/RP6 et le signal Tx ? RB7/RP7 */
	_U1RXR = 6;    // U1RX -> RP6
	_RP7R = 3;     // RP7 -> U1Tx

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

    // TRAITEMENT.

	while(1) {  
        while (!adcConversionDone()) {}  // en "pollant" l'ADC
        sample = adcRead();           // on ram?ne le résultat sur 8 bits

        // Sortie des filtres.

        outputFilter1 = filtreBP_delta_plus(sample); // Filtrer à 1100Hz.
        outputFilter0 = filtreBP_delta_moins(sample); // Filter à 900Hz.

        // Calcul des maximums.

        if(abs(outputFilter1) > treshold){
            maximum1100 = abs(outputFilter1);
        } else if(abs(outputFilter0) > treshold){
            maximum900 = abs(outputFilter0);
        }

        // Comparateur.

        Counter = Counter + 1;

        if(Counter >= Echantillions){
            if(maximum1100 > treshold){
                detHigh = 1;
            }

            else{
                detHigh = 0;
            }

            if(maximum900 > treshold){
                detLow = 1;
            }

            else{
                detLow = 0;
            }
            message = fskDetector(detLow, detHigh);
            if (message != 0){
                int16_t message2bits = (message & 0xFF00)>>8;
                int16_t message8bits = message & 0x00FF;
                printf("Message");
                while (U1STAbits.UTXBF){} // On attend que ça soit libre.
                U1TXREG = message2bits;
                while (U1STAbits.UTXBF){} // On attend à nouveau que ça soit libre.
                U1TXREG = message8bits;
            }
            Counter = 0;
            maximum1100 = 0;
            maximum900 = 0;
            // Ecrire un petit code qui permet d'allumer et éteindre la LED. 
            // Pour dire qu'on a envoyé de l'information et la réeteindre par la suite.
        }

    }   
    while (U1STAbits.UTXBF) {}      // On voit si l'UART est disponible.
    return 0;
}