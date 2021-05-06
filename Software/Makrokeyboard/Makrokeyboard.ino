#include <Adafruit_NeoPixel.h>
#include <Keyboard.h>

//defines for LEDS
#define FADE 1


//defines for I/Os
#define TS1 6
#define TS2 8
#define TS3 9
#define TS4 10
#define TS5 5

#define TR1 12
#define TR2 4
#define TR3 2
#define TR4 3

#define KEY_SPACE 32

uint8_t rows[4] = {TR4, TR3, TR2, TR1};
uint8_t cols[5] = {TS5, TS4, TS3, TS2, TS1};


//F-Keys
char key[4][5] = {{(char)KEY_F1, (char)KEY_F2, (char)KEY_F3, (char)KEY_F4, (char)KEY_F5},
                  {(char)KEY_F6, (char)KEY_F7, (char)KEY_F8, (char)KEY_F9, (char)KEY_F10},
                  {(char)KEY_F11, (char)KEY_F12, (char)KEY_F13, (char)KEY_F14, (char)KEY_F15},                    
                  {(char)KEY_F16, (char)KEY_F17, (char)KEY_F18, (char)KEY_F19, (char)KEY_F20}};                    

//Key Mask for second Key
char mask[4][5] = {{(char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL},
                   {(char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL},
                   {(char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL},                    
                   {(char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL, (char)KEY_LEFT_CTRL}};            

//Key Mask for third Key
char mask2[4][5] = {{(char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT},
                    {(char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT},
                    {(char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT},                    
                    {(char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT, (char)KEY_LEFT_SHIFT}};

//Mask for Strings
String str_mask[4][5] = {{"","","","",""},
                         {"","","","",""},
                         {"","","","",""},
                         {"","","","",""}};
                         
//Mask for selecting Key Behaviour :
//0: Regular key behaviour with masks
//1: Single key stroke of "Key" Matrix
//2: Print String from String Mask

char press_once[4][5] = {{0,0,0,0,0},
                         {0,0,0,0,0},
                         {0,0,0,0,0},                    
                         {0,0,0,0,0}};


//Led Maps for different Patterns
uint8_t led_map_default[4][5] = {{16,17,18,19,20},
                                 {15,14,13,12,11},
                                 {6,7,8,9,10},                    
                                 {5,4,3,2,1}};

uint8_t led_map_horz[21] = {16,17,18,19,20,15,14,13,12,11,6,7,8,9,10,5,4,3,2,1,0};
uint8_t led_map_diag[21] = {16,15,17,6,14,18,5,7,13,19,4,8,12,20,3,9,11,2,10,1,0};

//Buffer for pressed Keys
uint8_t key_buffer[4][5] = {0};


//Variables for RGB keys
char PIN = 7;
int NUMPIXELS = 21; 
unsigned int countges = 0;

unsigned int RES = 600;

int phaseshift = RES/90;

int adcval = 0;

int colorstato = 0;
int counter = 0;

int last_key = -1;
int fade_out = 0;


//Functions:

void RGB_Simple(uint8_t r, uint8_t g, uint8_t b);
void RGB_Solid_Single(uint16_t val);
void RGB_Solid_Rows(uint16_t val);
void RGB_Solid_Cols(uint16_t val);
void key_flash(void);
void key_wave(void);
void RGB_Fade_Diag(void);

void get_keys(void){
  for(uint8_t i = 0; i<4; i++){
    digitalWrite(rows[i], LOW);
    for(uint8_t j = 0; j<5; j++){
      delay(1);
      if(!digitalRead(cols[j])){
        if(key_buffer[i][j] == 0){
          key_handle(i,j,1);
          fade_out = 255;
        }
        key_buffer[i][j] = 1;
        last_key = led_map_default[i][j];
        Serial.println(fade_out);
        
      }
      else{
        if(key_buffer[i][j] == 1){
          key_handle(i,j,0);
        }
        key_buffer[i][j] = 0;
      }
    }
    digitalWrite(rows[i], HIGH);
  }

}





void key_handle(uint8_t x, uint8_t y, uint8_t z){
  if(z == 1){
    if(press_once[x][y] == 0){
      if(mask[x][y]!= 0){
        Keyboard.press(mask[x][y]);
      }
      if(mask2[x][y]!= 0){
        Keyboard.press(mask2[x][y]);
      }
      Keyboard.press(key[x][y]);
    }
    else if(press_once[x][y] == 2){
      Keyboard.print(str_mask[x][y]);
    }
    else{
      Keyboard.print(key[x][y]);
    }
  }
  else{
    if(press_once[x][y] == 0){
      if(mask[x][y]!= 0){
        Keyboard.release(mask[x][y]);
      }
      if(mask2[x][y]!= 0){
        Keyboard.release(mask2[x][y]);
      }
      Keyboard.release(key[x][y]);
    }
  }
  
}


unsigned char color_r(unsigned int count){
  if(count >= 0 && count < RES/3){
    return (int)(255*count/(RES/3));
  }
  else if (count >=(RES/3) && count < (2*RES/3)){
    return (int)(510-(255*count/(RES/3)));
  }
  else if(count >= RES && count < (RES+(RES/3))){
    return (int)(255*(count-RES)/(RES/3));
  }
  else if (count >=RES+(RES/3) && count <RES+(2*RES/3)){
    return (int)(510-(255*(count-RES)/(RES/3)));
  }
  else{
    return 0;
  }
  
}

unsigned char color_g(unsigned int count){
  
  if (count >=(RES/3) && count <(2*RES/3)){
    return (int)((255*(count-(RES/3))/(RES/3)));
  }
  else if (count >=(2*RES/3) && count <=(RES)){
    return (int)(510-(255*(count-(RES/3))/(RES/3)));
  }
  else if (count >=RES + (RES/3) && count <RES +(2*RES/3)){
    return (int)((255*(count-(RES/3)-RES)/(RES/3)));
  }
  else if (count >= RES + (2*RES/3) && count <= RES+ (RES)){
    return (int)(510-(255*(count-(RES/3)-RES)/(RES/3)));
  }
  else{
    return 0;
  }
  
}

unsigned char color_b(unsigned int count){
  if(count >= 0 && count < (RES/3)){
    return (int)(255-(255*count/(RES/3)));
  }
  else if (count >=(2*RES/3) && count <(RES)){
    return (int)((255*(count-(2*RES/3))/(RES/3)));
  }
  else if(count >= RES && count < RES+(RES/3)){
    return (int)(255-(255*(count-RES)/(RES/3)));
  }
  else if (count >=RES +(2*RES/3) && count <2*(RES)){
    return (int)((255*(count-(2*RES/3)-RES)/(RES/3)));
  }
  
  else{
    return 0;
  }
  
}



Adafruit_NeoPixel led = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


//Lighting Wave Pattern
void key_wave(void){
  led.clear();
  static uint8_t bright[4][5] = {{50,50,50,50,50},
                                 {50,50,50,50,50},
                                 {50,50,50,50,50},                    
                                 {50,50,50,50,50}};
  static uint8_t spread[4][5] = {0};
  static uint8_t was_pressed[4][5] = {0};
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 5; j++){
      if(key_buffer[i][j]){
        if(spread[i][j] == 0){
          spread[i][j] = 50;
        }
        else if(was_pressed[i][j] == 0){
          spread[i][j] = 40;
        }
        was_pressed[i][j] = 1;
        
      }
      else{
        was_pressed[i][j] = 0;
      }
      if(spread[i][j] > 0){
        for(int h = 0; h<4; h++){
          for(int k = 0; k<5; k++){
            float dist = sqrt(abs(h-i) + abs(k-j));
            if(-(spread[i][j]-50)>=(dist*3)){
              bright[h][k] = (uint8_t)((bright[h][k] + 50 - spread[i][j]*2*sin((spread[i][j]-50)*0.4 + dist) + 100)/2);
            }
            else{
              bright[h][k] = 50;
            }
          }
        }
        spread[i][j]--;
      }
      
      led.setPixelColor(led_map_default[i][j],0,(uint8_t)((bright[i][j])*0.5),bright[i][j]);
    }
  }
  led.show();
}


//Show Single RGB Colour on the whole Keyboard
void RGB_Simple(uint8_t r, uint8_t g, uint8_t b){
  led.setBrightness(50);
  for(int i = 0; i< NUMPIXELS;++i){
    led.setPixelColor(led_map_diag[i],r,g,b);
    
  }
  led.show();
}

//Keys falsh up and fade after a short period of time
void key_flash_fade(void){
  static uint8_t bright[4][5] = {0};
  led.clear();
  for(int i = 0; i < 5; i++){
    for(int j = 0; j < 4; j++){
      if(key_buffer[i][j]){
        led.setPixelColor(led_map_default[i][j],255,255,255);
        bright[i][j] = 255;
        
      }
      else{
        if(bright[i][j]){
          led.setPixelColor(led_map_default[i][j],bright[i][j],bright[i][j],bright[i][j]);
          bright[i][j] -= 5;
        }
      }
      
    }
  }
  led.show();
}


//Keys light up while beeign held down
void key_flash(void){
  led.clear();
  for(int i = 0; i < 5; i++){
    for(int j = 0; j < 4; j++){
      if(key_buffer[i][j]){
        led.setPixelColor(led_map_default[i][j],255,255,255);
        
      }
      
    }
  }
  led.show();
}


//All keys have a slight colour fade across the Keyboard starting at Val
void RGB_Solid(uint16_t val){
  led.setBrightness(50);
  for(int i = 0; i< NUMPIXELS;++i){
    if(last_key != led_map_diag[i]){      
      const int HYS = 50;
      const int MIN = 0 + HYS;
      const int MAX = 255- HYS;
      int RED = color_r((val + i*phaseshift)%RES);
      if(RED<=MIN)
        RED = MIN;
      if(RED>=MAX)
        RED = MAX;
      int BLUE = color_b((val + i*phaseshift)%RES);
      if(BLUE<=MIN)
        BLUE = MIN;
      if(BLUE>=MAX)
        BLUE = MAX;
      int GREEN = color_g((val + i*phaseshift)%RES);
      if(GREEN<=MIN)
        GREEN = MIN;
      if(GREEN>=MAX)
        GREEN = MAX;
      led.setPixelColor(led_map_diag[i],RED,BLUE,GREEN);
    } else if(FADE){
      led.setPixelColor(led_map_diag[i],(int)((fade_out+color_r((val + i*phaseshift)%RES))/(2.0-(255-fade_out)/255)),(int)((fade_out+color_g((val + i*phaseshift)%RES))/(2.0-(255-fade_out)/255)),(int)((fade_out+color_b((val + i*phaseshift)%RES))/(2.0-(255-fade_out)/255)));

    } else {
      led.setPixelColor(led_map_diag[i],255,255,255);
    }
  }
  led.show();
}


//Fade between rows
void RGB_Solid_Rows(uint16_t val){
  led.setBrightness(50);
  for(int i = 0; i< 4;++i){
    led.setPixelColor(led_map_horz[(i*5)],color_r((val + i*phaseshift)%RES),color_g((val + i*phaseshift)%RES),color_b((val + i*phaseshift)%RES));
    led.setPixelColor(led_map_horz[(i*5)+1],color_r((val + i*phaseshift)%RES),color_g((val + i*phaseshift)%RES),color_b((val + i*phaseshift)%RES));
    led.setPixelColor(led_map_horz[(i*5)+2],color_r((val + i*phaseshift)%RES),color_g((val + i*phaseshift)%RES),color_b((val + i*phaseshift)%RES));
    led.setPixelColor(led_map_horz[(i*5)+3],color_r((val + i*phaseshift)%RES),color_g((val + i*phaseshift)%RES),color_b((val + i*phaseshift)%RES));
    led.setPixelColor(led_map_horz[(i*5)+4],color_r((val + i*phaseshift)%RES),color_g((val + i*phaseshift)%RES),color_b((val + i*phaseshift)%RES));
    
  }
  led.show();
}

//Fade between Cols
void RGB_Solid_Cols(uint16_t val){
  led.setBrightness(50);
  for(int i = 0; i< 5;++i){
    led.setPixelColor(led_map_horz[i],color_r((val + i*phaseshift)%RES),color_g((val + i*phaseshift)%RES),color_b((val + i*phaseshift)%RES));
    led.setPixelColor(led_map_horz[i+5],color_r((val + i*phaseshift)%RES),color_g((val + i*phaseshift)%RES),color_b((val + i*phaseshift)%RES));
    led.setPixelColor(led_map_horz[i+10],color_r((val + i*phaseshift)%RES),color_g((val + i*phaseshift)%RES),color_b((val + i*phaseshift)%RES));
    led.setPixelColor(led_map_horz[i+15],color_r((val + i*phaseshift)%RES),color_g((val + i*phaseshift)%RES),color_b((val + i*phaseshift)%RES)); 
  }
  led.show();
}

void RGB_Fade_Diag(void){
  led.setBrightness(50);
  for(int i = 0; i< NUMPIXELS;++i){
    led.setPixelColor(led_map_diag[i],color_r((countges + i*phaseshift)%RES),color_g((countges + i*phaseshift)%RES),color_b((countges + i*phaseshift)%RES));
  }
  led.show();
  if (countges < RES){
    countges ++;
  }
  else{
    countges = 0;
  }
}

void setup() {
  //Initializing all I/Os
  led.clear();
  led.begin();
  led.setBrightness(50);
  pinMode(TR1, OUTPUT);
  pinMode(TR2, OUTPUT);
  pinMode(TR3, OUTPUT);
  pinMode(TR4, OUTPUT);
  pinMode(TS1, INPUT);
  pinMode(TS2, INPUT);
  pinMode(TS3, INPUT);
  pinMode(TS4, INPUT);
  pinMode(TS5, INPUT);
  digitalWrite(TR1, LOW);
  digitalWrite(TR2, LOW);
  digitalWrite(TR3, LOW);
  digitalWrite(TR4, LOW);
  led.show();
  Keyboard.begin();
  delay(10);
  RGB_Solid(600);
  
  
}

//Main loop
uint16_t leds;
void loop() {
    


  get_keys();
 
  RGB_Solid(leds++);
  if(FADE){
    if(fade_out <= 0){
      last_key = -1;
    }
    else{
      fade_out--;
    }
  }
  
  

}
