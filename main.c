#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "stdio.h"
#include "stdint.h"
typedef struct
{
	unsigned long buf[100];
} typebuf;
static typebuf ulADC0_Value[2];
unsigned long ADCNumBuf = 0;
unsigned long ADC_idx=0;
unsigned long refresh = 0;


void TIMConfigExp()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    //
	// Configure TimerA as a 16-bit one shot timer, and TimerB as a 16-bit edge
	// capture counter.
	//
	TimerConfigure(TIMER0_BASE,
	                 (TIMER_CFG_16_BIT_PAIR |
	                 TIMER_CFG_A_ONE_SHOT |
	                 TIMER_CFG_B_CAP_COUNT));

	// Configure the counter (TimerB) to count both edges.
	TimerControlEvent(TIMER0_BASE, TIMER_B, TIMER_EVENT_BOTH_EDGES);
	TimerIntEnable( TIMER0_BASE, TIMER_CAPA_EVENT );
	// Enable the timers.
	TimerEnable(TIMER0_BASE, TIMER_BOTH);
}

void ADCConfigExp()
{
	//
	// The ADC0 peripheral must be enabled for use.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

	//
	// For this example ADC0 is used with AIN0 on port E7.
	// The actual port and pins used may be different on your part, consult
	// the data sheet for more information.  GPIO port E needs to be enabled
	// so these pins can be used.
	// TODO: change this to whichever GPIO port you are using.
	//
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

	//
	// Select the analog ADC function for these pins.
	// Consult the data sheet to see which functions are allocated per pin.
	// TODO: change this to select the port/pin you are using.
	//
//	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_7);

	//
	// Enable sample sequence 3 with a processor signal trigger.  Sequence 3
	// will do a single sample when the processor sends a signal to start the
	// conversion.  Each ADC module has 4 programmable sequences, sequence 0
	// to sequence 3.  This example is arbitrarily using sequence 3.
	//
	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

	//
	// Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
	// single-ended mode (default) and configure the interrupt flag
	// (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
	// that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
	// 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
	// sequence 0 has 8 programmable steps.  Since we are only doing a single
	// conversion using sequence 3 we will only configure step 0.  For more
	// information on the ADC sequences and steps, reference the datasheet.
	//
	    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
	                             ADC_CTL_END);

	    //
	    // Since sample sequence 3 is now configured, it must be enabled.
	    //
	    ADCSequenceEnable(ADC0_BASE, 3);

	    //
	    // Clear the interrupt status flag.  This is done to make sure the
	    // interrupt flag is cleared before we sample.
	    //
	    ADCIntClear(ADC0_BASE, 3);

	    //
	    // Sample AIN0 forever.  Display the value on the console.
	    //
}

void  ADCSeq3_IRQHandler(void)
{
	unsigned long  *val = &(ulADC0_Value[ADCNumBuf].buf[ADC_idx]);
	ADCIntClear(ADC0_BASE, 3);

	ADCSequenceDataGet(ADC0_BASE, 3, val );
	ADC_idx ++;
	if(ADC_idx >= 1000)
	{
		ADC_idx = 0;
		ADCNumBuf ^= 0x01;
		refresh = 1;
	}
}

void  Timer0A_IRQHandler(void)
{//
	TimerIntClear(TIMER0_BASE, TIMER_CAPA_EVENT );
    // Trigger the ADC conversion.
    //
    ADCProcessorTrigger(ADC0_BASE, 3);

}


int main(void)
{
	volatile unsigned long ulLoop;
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);


	//
		// Set the clocking to run at 20 MHz (200 MHz / 10) using the PLL.  When
		// using the ADC, you must either use the PLL or supply a 16 MHz clock
		// source.
		// TODO: The SYSCTL_XTAL_ value must be changed to match the value of the
		// crystal on your board.
		//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

		SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
					   SYSCTL_XTAL_10MHZ);

//		__disable_irq();
		/*ADCConfigExp();
		TIMConfigExp();*/
//		__enable_irq();

	    //
	    // Enable the GPIO pin for the LED (PF3).  Set the direction as output, and
	    // enable the GPIO pin for digital function.
	    //

	    while(1)
	    {
	    		// Output high level
	    		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, ~GPIO_PIN_4);
	    		// Delay some time
	    		for(ulLoop = 0; ulLoop < 200000; ulLoop++);
	    		// Output low level
	    		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);

	    	if( refresh )
	    	{
	    		refresh = 0;
	    		uint32_t n = ADCNumBuf ^ 0x01;

	    	}
	    }

}


