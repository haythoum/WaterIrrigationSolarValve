#include "Controller.h"

Controller controller;

void setup() {
  Serial.begin(115200);
  delay(2000);   // give USB-serial time to enumerate before the first prints
  controller.begin();
}

void loop() {
  controller.run();
}
