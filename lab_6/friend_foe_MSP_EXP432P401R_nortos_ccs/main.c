/* Description: Skeleton code for friend or foe classifier
 *
 * Author: Naga Kandasamy
 * Date created: August 17, 2019
*******************************************************************************/
/* DriverLib includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard includes */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* User-specific includes. */
#include "uart_functions.h"

/* Global variables shared between the main code and the ISR. */
bool intFlag = true;
uint16_t firstVal, ticks, count, periodCount, oneMsCount, twoMsCount = 0;

/* Configuration parameters for timer in PWM mode. */
Timer_A_PWMConfig pwmConfig =
{
 TIMER_A_CLOCKSOURCE_ACLK,                  // Clock source
 TIMER_A_CLOCKSOURCE_DIVIDER_1,             // Divider ratio, 1:1
 256,                                       // Timer period which will stored in CCR0
 TIMER_A_CAPTURECOMPARE_REGISTER_1,         // Compare register to use
 TIMER_A_OUTPUTMODE_SET_RESET,              // Output mode for PWM signal
 128                                        // Duty cycle length which will be stored in CCR1
};

/* ISR to handle capture events at Timer A2. */
void TA2_N_IRQHandler(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
    count++;

    if (count == 1)
    {
        /* UNCOMMENT FOR METHOD 1 */ /*
        firstVal = Timer_A_getCaptureCompareCount(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
        printf("%u\n", firstVal);
        */

        /* UNCOMMENT FOR METHOD 2 */ /*
        Timer_A_clearTimer(TIMER_A2_BASE);
        */
    }
    else
    {

        uint8_t done = 0;
        /* UNCOMMENT FOR METHOD 1 */ /*
        uint8_t ticks = Timer_A_getCaptureCompareCount(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1) - firstVal;
        firstVal += ticks;

        */

        /* UNCOMMENT FOR METHOD 2 */ /*
        uint8_t ticks = Timer_A_getCaptureCompareCount(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
        Timer_A_clearTimer(TIMER_A2_BASE);
        */

        printf("%u\n", ticks);

        unsigned int time = ticks / 12000;   // To get ms instead of sec
        printf("%d\n", time);
        if(time == 1)
        {
            oneMsCount++;
        }
        else if(time == 2)
        {
            twoMsCount++;
        }
        else if(time == 10 && oneMsCount == 9)
        {
            oneMsCount = 0;
            writeString("\n\rFriend Signal");
            done = 1;
        }
        else
        {
            writeString("\n\rUnknown Signal");
            oneMsCount = 0;
            twoMsCount = 0;
            done = 1;
        }

        if(oneMsCount > 0 && twoMsCount > 0)
        {
            writeString("\n\rUnknown Signal");
            oneMsCount = 0;
            twoMsCount = 0;
            done = 1;
        }

        if(twoMsCount == 10)
        {
            writeString("\n\rFoe Signal");
            oneMsCount = 0;
            done = 1;
        }

        if(done == 1)
        {
            intFlag = false;
            Timer_A_stopTimer(TIMER_A2_BASE);
            return;
        }
    }

    return;
}

/* Function initializes the Timer module used by the signal classifier. */
void initSignalClassifier(void)
{
    /* Configuration parameters for input capture mode. */
    const Timer_A_ContinuousModeConfig continuousModeConfig =
    {
     TIMER_A_CLOCKSOURCE_SMCLK,                 // Clock source is SMCLK
     TIMER_A_CLOCKSOURCE_DIVIDER_1,             // 1:1 divider ratio for the counter clock
     TIMER_A_TAIE_INTERRUPT_DISABLE,            // Disable overflow ISR
     TIMER_A_DO_CLEAR
    };

    const Timer_A_CaptureModeConfig captureModeConfig =
    {
     TIMER_A_CAPTURECOMPARE_REGISTER_1,                     // Register 1 holds the capture value
     TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE,           // Capture on both rising and falling edges
     TIMER_A_CAPTURE_INPUTSELECT_CCIxA,                     // Input select
     TIMER_A_CAPTURE_SYNCHRONOUS,
     TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,               // Enable capture interrupt
     TIMER_A_OUTPUTMODE_OUTBITVALUE
    };

    /* Configure P5.6 as primary module input for Timer A2. */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
    /* Configure timer in continuous mode with input capture. */
    Timer_A_configureContinuousMode(TIMER_A2_BASE, &continuousModeConfig);
    Timer_A_initCapture(TIMER_A2_BASE, &captureModeConfig);
    Timer_A_clearTimer(TIMER_A2_BASE);

    /* Enable interrupt processing for the timer. */
    Timer_A_enableCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
    Interrupt_enableInterrupt(INT_TA2_N);
    Interrupt_enableMaster();
    Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_CONTINUOUS_MODE);  // Start timer.

    return;
}

/* Function generates an unknown signal. */

/* ISR to handle end of period notification from Timer A0. */
void TA0_0_IRQHandler(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    writeString("\n\rEnd of timer period");
    /* User-defined code. */
    periodCount++;
    if(periodCount == 5)
    {
        writeString("\n\rChanging");
        uint16_t PWM_PERIOD = 1280;
        uint16_t DUTY_CYCLE_LENGTH = 0.5 * PWM_PERIOD;

        /* Configuration parameters for timer in PWM mode. */
        pwmConfig.dutyCycle = DUTY_CYCLE_LENGTH;
        pwmConfig.timerPeriod = PWM_PERIOD;
        Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig); // Generate PWM signal.
    }
    else if(periodCount == 6)
    {
        writeString("\n\rChanging");
        uint16_t PWM_PERIOD = 256;
        uint16_t DUTY_CYCLE_LENGTH = 0.5 * PWM_PERIOD;

        /* Configuration parameters for timer in PWM mode. */
        pwmConfig.dutyCycle = DUTY_CYCLE_LENGTH;
        pwmConfig.timerPeriod = PWM_PERIOD;
        Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig); // Generate PWM signal.
        periodCount = 0;
    }

    return;
}

void generateUnknownSignal(void)
{
    uint16_t PWM_PERIOD = 256;
    uint16_t DUTY_CYCLE_LENGTH = 0.5 * PWM_PERIOD;
    pwmConfig.dutyCycle = DUTY_CYCLE_LENGTH;
    pwmConfig.timerPeriod = PWM_PERIOD;

    /* Configure P2.4 as primary module output for PWM signal generated by Timer A0. */
   GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
   Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig); // Generate PWM signal.

    return;
}

/* Function to generate the friend signal. */
void generateFriendSignal(void)
{
    uint16_t PWM_PERIOD = 256;
    uint16_t DUTY_CYCLE_LENGTH = 0.5 * PWM_PERIOD;
    pwmConfig.dutyCycle = DUTY_CYCLE_LENGTH;
    pwmConfig.timerPeriod = PWM_PERIOD;

    /* Configure P2.4 as primary module output for PWM signal generated by Timer A0. */
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig); // Generate PWM signal.

    /* Uncomment these lines of you wish to use an ISR to handle the end of a PWM period.
     * For example, to count the number of PWM periods, vary the duty cycle as a function of
     * the PWM period, etc.
     */
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Interrupt_enableInterrupt(INT_TA0_0);
    return;
}

/* Function to generate the foe signal. */
void generateFoeSignal(void)
{
    uint16_t PWM_PERIOD = 512;
    uint16_t DUTY_CYCLE_LENGTH = 0.5 * PWM_PERIOD;
    pwmConfig.dutyCycle = DUTY_CYCLE_LENGTH;
    pwmConfig.timerPeriod = PWM_PERIOD;

    /* Configure P2.4 as primary module output for PWM signal generated by Timer A0. */
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig); // Generate PWM signal.
    return;
}

int main(void)
{
    /* Stop Watchdog timer.  */
    MAP_WDT_A_holdTimer();

    /* Initialize the high-speed clock system. */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12); // Set DCO to 12 MHz
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* Initialize the low-speed auxiliary clock system. */
    CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ); // Reference oscillator is set to 128KHz
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1); // Auxiliary clock derives from reference

    /* Initialize the UART module. */
    initUART();
    writeString("Established communication with the board");

    char userChoice = 'u'; // Default value to to generate an unknown signal
    writeString("\r\nChoose a signal to generate\r\n");
    char buf[1];
    readString(buf);
    userChoice = buf[0];
    switch (userChoice)
    {
    case 'F':
        writeString("\n\rGenerating friend signal");
        generateFriendSignal();
        break;

    case 'f':
        writeString("\n\rGenerating foe signal");
        generateFoeSignal();
        break;

    default:
        writeString("\n\rGenerating some unknown signal");
        generateUnknownSignal();
        break;
    }


    /* Initialize the Timer A2 for capture mode. */
    initSignalClassifier();

    /* Main Processing loop for signal classification. */
     while(intFlag);

     writeString("\n\rNum ticks: ");
     writeInt(ticks);

     writeString("\n\r");

     while(1);
}



