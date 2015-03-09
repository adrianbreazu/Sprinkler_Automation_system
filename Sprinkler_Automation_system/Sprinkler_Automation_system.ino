// ---------------------------------------------------------------------------
// Created by Breazu Adrian - breazuadrian@gmail.com
// Copyright 2015 License: GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html
// ---------------------------------------------------------------------------

// connect the soil sensor to Analog pin A0
// connect the relay to digital pin D7

#define Relay 7

int soilHumidity;

void setup () {
	Serial.begin(9800);
	//initialize Relay data pin
	pinMode(Relay, OUTPUT);
}

void loop() {
        //read soilHumidity
        soilHumidity = analogRead(0);
  
	//print soil value
	Serial.print("Soil humdity value: ");
	Serial.println(soilHumidity);
	delay(500);
        if (soilHumidity > 300)
        {
          Serial.println("Start relay for 2 seconds");
	  digitalWrite(Relay,LOW);
	  delay(2000);
  	  digitalWrite(Relay,HIGH);
        }
}
