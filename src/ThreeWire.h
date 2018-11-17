#pragma once

class ThreeWire 
{
public:
    ThreeWire(uint8_t ioPin, uint8_t clkPin, uint8_t cePin) :
        _ioPin(ioPin),
        _clkPin(clkPin),
        _cePin(cePin)
    {
    }

    void begin() {
        // just making sure they are in a default state
        // as required state is set when transmissions are started
        // three wire devices have internal pull downs so they will be low
        pinMode(_clkPin, INPUT);
        pinMode(_ioPin, INPUT);
        pinMode(_cePin, INPUT);
    }

    void end() {
        // reset back to lowest power usage
        pinMode(_clkPin, INPUT);
        pinMode(_ioPin, INPUT);
        pinMode(_cePin, INPUT);
    }

    void beginTransmission() {
        digitalWrite(_cePin, LOW); // default, not enabled
        pinMode(_cePin, OUTPUT);

        digitalWrite(_clkPin, LOW); // default, clock low
        pinMode(_clkPin, OUTPUT);

        pinMode(_ioPin, OUTPUT);

        digitalWrite(_cePin, HIGH); // start the session
        delayMicroseconds(4);           // tCC = 4us
    }

    void endTransmission() {
        digitalWrite(_cePin, LOW);
        delayMicroseconds(4);           // tCWH = 4us
    }

    void write(uint8_t value) {
        for (uint8_t bit = 0; bit < 8; bit++) {
            digitalWrite(_ioPin, value & 0x01);
            delayMicroseconds(1);     // tDC = 200ns

            // clock up, data is read by DS1302
            digitalWrite(_clkPin, HIGH);
            delayMicroseconds(1);     // tCH = 1000ns, tCDH = 800ns

            digitalWrite(_clkPin, LOW);
            delayMicroseconds(1);     // tCL=1000ns, tCDD=800ns

            value >>= 1;
        }
    }

    uint8_t read() {
        uint8_t value = 0;

        // Set IO line for input
        pinMode(_ioPin, INPUT);

        for (uint8_t bit = 0; bit < 8; bit++) {
            // first bit is present on io pin, so only clock the other
            // bits
            value <<= 1;
            value |= digitalRead(_ioPin);
        
            // Clock up, prepare for next
            digitalWrite(_clkPin, HIGH);
            delayMicroseconds(1);

            // Clock down, value is ready after some time.
            digitalWrite(_clkPin, LOW);
            delayMicroseconds(1);        // tCL=1000ns, tCDD=800ns
        }

        return value;
    }

private:
    const uint8_t _ioPin;
    const uint8_t _clkPin;
    const uint8_t _cePin;
};
