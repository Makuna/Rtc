/*-------------------------------------------------------------------------
RTC library

Written by Michael C. Miller.

I invest time and resources providing this open source code,
please support me by dontating (see https://github.com/Makuna/Rtc)

-------------------------------------------------------------------------
This file is part of the Makuna/Rtc library.

Rtc is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

Rtc is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with Rtc.  If not, see
<http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------*/

#pragma once


class RtcTemperature
{
public:
    // Constructor
	// a) Merge RTC registers into signed scaled temperature (x256),
	//    then bind to RTC resolution.
	//         |         r11h          | DP |         r12h         |
	// Bit:     15 14 13 12 11 10  9  8   .  7  6  5  4  3  2  1  0  -1 -2
	//           s  i  i  i  i  i  i  i   .  f  f  0  0  0  0  0  0
	//
	// b) Rescale to (x4) by right-shifting (6) bits
	//         |                                         | DP |    |
	// Bit:     15 14 13 12 11 10  9  8  7  6  5  4  3  2   .  1  0  -1 -2
	//           s  s  s  s  s  s  s  i  i  i  i  i  i  i      f  f   0  0
    RtcTemperature(int8_t highByteDegreesC, uint8_t lowByteDegreesC)
    {
        int16_t scaledDegC = ((highByteDegreesC << 8) | (lowByteDegreesC & 0xC0)) >> 6;
		_centiDegC = scaledDegC * 100 / 4;
    }

	RtcTemperature(int16_t centiDegC = 0) :
		_centiDegC(centiDegC)
	{
	}

    // Float temperature Celsius
    float AsFloatDegC()
    {
        return (float)_centiDegC / 100.0f;
    }

	// Float temperature Fahrenheit
	float AsFloatDegF()
	{
		return AsFloatDegC() * 1.8f + 32.0f;
	}

	// centi degrees (1/100th of a degree), 
	int16_t AsCentiDegC()
	{
		return _centiDegC;
	}

	void Print(Stream& target, uint8_t decimals = 2, char decimal = '.')
	{
		int16_t decimalDivisor = 1;
		int16_t integerPart;
		int16_t decimalPart;

		{
			int16_t rounded = abs(_centiDegC);
			// round up as needed
			if (decimals == 0)
			{
				rounded += 50;
			}
			else if (decimals == 1)
			{
				rounded += 5;
				decimalDivisor = 10;
			}

			integerPart = rounded / 100;
			decimalPart = (rounded % 100) / decimalDivisor;
		}

		// test for zero before printing negative sign to not print-0.00
		if (_centiDegC < 0 && (integerPart != 0 || decimalPart != 0))
		{
			target.print('-');
		}

		// print integer part
		target.print(integerPart);

		// print decimal part
		if (decimals != 0)
		{
			target.print(decimal);

			if (decimalPart != 0)
			{
				target.print(decimalPart);
			}
			else
			{
				// append zeros as requested
				while (decimals > 0)
				{
					target.print('0');
					decimals--;
				}
			}
		}
	}

	bool operator==(const RtcTemperature& other) const
	{
		return (_centiDegC == other._centiDegC);
	};

	bool operator>(const RtcTemperature& other) const
	{
		return (_centiDegC > other._centiDegC);
	};

	bool operator<(const RtcTemperature& other) const
	{
		return (_centiDegC < other._centiDegC);
	};

	bool operator>=(const RtcTemperature& other) const
	{
		return (_centiDegC >= other._centiDegC);
	};

	bool operator<=(const RtcTemperature& other) const
	{
		return (_centiDegC <= other._centiDegC);
	};

	bool operator!=(const RtcTemperature& other) const
	{
		return (_centiDegC != other._centiDegC);
	};

	RtcTemperature operator-(const RtcTemperature& right)
	{
		RtcTemperature result;

		result._centiDegC = (_centiDegC - right._centiDegC);
		return result;
	}

	RtcTemperature operator+(const RtcTemperature& right)
	{
		RtcTemperature result;

		result._centiDegC = (_centiDegC + right._centiDegC);
		return result;
	}

protected:
    int16_t  _centiDegC;  // 1/100th of a degree temperature (100 x degC)
};
