#include <ESP8266WiFi.h>
#include <espnow.h>
//24:D7:EB:EF:BC:7E
uint8_t broadcastAddress[] = {0x24, 0xD7, 0xEB, 0xEF, 0xBC, 0x7E};
uint8_t this_mac[6] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11}; // Local MAC address
String receivedMessage;

// Adjust the size of this buffer as needed
#define MAX_MESSAGE_LENGTH 250

typedef struct struct_message {
  char data[MAX_MESSAGE_LENGTH];
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
  receivedMessage = String(incomingReadings.data);
  Serial.println( receivedMessage);
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  wifi_set_macaddr(0, const_cast<uint8_t*>(this_mac));
  delay(500);
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Sending data (optional, modify as needed)
  // esp_now_send(broadcastAddress, (uint8_t *) &sendReadings, sizeof(sendReadings));
  delay(50);
}
