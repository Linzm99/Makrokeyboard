#include <Adafruit_NeoPixel.h>

char PIN = 2;
int NUMPIXELS = 1; 

Adafruit_NeoPixel led = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {

  
  led.clear();
  led.begin();
  led.setBrightness(20);
  led.setPixelColor(0,0,0,255);
  led.show();

  

  

  
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
