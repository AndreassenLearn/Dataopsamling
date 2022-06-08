#include <Arduino.h>

#include "include/Flags.h"
#include "include/Lm335.h"
#include "include/Led.h"
#include "include/Button.h"
#include "include/Storage.h"
#include "include/Measurer.h"
#include "include/ProgramFlow.h"

/* Port bits */
#define BIT_ADC_TEMPERATURE_SENSOR 0

/* Millisecond intervals */
#define MS_MEASURE_INTERVAL 30000 // 30s
#if WDT_PRESCALER_SETTING == 0
	#define MS_MEASURE_COUNT_UP_INTERVAL 8192 // 8s
#elif WDT_PRESCALER_SETTING == 1
	#define MS_MEASURE_COUNT_UP_INTERVAL 4096 // 4s
#endif

/* Objects */
Lm335 temperatureSensor = Lm335(BIT_ADC_TEMPERATURE_SENSOR);
Led led = Led(&DDRD, &PORTD, PORTD5);
Button button = Button(&DDRD, &PORTD, &PIND, PORTD2);
Storage storage = Storage();
Measurer measurer = Measurer(MS_MEASURE_INTERVAL, MS_MEASURE_COUNT_UP_INTERVAL, storage, temperatureSensor);
ProgramFlow program = ProgramFlow(led, button, storage);

/* Milliseconds */
uint64_t currentMillis, previousPrintMillis, previousWakeMillis, previousMeasureCountUpMillis = 0;

/* Interrupts */
EMPTY_INTERRUPT(ADC_vect); // Enable ADC interrupt (triggers after ADC conversion completion).
//EMPTY_INTERRUPT(INT0_vect);

void setup()
{
  Serial.begin(115200);
	Serial.flush();
	
	// Wait for button press before starting program.
	led.On();
	while (button.GetState() != Button::Pressed)
	{
		button.Service(millis());
	}
	led.Off();
	
	// Configure interrupts
	program.ConfigureButtonInterrupt();
	program.ConfigureWdtInterrupt();
	sei(); // Enable interrupts.
	
	storage.Initiate(measurer.S_INTERVAL);
}

void loop()
{
	currentMillis = millis();
	
	program.KickWDT();
	
	// Services
	button.Service(currentMillis);
	led.Service(currentMillis);
	measurer.Service();
	
	program.HandleButton();
}

ISR(INT0_vect)
{
#ifdef VERBOSE_MODE
	Serial.println("Interrupt: INT0");
#endif

	EIMSK &= ~(1UL << INT0); // Disable INT0 (until next sleep).
	button.SetState(Button::Interrupt);
}

ISR(WDT_vect)
{
#ifdef VERBOSE_MODE
	Serial.println("Interrupt: WDT");
#endif
	
	measurer.CountUp();
}
