# Quadrature Decoder for Teensy 4.0 Using Hardware based Decoding

When the Teensy 4.0 was released to the public, one of the first main attractions for me to start using it was that it was powerful! Reading though the docs for the NXP chip used, I learned about the Hardware based Quadrature decoding. So I set out trying to create a quick and easy to use library for this.

The pin assignments are fixed (for now) and with the limited amount of physical pin access from the XBAR, right now a max of 2 encoders are supported.

| Pin Assignments| Encoder 1 | Encoder 2 |
|---|---|---|
| A | 1 | 4 |
| B | 2 | 5 |
| Z | 3 | 7 |

Currently this library is a Platform.IO project which you can import and use. If you want a dedicated library... ill get on to that soon

## How to use

Depending on which Encoder you want to use, (1 or 2) initialize the object like this:

```c++
QuadDecoder<#> // Where # is 1 or 2
```

## Modes

There are currently 2 modes you may choose to run the quadrature decoder: Position mode or Velocity mode

### Position Mode

Position mode is rather simple and may be what you are looking for. This just outputs the current count of either channel of encoder. When creating the encoder object, set mode to 0 or leave it blank.

i.e.

```c++
QuadDecoder<1> Enc1(); //New Quadrature Encoder Object

// OR

QuadDecoder<1> Enc1(0); //New Quadrature Encoder Object
```

Next when you begin your encoder, all you need to provide is the ticks per revolution. It is currently set to reset back to 0 after one complete revolution.

```c++
Enc1.begin(TICKS_PER_REV);
```

and lastly, to get the position just call:
```c++
Enc1.getCount()
```


### Velocity Mode

Velocity mode is just like position mode but allows for getting a really accurate velocity of the motor. It uses on board PIT timers which in my testing was the most accurate way I could get the velocity of the wheel. The default of the PIT timers are 10Hz but can be set higher. When creating the encoder object, set mode to 1.

i.e.

```c++
QuadDecoder<1> Enc1(1); //New Quadrature Encoder Object
```

Next when you begin your encoder, you need to provide is the ticks per revolution as well as the calculation rate of velocity in microseconds(us) - Default is 100ms. Velocity will output a (+) or (-) value depending on the direction of the motor spin. You may also get current position at the same time using velocity mode

```c++
Enc1.begin(TICKS_PER_REV, UPDATE_RATE_US);

// OR

Enc1.begin(TICKS_PER_REV); // Leaves it default at 100ms/100000us
```

and lastly, to get the velocity just call:
```c++
Enc1.getVelocity()
```




