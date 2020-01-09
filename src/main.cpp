/*  QuadEncoder.h Library was created
*   By Anmol Modur
*   9/20/19
*   v1.2
*   Encoder 1 Pins: 1(A), 2(B), 3(Z)
*   Encoder 2 Pins: 4(A), 5(B), 7(Z)
*/

#include <Arduino.h>
#include "QuadDecoder.h"

#define TICKS_PER_REV 118700 // Number of ticks per wheel revolution

QuadDecoder<1> Enc1(1); //New Quadrature Encoder Object
QuadDecoder<2> Enc2(1); //New Quadrature Encoder Object

void setup()
{
	// Quadtrature decoder setup
	Enc1.begin(TICKS_PER_REV);
	Enc2.begin(TICKS_PER_REV);

	// Start Serial Monitor
	Serial.begin(115200);
	delay(100);
}

void loop()
{
	Serial.printf("%d\t %f\t %d\t %f\n", Enc1.getCount(), Enc1.getVelocity(), Enc2.getCount(), Enc2.getVelocity());
	delay(100);
}