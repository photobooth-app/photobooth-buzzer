/*
 * photobooth buzzer.
 * emulates a keyboard to trigger captures
 *
 * Copyright (c) 2023 Michael G
 *
 * Licensed under MIT License
 * https://opensource.org/licenses/MIT
 */

//
// This program lets an ESP32 act as a keyboard connected via Bluetooth.
// When a button attached to the ESP32 is pressed, it will generate key strokes.
//

#define US_KEYBOARD 1

#include <Arduino.h>
#include "BLEDevice.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"
#include "Button2.h"
#include "Adafruit_MAX1704X.h"

// Change the below values if desired
#define BUTTON_GPIO 12
#define HID_CHAR_TAKE_PICTURE "i"
#define DEVICE_NAME "photobooth-app buzzer"

// Forward declarations
void setupBluetooth();
void setupButtons();
void typeText(const char *text);
void handleTap(Button2 &b);

// global variables
BLEHIDDevice *hid;
Adafruit_MAX17048 maxlipo;
Button2 btnBuzzer;
bool isBleConnected = false;

void Task_Fuelgauge(void *pvParameters)
{
  (void)pvParameters;

  /* init code*/

  Serial.println(F("\nInit MAX17048"));

  delay(100);
  if (!maxlipo.begin())
  {
    Serial.println(F("Couldnt find MAX17048?\nMake sure a battery is plugged in! Continue without Max"));
  }
  else
  {
    Serial.print(F("Found MAX17048"));
    Serial.print(F(" with Chip ID: 0x"));
    Serial.println(maxlipo.getChipID(), HEX);
  }

  while (1) // A Task shall never return or exit.
  {

    float cellVoltage = maxlipo.cellVoltage();
    if (isnan(cellVoltage))
    {
      Serial.println("Failed to read cell voltage, check battery is connected!");
    }
    else
    {
      Serial.print(F("Batt Voltage: "));
      Serial.print(cellVoltage, 3);
      Serial.print(" V, ");
      Serial.print(F("Batt Percent: "));
      Serial.print(maxlipo.cellPercent(), 1);
      Serial.println(" %");
      hid->setBatteryLevel(static_cast<uint8_t>(maxlipo.cellPercent() + 0.5));
    }

    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  // reduced power usage
  setCpuFrequencyMhz(80);

  // start serial
  Serial.begin(115200);

  // setup buttons
  setupButtons();

  // start Bluetooth and start advertising
  setupBluetooth();

  // setup fuel gauge
  xTaskCreatePinnedToCore(Task_Fuelgauge, "Task_Fuelgauge", 4096, NULL, 3, NULL, ARDUINO_RUNNING_CORE);

  delay(1000);
}

void loop()
{
  btnBuzzer.loop();
}

void handleTap(Button2 &b)
{
  if (isBleConnected)
  {
    // button has been pressed: type message
    Serial.println(HID_CHAR_TAKE_PICTURE);
    typeText(HID_CHAR_TAKE_PICTURE);
  }
  else
    Serial.println("not connected, button press ignored");
}

// Message (report) sent when a key is pressed or released
struct InputReport
{
  uint8_t modifiers;      // bitmask: CTRL = 1, SHIFT = 2, ALT = 4
  uint8_t reserved;       // must be 0
  uint8_t pressedKeys[6]; // up to six concurrenlty pressed keys
};

// Message (report) received when an LED's state changed
struct OutputReport
{
  uint8_t leds; // bitmask: num lock = 1, caps lock = 2, scroll lock = 4, compose = 8, kana = 16
};

// The report map describes the HID device (a keyboard in this case) and
// the messages (reports in HID terms) sent and received.
static const uint8_t REPORT_MAP[] = {
    USAGE_PAGE(1), 0x01,      // Generic Desktop Controls
    USAGE(1), 0x06,           // Keyboard
    COLLECTION(1), 0x01,      // Application
    REPORT_ID(1), 0x01,       //   Report ID (1)
    USAGE_PAGE(1), 0x07,      //   Keyboard/Keypad
    USAGE_MINIMUM(1), 0xE0,   //   Keyboard Left Control
    USAGE_MAXIMUM(1), 0xE7,   //   Keyboard Right Control
    LOGICAL_MINIMUM(1), 0x00, //   Each bit is either 0 or 1
    LOGICAL_MAXIMUM(1), 0x01,
    REPORT_COUNT(1), 0x08, //   8 bits for the modifier keys
    REPORT_SIZE(1), 0x01,
    HIDINPUT(1), 0x02,     //   Data, Var, Abs
    REPORT_COUNT(1), 0x01, //   1 byte (unused)
    REPORT_SIZE(1), 0x08,
    HIDINPUT(1), 0x01,     //   Const, Array, Abs
    REPORT_COUNT(1), 0x06, //   6 bytes (for up to 6 concurrently pressed keys)
    REPORT_SIZE(1), 0x08,
    LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x65, //   101 keys
    USAGE_MINIMUM(1), 0x00,
    USAGE_MAXIMUM(1), 0x65,
    HIDINPUT(1), 0x00,     //   Data, Array, Abs
    REPORT_COUNT(1), 0x05, //   5 bits (Num lock, Caps lock, Scroll lock, Compose, Kana)
    REPORT_SIZE(1), 0x01,
    USAGE_PAGE(1), 0x08,    //   LEDs
    USAGE_MINIMUM(1), 0x01, //   Num Lock
    USAGE_MAXIMUM(1), 0x05, //   Kana
    LOGICAL_MINIMUM(1), 0x00,
    LOGICAL_MAXIMUM(1), 0x01,
    HIDOUTPUT(1), 0x02,    //   Data, Var, Abs
    REPORT_COUNT(1), 0x01, //   3 bits (Padding)
    REPORT_SIZE(1), 0x03,
    HIDOUTPUT(1), 0x01, //   Const, Array, Abs
    END_COLLECTION(0)   // End application collection
};

BLECharacteristic *input;
BLECharacteristic *output;

const InputReport NO_KEY_PRESSED = {};

/*
 * Callbacks related to BLE connection
 */
class BleKeyboardCallbacks : public BLEServerCallbacks
{

  void onConnect(BLEServer *server)
  {
    isBleConnected = true;

    // Allow notifications for characteristics
    BLE2902 *cccDesc = (BLE2902 *)input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    cccDesc->setNotifications(true);

    Serial.println("Client has connected");
  }

  void onDisconnect(BLEServer *server)
  {
    isBleConnected = false;

    // Disallow notifications for characteristics
    BLE2902 *cccDesc = (BLE2902 *)input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    cccDesc->setNotifications(false);

    Serial.println("Client has disconnected");

    // restart advertising: https://github.com/espressif/arduino-esp32/issues/6016
    server->getAdvertising()->start();
  }
};

/*
 * Called when the client (computer, smart phone) wants to turn on or off
 * the LEDs in the keyboard.
 *
 * bit 0 - NUM LOCK
 * bit 1 - CAPS LOCK
 * bit 2 - SCROLL LOCK
 */
class OutputCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *characteristic)
  {
    OutputReport *report = (OutputReport *)characteristic->getData();
    Serial.print("LED state: ");
    Serial.print((int)report->leds);
    Serial.println();
  }
};

void setupButtons()
{
  // configure pin for button
  btnBuzzer.begin(BUTTON_GPIO, INPUT_PULLUP);

  // setTapHandler() is called by any type of click, longpress or shortpress
  btnBuzzer.setTapHandler(handleTap);

  Serial.println(" Longpress Time:\t" + String(btnBuzzer.getLongClickTime()) + "ms");
  Serial.println(" DoubleClick Time:\t" + String(btnBuzzer.getDoubleClickTime()) + "ms");
  Serial.println();
}

void setupBluetooth()
{

  // initialize the device
  BLEDevice::init(DEVICE_NAME);
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new BleKeyboardCallbacks());

  // create an HID device
  hid = new BLEHIDDevice(server);
  input = hid->inputReport(1);   // report ID
  output = hid->outputReport(1); // report ID
  output->setCallbacks(new OutputCallbacks());

  // set manufacturer name
  hid->manufacturer()->setValue("mgrl");
  // set USB vendor and product ID
  hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
  // information about HID device: device is not localized, device can be connected
  hid->hidInfo(0x00, 0x02);

  // Security: device requires bonding
  BLESecurity *security = new BLESecurity();
  security->setAuthenticationMode(ESP_LE_AUTH_BOND);

  // set report map
  hid->reportMap((uint8_t *)REPORT_MAP, sizeof(REPORT_MAP));
  hid->startServices();

  // set battery level to 100%
  hid->setBatteryLevel(0);

  // advertise the services
  BLEAdvertising *advertising = server->getAdvertising();
  advertising->setAppearance(HID_KEYBOARD);
  advertising->addServiceUUID(hid->hidService()->getUUID());
  advertising->addServiceUUID(hid->deviceInfo()->getUUID());
  advertising->addServiceUUID(hid->batteryService()->getUUID());
  advertising->start();

  Serial.println("BLE ready");
  // delay(portMAX_DELAY);
};

void typeText(const char *text)
{
  int len = strlen(text);
  for (int i = 0; i < len; i++)
  {

    // translate character to key combination
    uint8_t val = (uint8_t)text[i];
    if (val > KEYMAP_SIZE)
      continue; // character not available on keyboard - skip
    KEYMAP map = keymap[val];

    // create input report
    InputReport report = {
        .modifiers = map.modifier,
        .reserved = 0,
        .pressedKeys = {
            map.usage,
            0, 0, 0, 0, 0}};

    // send the input report
    input->setValue((uint8_t *)&report, sizeof(report));
    input->notify();

    delay(5);

    // release all keys between two characters; otherwise two identical
    // consecutive characters are treated as just one key press
    input->setValue((uint8_t *)&NO_KEY_PRESSED, sizeof(NO_KEY_PRESSED));
    input->notify();

    delay(5);
  }
}