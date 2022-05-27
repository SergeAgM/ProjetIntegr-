#ifndef FILTRE_H
#define	FILTRE_H

#define M 11 // Number of fractionnary bits
#define D (1ul << M) // Denominator for the fixed-point numbers

int32_t filtreBP_delta_plus(int16_t newSample);

int32_t filtreBP_delta_moins(int16_t newSample);

#endif	/* ADC_H */