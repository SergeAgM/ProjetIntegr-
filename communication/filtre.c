#include <stdio.h>
#include <stdint.h>
#include "filtre.h"

// Traitement des filtres numériques (Band-Pass filter).

int32_t filtreBP_delta_plus(int16_t newSample) // Celui à 1100Hz.
{
    // Implémentation pour un filtre d'ordre p = 2.
    // Ici m = 2 également.
    // On prend delta+

    // Méthode: pour les termes non-causal (t<0)
    // On décide d'initialiser à 0.

    // Initialisation. ----------------------------------------------------

    // 1. Initialisation des coeffs.

    // Pour le filtre 1

    static int32_t a_1[] = {1*D,-1.77495574*D,0.98471132*D};
    static int32_t b_1[] = {1*D,2*D,1*D};
    static int32_t g_1 = 0.003035*D;
    //printf("la valeur de g_1 vaut : %d\n", b_1[1]);

    // Pour le filtre 2

    static int32_t a_2[] = {1*D,-1.7807248*D,0.98490396*D};
    static int32_t b_2[] = {1*D,2*D,1*D};
    static int32_t g_2 = 0.002955*D;

    // Pour le filtre 3

    static int32_t a_3[] = {1*D,-1.77882789*D,0.99358215*D};
    static int32_t b_3[] = {1*D,-2*D,1*D};
    static int32_t g_3 = 0.02263*D;

    // Pour le filtre 4

    static int32_t a_4[] = {1*D,-1.79257281*D,0.99377646*D};
    static int32_t b_4[] = {1*D,-2*D,1*D};
    static int32_t g_4 = 0.02271*D;

    // 2. Initialisation pour la récurrence.

    static int32_t Sample1[] = {0,0,0}; // Filtre 1
    static int32_t Output1[] = {0,0,0};

    static int32_t Sample2[] = {0,0,0}; // Filtre 2
    static int32_t Output2[] = {0,0,0};

    static int32_t Sample3[] = {0,0,0}; // Filtre 3
    static int32_t Output3[] = {0,0,0};

    static int32_t Sample4[] = {0,0,0}; // Filtre 4
    static int32_t Output4[] = {0,0,0};

    Sample1[2] = (int32_t) newSample;
    int32_t y;

    // Traitement. -----------------------------------------------

    // Pour le filtre 1 -------------

    Output1[2] = (b_1[0]*Sample1[2] + b_1[1]*Sample1[1] + b_1[2]*Sample1[0] - (a_1[1]*Output1[1] + a_1[2]*Output1[0]))>>M;

    // Décaler d'un rang les sorties. On décale de la droite vers la gauche.

    Output1[0] = Output1[1];
    Output1[1] = Output1[2]; // Sorties.


    // Décaler d'un rang les entrées.

    Sample1[0] = Sample1[1];
    Sample1[1] = Sample1[2]; // Entrées.

    Sample2[2] = (Output1[2]*g_1)>>M; // Nouvelle entrée pour notre prochain étage.
    //printf("Valeur du sample : %f/n", Output1[2]*g_1);
    // Pour le filtre 2. ------------

    Output2[2] = (b_2[0]*Sample2[2] + b_2[1]*Sample2[1] + b_2[2]*Sample2[0] - (a_2[1]*Output2[1] + a_2[2]*Output2[0]))>>M;

    // Décaler d'un rang les entrées.

    Output2[0] = Output2[1];
    Output2[1] = Output2[2];

    // Décaler d'un rang les sorties.

    Sample2[0] = Sample2[1];
    Sample2[1] = Sample2[2];

    Sample3[2] = (Output2[2]*g_2)>>M;

    // Pour le filtre 3. -------------

    Output3[2] = (b_3[0]*Sample3[2] + b_3[1]*Sample3[1] + b_3[2]*Sample3[0] - (a_3[1]*Output3[1] + a_3[2]*Output3[0]))>>M;

    // Décaler d'un rang les entrées.

    Output3[0] = Output3[1];
    Output3[1] = Output3[2];

    // Décaler d'un rang les sorties.

    Sample3[0] = Sample3[1];
    Sample3[1] = Sample3[2];

    Sample4[2] = (Output3[2]*g_3)>>M;

    // Pour le filtre 4. -------------

    Output4[2] = (b_4[0]*Sample4[2] + b_4[1]*Sample4[1] + b_4[2]*Sample4[0] - (a_4[1]*Output4[1] + a_4[2]*Output4[0]))>>M;

    // Décaler d'un rang les entrées.

    Output4[0] = Output4[1];
    Output4[1] = Output4[2];

    // Décaler d'un rang les sorties.

    Sample4[0] = Sample4[1];
    Sample4[1] = Sample4[2];

    y = (Output4[2]*g_4)>>M;

    return y;
}

int32_t filtreBP_delta_moins(int16_t newSample) // Celui à 1100Hz.
{
    // Implémentation pour un filtre d'ordre p = 2.
    // Ici m = 2 également.
    // On prend delta+

    // Méthode: pour les termes non-causal (t<0)
    // On décide d'initialiser à 0.

    // Initialisation. ----------------------------------------------------

    // 1. Initialisation des coeffs.

    // Pour le filtre 1

    static int32_t a_1[] = {1*D,-1.84602348*D,0.98747166*D};
    static int32_t b_1[] = {1*D,2*D,1*D};
    static int32_t g_1 = 0.002019*D;
    //printf("la valeur de g_1 vaut : %d\n", b_1[1]);

    // Pour le filtre 2

    static int32_t a_2[] = {1*D,-1.84997046*D,0.98763379*D};
    static int32_t b_2[] = {1*D,2*D,1*D};
    static int32_t g_2 = 0.001964*D;

    // Pour le filtre 3

    static int32_t a_3[] = {1*D,-1.85002393*D,0.99474384*D};
    static int32_t b_3[] = {1*D,-2*D,1*D};
    static int32_t g_3 = 0.0228*D;

    // Pour le filtre 4

    static int32_t a_4[] = {1*D,-1.8593739*D,0.99490711*D};
    static int32_t b_4[] = {1*D,-2*D,1*D};
    static int32_t g_4 = 0.02294*D;

    // 2. Initialisation pour la récurrence.

    static int32_t Sample1[] = {0,0,0}; // Filtre 1
    static int32_t Output1[] = {0,0,0};

    static int32_t Sample2[] = {0,0,0}; // Filtre 2
    static int32_t Output2[] = {0,0,0};

    static int32_t Sample3[] = {0,0,0}; // Filtre 3
    static int32_t Output3[] = {0,0,0};

    static int32_t Sample4[] = {0,0,0}; // Filtre 4
    static int32_t Output4[] = {0,0,0};

    Sample1[2] = (int32_t) newSample;
    int32_t y;

    // Traitement. -----------------------------------------------

    // Pour le filtre 1 -------------

    Output1[2] = (b_1[0]*Sample1[2] + b_1[1]*Sample1[1] + b_1[2]*Sample1[0] - (a_1[1]*Output1[1] + a_1[2]*Output1[0]))>>M;

    // Décaler d'un rang les sorties. On décale de la droite vers la gauche.

    Output1[0] = Output1[1];
    Output1[1] = Output1[2]; // Sorties.


    // Décaler d'un rang les entrées.

    Sample1[0] = Sample1[1];
    Sample1[1] = Sample1[2]; // Entrées.

    Sample2[2] = (Output1[2]*g_1)>>M; // Nouvelle entrée pour notre prochain étage.
    //printf("Valeur du sample : %f/n", Output1[2]*g_1);
    // Pour le filtre 2. ------------

    Output2[2] = (b_2[0]*Sample2[2] + b_2[1]*Sample2[1] + b_2[2]*Sample2[0] - (a_2[1]*Output2[1] + a_2[2]*Output2[0]))>>M;

    // Décaler d'un rang les entrées.

    Output2[0] = Output2[1];
    Output2[1] = Output2[2];

    // Décaler d'un rang les sorties.

    Sample2[0] = Sample2[1];
    Sample2[1] = Sample2[2];

    Sample3[2] = (Output2[2]*g_2)>>M;

    // Pour le filtre 3. -------------

    Output3[2] = (b_3[0]*Sample3[2] + b_3[1]*Sample3[1] + b_3[2]*Sample3[0] - (a_3[1]*Output3[1] + a_3[2]*Output3[0]))>>M;

    // Décaler d'un rang les entrées.

    Output3[0] = Output3[1];
    Output3[1] = Output3[2];

    // Décaler d'un rang les sorties.

    Sample3[0] = Sample3[1];
    Sample3[1] = Sample3[2];

    Sample4[2] = (Output3[2]*g_3)>>M;

    // Pour le filtre 4. -------------

    Output4[2] = (b_4[0]*Sample4[2] + b_4[1]*Sample4[1] + b_4[2]*Sample4[0] - (a_4[1]*Output4[1] + a_4[2]*Output4[0]))>>M;

    // Décaler d'un rang les entrées.

    Output4[0] = Output4[1];
    Output4[1] = Output4[2];

    // Décaler d'un rang les sorties.

    Sample4[0] = Sample4[1];
    Sample4[1] = Sample4[2];

    y = (Output4[2]*g_4)>>M;

    return y;
}
