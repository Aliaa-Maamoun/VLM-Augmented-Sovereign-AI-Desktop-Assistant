#include <WiFi.h>
#include <HTTPClient.h>
#include <driver/i2s.h>

// ====================================================================
// 1. CONFIGURATION BLOCK (Adjust these variables)
// ====================================================================
const char* ssid     = "Mohamed’s iPhone";      // Exact name of your phone hotspot
const char* password = "123456789";  // Exact hotspot password
const char* serverUrl = "http://172.20.10.2:5000/audio"; // REPLACE 'X' WITH YOUR LIVE LAPTOP IP!

// Verified hardware pins for your specific board layout
#define I2S_SD      10  // Serial Data / DOUT
#define I2S_WS      11  // Word Select / LRC
#define I2S_BCLK    12  // Bit Clock / BCLK
#define I2S_PORT    I2S_NUM_0

// Memory buffer allocation
#define BUFFER_SIZE 4000 
uint8_t audioBuffer[BUFFER_SIZE];

// ====================================================================
// 2. HARDWARE INITIALIZATION
// ====================================================================
void setupI2S() {

  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    
    .sample_rate = 8000, 
    
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 512,
    .use_apll = false
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
}

void setup() {
  Serial.begin(115200);
  
  // Clear any existing connections
  WiFi.disconnect();
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.print("[Wi-Fi System] Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[Wi-Fi System] Link Established Successfully!");
  
  setupI2S();
  Serial.println("[Hardware System] I2S Microphone Pipeline Online.");
}

// ====================================================================
// 3. THE RUNTIME INGESTION LOOP
// ====================================================================
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    size_t bytesRead = 0;
    
    // Read raw data from the microphone
    i2s_read(I2S_PORT, &audioBuffer, BUFFER_SIZE, &bytesRead, portMAX_DELAY);

    if (bytesRead > 0) {
      // --- LIVE DATA PRINTER ---
      // We will print out a sample byte so we can see if numbers change when you make noise!
      Serial.print("Mic Output Value: ");
      Serial.println(audioBuffer[0]); 

      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/octet-stream");
      int httpResponseCode = http.POST(audioBuffer, bytesRead);
      http.end();
    }
  } else {
    WiFi.begin(ssid, password);
    delay(2000);
  }
}