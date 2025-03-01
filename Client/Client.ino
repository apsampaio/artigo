#include <WiFi.h>
#include "esp_camera.h"

#define BOUNDARY      "--------------------------FATECSOROCABA"
#define IP            "YOUR_LOCAL_IP"

#define FLASH 4
#define BTN 2

char* ssid = "YOUR_SSID";
char* password = "YOUR_PASSWORD";

WiFiClient client;

// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
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

void takePicture ( void ) ;

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(BTN, INPUT);
  pinMode(FLASH, OUTPUT);

  WiFi.begin(ssid,password);
  Serial.print("Connecting..");
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected!")
  
  // OV2640 camera module pinout configuration
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

  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }
  
  // Camera image configuration
  sensor_t * s = esp_camera_sensor_get();
  s->set_brightness(s, 0);     // -2 to 2
  s->set_contrast(s, 0);       // -2 to 2
  s->set_saturation(s, 0);     // -2 to 2
  s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
  s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
  s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
  s->set_aec2(s, 0);           // 0 = disable , 1 = enable
  s->set_ae_level(s, 0);       // -2 to 2
  s->set_aec_value(s, 300);    // 0 to 1200
  s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
  s->set_agc_gain(s, 0);       // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
  s->set_bpc(s, 0);            // 0 = disable , 1 = enable
  s->set_wpc(s, 1);            // 0 = disable , 1 = enable
  s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
  s->set_lenc(s, 1);           // 0 = disable , 1 = enable
  s->set_hmirror(s, 1);        // 0 = disable , 1 = enable
  s->set_vflip(s, 1);          // 0 = disable , 1 = enable
  s->set_dcw(s, 1);            // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);       // 0 = disable , 1 = enable
  s->set_framesize(s, FRAMESIZE_UXGA);
}

void loop() {

  if(digitalRead(FLASH)){
    digitalWrite(FLASH, LOW);
  }

  if(!digitalRead(BTN)) {
    while(!digitalRead(BTN)) delay(20);
    digitalWrite(FLASH, HIGH);
    delay(20);
    takePicture();
  }
  delay(500);
}

void takePicture(void) {
  camera_fb_t * pic = NULL;
  pic = esp_camera_fb_get();
  
  if (!pic) {
    Serial.println("Camera capture failed");
    return;
  }
  Serial.println("Camera capture worked!");
  
  if (client.connect(IP, 3333)) {
    Serial.println("Client connected!");
    
    String bodyStart = makeBody();
    String bodyEnd = makeBodyEnd();
    size_t requestLength = bodyStart.length() + bodyEnd.length() + pic->len;
    String header = makeHeader(requestLength);

    client.print(header + bodyStart);
    client.write((const char *)pic->buf, pic->len);
    client.print("\r\n" + bodyEnd);
  
    long tOut = millis() + 2000;
     while(client.connected() && tOut > millis())
     {
      if (client.available())
      {
        String serverRes = client.readStringUntil('\r');
        Serial.println(serverRes);
      }
     }          
    client.stop(); 
    
  }
  esp_camera_fb_return(pic);
}

String makeBody() {
  String data;
  
  data = "--";
  data += BOUNDARY;
  data += F("\r\n");
  
  data += F("Content-Disposition: form-data; name=\"placa\"; filename=\"placa.jpeg\"\r\n");
  data += F("Content-Type: image/jpeg\r\n");
  data += F("\r\n");

  return(data);
}

String makeHeader(size_t length) {
  String data;
    data = F("POST /placa HTTP/1.1\r\n");
    data += F("Content-Type: multipart/form-data; boundary=");
    data += BOUNDARY;
    data += F("\r\n");
    data += F("Host: ");
    data += IP;
    data += F("3333\r\n");
    data += ("Content-Length: ");
    data += String(length);
    data += F("\r\n");
    data += F("\r\n");
  return(data);
}

String makeBodyEnd() {
    String data;
    data = String("--") + BOUNDARY + String("--\r\n");
    return data;
}
