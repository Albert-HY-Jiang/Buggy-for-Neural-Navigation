#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory
#include "BluetoothSerial.h"   //bluetooth
// define the number of bytes you want to access
#define EEPROM_SIZE 1
 
RTC_DATA_ATTR int bootCount = 0;  

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

//Example SerialtoSerialBT
// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

#define CHUNK_SIZE (size_t)32

BluetoothSerial SerialBT;
String device_name = "ESP32_CAM_BT";
  
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(9600);   //The MSP432 will be communicating through default serial port (GPIO 1 TX, GPIO 3 RX)
  cam_init();
  bt_init();
}

void loop() {
  String read;
  if(Serial.available())
  {
    //read time stamp and distance from MSP432
    read = Serial.readString();
    
    /*
    * structure of bytes stream transferred
    * transmission started\n
    * time stamp, distance\n
    * file size(bytes)\n
    * file bytes
    */

    SerialBT.printf("transmission started\n");
    SerialBT.print(read);
    take_picture();

    //tell MSP432 the cam is ready again
    Serial.print("r\n");
  }
}

void cam_init()
{
  Serial.setDebugOutput(false);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
 
  //ESP32 cam will most likelt have psram, so it will be using UXGA
  if(psramFound()){
    Serial.print("Using UXGA\n");
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;    //it can holds two frame buffer at the same time
  } else {
    Serial.print("Using SVGA\n");
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

    // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void bt_init()
{
  SerialBT.begin(device_name);
  SerialBT.register_callback(btCallback);
  //Serial.println("The device started, now you can pair it with bluetooth");
}

void btCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  //Serial Port Profile (SPP) API
  if(event == ESP_SPP_SRV_OPEN_EVT){
    //this event corresponds to establishing bluetooth connection
    //The "go" triggers MSP432 to start moving
    Serial.println("go");
  }
}
void take_picture()
{
  camera_fb_t * fb = NULL;
  
  // Take Picture with Camera
  pinMode(4, OUTPUT);   //GPIO 4 is the LED flashlight
  digitalWrite(4, HIGH);
  fb = esp_camera_fb_get();  
  esp_camera_fb_return(fb); //need to free frame buffer to prevent memory overflow
  //Taking the picture twice allows the camera to adjust itself. It usually works better under brighter environment
  delay(1000);
  fb = esp_camera_fb_get();
  
  digitalWrite(4, LOW);
  if(!fb) {
    //notify each side the capture failed
    Serial.println("Camera capture failed");
    SerialBT.println("Camera capture failed");
    return; 
  }  


  SerialBT.printf("%u\n",fb->len);
  SerialBT.flush();
  delay(10);
  SerialBT.write(fb->buf, fb->len);
  SerialBT.flush(); //make sure to transmit the whole file
  esp_camera_fb_return(fb);

  
} 