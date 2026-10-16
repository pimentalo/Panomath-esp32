/*
  ESP32 ASCII Art - Connections Guide (DevKit v1)

                     +--------------------+
              V+ >  -| 3V3            VIN |- 
                    -| GND            GND |- 
                    -| GPIO15      GPIO13 |-
                    -| GPIO2       GPIO12 |-                                   
               LED  -| GPIO4       GPIO14 |-                                   
                    -| GPIO16      GPIO27 |-
                    -| GPIO17      GPIO26 |-
                    -| GPIO5       GPIO25 |-
                    -| GPIO18      GPIO33 |- 
                    -| GPIO19      GPIO32 |- 
                    -| GPIO21      GPIO35 |-               (input only)
                    -| GPIO3/RX0   GPIO34 |-               (input only)
                    -| GPIO1/TX0   GPIO39 |-               (input only)
                    -| GPIO22      GPIO36 |-               (input only)
                    -| GPIO23          EN |-
                     +--------------------+

*/

#define GPIO4  4
#define GPIO12 26
#define GPIO14 27
//#define GPIO18 20
//#define GPIO19 21
//#define GPIO21 12
#define GPIO18 18
#define GPIO19 19
#define GPIO21 21
#define GPIO22 22
#define GPIO23 23
#define GPIO34 32
#define GPIO35 31
#define GPIO36 34
#define GPIO39 33



#define PIN_LED_MATRIX   GPIO4 // 

#define PIN_MIC_DOUT     GPIO21 // I2S SD
#define PIN_MIC_LRCLK    GPIO23 //GPIO19 // I2S WS
#define PIN_MIC_BCLK     GPIO22 // GPIO18 // I2S CLK
#define PIN_HP           GPIO20 // PWM

#define PIN_PAD_FNCT	GPIO35 // Touch 5
#define PIN_PAD_PLUS     GPIO34 // Touch 6
#define PIN_PAD_MINUS    GPIO39 // Touch 7
#define PIN_PAD_ENTER	GPIO36 // Touch 8
#define PIN_PAD_WIFI	GPIO12 //	Touch 9


