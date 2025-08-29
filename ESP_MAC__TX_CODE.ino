#include <ESP8266WiFi.h>
#include <espnow.h>
//C8:C9:A3:56:4A:FD


uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0x56, 0x4A, 0xFD};
//F4:CF:A2:C7:29:D2
// Define the switch pin
#define sw1 2

// Data structure for sending and receiving
typedef struct struct_message {
    char data[250];  // Buffer for incoming data
} struct_message;

struct_message sendReadings;
struct_message incomingReadings;

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Received data: ");
  Serial.println(incomingReadings.data);
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_register_recv_cb(OnDataRecv);

  pinMode(sw1, INPUT);
}

void loop() {
  static String inputString = "";  // String to hold incoming data
  
  // Read from Serial and accumulate characters until "\n" is encountered
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;

    // Check if the end marker "\n" is encountered
    if (inChar == '\n') {
      // Copy the accumulated string to the send buffer
      inputString.trim(); // Remove any extraneous whitespace/newlines
      inputString.toCharArray(sendReadings.data, sizeof(sendReadings.data));
      
      // Send data via ESP-NOW
      esp_now_send(broadcastAddress, (uint8_t *) &sendReadings, sizeof(sendReadings));

      // Print sent data for debugging
      Serial.print("Sent: ");
      Serial.println(sendReadings.data);

      // Clear the input string for the next message
      inputString = "";
    }
  }
}
