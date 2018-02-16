// These tests do not rely on RTC hardware at all

//#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>

void PrintPassFail(bool passed)
{
    if (passed)
    {
      Serial.print("passed");
    }
    else
    {
      Serial.print("failed");
    }
}

void ComparePrintlnPassFail(RtcTemperature& rtcTemp, float compare)
{
    Serial.print(rtcTemp.AsFloatDegC());
    Serial.print("C ");
    PrintPassFail(rtcTemp.AsFloatDegC() == compare);
    Serial.println();
}

void ConstructorTests()
{
    // RTC constructors
    Serial.println("Constructors:");
    {
      RtcTemperature temp075Below(0b11111111, 0b01000000); // -0.75 
      ComparePrintlnPassFail(temp075Below, -0.75f);
      
      RtcTemperature temp050Below(0b11111111, 0b10000000); // -0.5 
      ComparePrintlnPassFail(temp050Below, -0.50f);
      
      RtcTemperature temp025Below(0b11111111, 0b11000000); // -0.25 
      ComparePrintlnPassFail(temp025Below, -0.25f);
      
      RtcTemperature tempZero(0b00000000, 0b00000000); // 0.0 
      ComparePrintlnPassFail(tempZero, -0.0f);
      
      RtcTemperature temp025Above(0b00000000, 0b01000000); // 0.25 
      ComparePrintlnPassFail(temp025Above, 0.25f);
      
      RtcTemperature temp050Above(0b00000000, 0b10000000); // 0.5 
      ComparePrintlnPassFail(temp050Above, 0.5f);
      
      RtcTemperature temp075Above(0b00000000, 0b11000000); // 0.75 
      ComparePrintlnPassFail(temp075Above, 0.75f);

      RtcTemperature temp25Above(0b00011001, 0b00000000); // 25.0
      ComparePrintlnPassFail(temp25Above, 25.0f);

      RtcTemperature temp25Below(0b11100111, 0b00000000); // -25.0
      ComparePrintlnPassFail(temp25Below, -25.0f);
    }
    Serial.println();
    
    // SameType
    {
      Serial.print("same type ");
      RtcTemperature temp25Below(0b11100111, 0b00000000); // -25.0
      RtcTemperature test = temp25Below;
      ComparePrintlnPassFail(test, -25.0f);
    }
    
    // CentiDegrees
    {
      Serial.print("centi degrees ");
      RtcTemperature temp025Below(-25); // -0.25
      ComparePrintlnPassFail(temp025Below, -0.25f);

      Serial.print("centi degrees ");
      RtcTemperature temp025Above(25); // 0.25
      ComparePrintlnPassFail(temp025Above, 0.25f);
      
      Serial.print("centi degrees ");
      RtcTemperature temp25Below(-2500); // -25.0
      ComparePrintlnPassFail(temp25Below, -25.0f);

      Serial.print("centi degrees ");
      RtcTemperature temp25Above(2500); // 25.0
      ComparePrintlnPassFail(temp25Above, 25.0f);
    }
    
    Serial.println();
}

void PrintlnExpected(RtcTemperature& temp, uint16_t digits)
{
  Serial.print(" = ");
  Serial.print(temp.AsFloatDegC(), digits);
  Serial.println();
}

void PrintTests()
{
  Serial.println("Prints:");
  
  RtcTemperature temp25Above(2500);
  temp25Above.Print(Serial);
  PrintlnExpected(temp25Above, 2);
  
  RtcTemperature temp25Below(-2500);
  temp25Below.Print(Serial);
  PrintlnExpected(temp25Below, 2);

  RtcTemperature temp025Above(25);
  temp025Above.Print(Serial);
  PrintlnExpected(temp025Above, 2);
  temp025Above.Print(Serial, 1);
  PrintlnExpected(temp025Above, 1);
  
  RtcTemperature temp025Below(-25);
  temp025Below.Print(Serial);
  PrintlnExpected(temp025Below, 2);
  temp025Below.Print(Serial, 1);
  PrintlnExpected(temp025Below, 1);

  RtcTemperature temp050Above(50);
  temp050Above.Print(Serial);
  PrintlnExpected(temp050Above, 2);
  temp050Above.Print(Serial, 0);
  PrintlnExpected(temp050Above, 0);
  
  RtcTemperature temp050Below(-50);
  temp050Below.Print(Serial);
  PrintlnExpected(temp050Below, 2);
  temp050Below.Print(Serial, 0);
  PrintlnExpected(temp050Below, 0);
  temp050Below.Print(Serial, 2, ',');
  Serial.println(" == -0,50");
  
  Serial.println();
}

void MathmaticalOperatorTests()
{
  Serial.println("Mathmaticals:");

  RtcTemperature temp050Below(-50);
  RtcTemperature temp050Above(50);
  RtcTemperature temp050Diff(100);
  RtcTemperature temp050Same(-50);
  RtcTemperature tempResult;

  Serial.print("equality ");
  PrintPassFail(temp050Below == temp050Same);
  Serial.println();

  Serial.print("inequality ");
  PrintPassFail(temp050Below != temp050Above);
  Serial.println();
  
  Serial.print("less than ");
  PrintPassFail(temp050Below < temp050Above);
  Serial.println();

  Serial.print("greater than ");
  PrintPassFail(temp050Above > temp050Below);
  Serial.println();

  Serial.print("less than ");
  PrintPassFail(temp050Below <= temp050Above);
  Serial.print(" or equal ");
  PrintPassFail(temp050Below <= temp050Same);
  Serial.println();

  Serial.print("greater than ");
  PrintPassFail(temp050Above >= temp050Below);
  Serial.print(" or equal ");
  PrintPassFail(temp050Below >= temp050Same);
  Serial.println();

  tempResult = temp050Above - temp050Below;
  Serial.print("subtraction ");
  PrintPassFail(tempResult == temp050Diff);
  Serial.println();

  tempResult = temp050Above + temp050Above;
  Serial.print("addition ");
  PrintPassFail(tempResult == temp050Diff);
  Serial.println();
    
  Serial.println();
}

void setup () 
{
    Serial.begin(115200);
    while (!Serial);
    Serial.println();
    
    ConstructorTests();
    PrintTests();
    MathmaticalOperatorTests();
}

void loop () 
{
    delay(500);
}

