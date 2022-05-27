// DSPIC33FJ128MC802 Configuration Bit Settings
// 'C' source line config statements
// FBS
#pragma config BWRP = WRPROTECT_OFF     // Boot Segment Write Protect (Boot Segment may be written)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash Code Protection (No Boot program Flash segment)
#pragma config RBS = NO_RAM             // Boot Segment RAM Protection (No Boot RAM)

// FSS
#pragma config SWRP = WRPROTECT_OFF     // Secure Segment Program Write Protect (Secure segment may be written)
#pragma config SSS = NO_FLASH           // Secure Segment Program Flash Code Protection (No Secure Segment)
#pragma config RSS = NO_RAM             // Secure Segment Data RAM Protection (No Secure RAM)

// FGS
#pragma config GWRP = OFF               // General Code Segment Write Protect (User program memory is not write-protected)
#pragma config GSS = OFF                // General Segment Code Protection (User program memory is not code-protected)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Mode (Internal Fast RC (FRC))
#pragma config IESO = OFF               // Internal External Switch Over Mode (Start-up device with user-selected oscillator source)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Source (Primary Oscillator Disabled)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration (Allow Only One Re-configuration)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor (Both Clock Switching and Fail-Safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler (1:32,768)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // POR Timer Value (128ms)
#pragma config ALTI2C = OFF             // Alternate I2C  pins (I2C mapped to SDA1/SCL1 pins)
#pragma config LPOL = ON                // Motor Control PWM Low Side Polarity bit (PWM module low side output pins have active-high output polarity)
#pragma config HPOL = ON                // Motor Control PWM High Side Polarity bit (PWM module high side output pins have active-high output polarity)
#pragma config PWMPIN = ON              // Motor Control PWM Module Pin Mode bit (PWM module pins controlled by PORT register at device Reset)

// FICD
#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>


/* Configures PLL prescaler, PLL postscaler, PLL divisor to obtain Fosc = 80MHz
 * with the FRC oscillator (Fin = 7.37MHz). 
 * We obtain Fosc = 7.37MHz*65/(*2) = 79.96MHz */
void frcPll40MHzConfig(void) {
	// Fosc = Fin*M/(N1*N2), where :
	//		M = PLLFBD + 2
	// 		N1 = PLLPRE + 2
	// 		N2 = 2 x (PLLPOST + 1)
    PLLFBD = 63;
    CLKDIVbits.PLLPRE = 1;
    CLKDIVbits.PLLPOST = 0;

	// Initiate Clock Switch to FRC with PLL
	__builtin_write_OSCCONH( 0x01 );
	__builtin_write_OSCCONL( OSCCON | 0x01 );
	// Wait for Clock switch to occur
    while (OSCCONbits.COSC != 0b001);
    // Wait for PLL to lock
    while(OSCCONbits.LOCK != 1);
}

// Initialiser les moteurs et tout le blabla.

void setup(void){
	
	AD1PCFGL = 0xFFFF; //set all the pins on 1

	// Configuration des capteurs avec le moteur 1.
	QEI1CONbits.QEIM = 0b111; //Fait registre pour data
    QEI1CONbits.TQCKPS = 0b00; //1:1 prescale value
    QEI1CONbits.TQCS = 0 ;// Timer Clock Source Select bit = Internal clock (TCY)
    RPINR14bits.QEA1R = 2;
    RPINR14bits.QEB1R = 3;

	// Configuration des capteurs avec le moteur 2.
    QEI2CONbits.QEIM = 0b111;
    QEI2CONbits.TQCKPS = 0b00; //1:1 prescale value
    QEI2CONbits.TQCS = 0 ;// Timer Clock Source Select bit = Internal clock (TCY)
    RPINR16bits.QEA2R = 13;
    RPINR16bits.QEB2R = 14;

	// PWM1
    
    // Initializing Output Compare Module for generation of PWM1 that controls MOTOR 1
    
    // Provient de la Datasheet sur l'UV.
    
    T2CONbits.TON = 0; // Disable Timer
    OC1CONbits.OCM = 0b000; // Disable Output Compare Module
    OC1R = 0; // Write the duty cycle for the first PWM pulse
    OC1RS = 0; // Write the duty cycle for the second PWM pulse
    OC1CONbits.OCTSEL = 0; // Select Timer 2 as output compare time base
    OC1R = 1; // Load the Compare Register Value
    OC1CONbits.OCM = 0b110; // Select the Output Compare mode
    _RP9R = 0b10010; // Patte RB6.
    
    // Initialize and enable Timer2

    T2CONbits.TCS = 0; // Select internal instruction cycle clock
    T2CONbits.TGATE = 0; // Disable Gated Timer mode
    T2CONbits.TCKPS = 0b00; // Select 1:1 Prescaler 0b00
    TMR2 = 0x00; // Clear timer register
    PR2 = 3999; // Load the period value
    //IPC1bits.T2IP = 0x01; // Set Timer 2 Interrupt Priority Level
    //IFS0bits.T2IF = 0; // Clear Timer 2 Interrupt Flag
    //IEC0bits.T2IE = 1; // Enable Timer 2 interrupt
    T2CONbits.TON = 1; // Start Timer2
    
    //PWM2

    // Provient de la Datasheet sur l'UV.
    
    T3CONbits.TON = 0; // Disable Timer // Moteur de gauche.
    OC2CONbits.OCM = 0b000; // Disable Output Compare Module
    OC2R = 0; // Write the duty cycle for the first PWM pulse
    OC2RS = 0; // Write the duty cycle for the second PWM pulse
    OC2CONbits.OCTSEL = 1; // Select Timer 3 as output compare time base
    OC2R = 1; // Load the Compare Register Value
    OC2CONbits.OCM = 0b110; // Select the Output Compare mode
    _RP10R = 0b10011; // Patte RB7.
    
    
    // Initialize and enable Timer3

    T3CONbits.TCS = 0; // Select internal instruction cycle clock // Moteur de droite£.
    T3CONbits.TGATE = 0; // Disable Gated Timer mode
    T3CONbits.TCKPS = 0b00; // Select 1:1 Prescaler 0b00
    TMR3 = 0x00; // Clear timer register
    PR3 = 3999; // Load the period value
    //IPC2bits.T3IP = 0x01; // Set Timer3 Interrupt Priority Level
    //IFS0bits.T3IF = 0; // Clear Timer3 Interrupt Flag
    //IEC0bits.T3IE = 1; // Enable Timer3 interrupt
    T3CONbits.TON = 1; // Start Timer3
    
}

void swap(int swap_moteur1, int swap_moteur2){
    
    int inversion_moteur1 = 0;
    int inversion_moteur2 = 0;
    
    if(swap_moteur1 < 0)
    {
        inversion_moteur1 = 1;
    } else if (swap_moteur2 < 0){
        inversion_moteur2 = 1;
    }
    
    QEI2CONbits.SWPAB = inversion_moteur1;
    QEI2CONbits.SWPAB = inversion_moteur2;
}