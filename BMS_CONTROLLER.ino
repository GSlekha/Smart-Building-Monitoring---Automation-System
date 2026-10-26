#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>
#include <math.h>

#define SDA_PIN 21
#define SCL_PIN 22

#define FAN_PIN 25     // Relay 1 - Fan
#define LIGHT_PIN 26   // Relay 2 - Light

// Active-low relay module
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// Control thresholds
#define FAN_ON_TEMP   28.0
#define FAN_OFF_TEMP  26.0

#define LIGHT_ON_LUX  20.0
#define LIGHT_OFF_LUX 40.0

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Adafruit_BME280 bme;
BH1750 lightMeter;

bool fanOn = false;
bool lightOn = false;
bool oledOk = false;

bool bmeOk = false;
bool luxSensorOk = false;

void setup() {
  // Keep relays OFF during startup
  digitalWrite(FAN_PIN, RELAY_OFF);
  digitalWrite(LIGHT_PIN, RELAY_OFF);

  pinMode(FAN_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);

  digitalWrite(FAN_PIN, RELAY_OFF);
  digitalWrite(LIGHT_PIN, RELAY_OFF);

  Serial.begin(115200);
  delay(500);

  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize OLED
  oledOk = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  if (!oledOk) {
    Serial.println("OLED not found");
  }

  // Initialize sensors
  bmeOk = bme.begin(0x76, &Wire);
  luxSensorOk = lightMeter.begin();

  if (!bmeOk) {
    Serial.println("FAULT: BME280 not found");
  }

  if (!luxSensorOk) {
    Serial.println("FAULT: BH1750 not found");
  }

  if (oledOk) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    display.println("Smart Building BMS");
    display.println("Starting...");
    display.display();
  }

  delay(1500);
}

void loop() {
  // Read sensors
  float temp = NAN;
  float hum = NAN;
  float pres = NAN;
  float lux = NAN;

  if (bmeOk) {
    temp = bme.readTemperature();
    hum = bme.readHumidity();
    pres = bme.readPressure() / 100.0F;
  }

  if (luxSensorOk) {
    lux = lightMeter.readLightLevel();
  }

  // Detect invalid sensor readings
  bool bmeFault =
      !bmeOk ||
      !isfinite(temp) ||
      !isfinite(hum) ||
      !isfinite(pres) ||
      temp < -40.0 || temp > 85.0 ||
      hum < 0.0 || hum > 100.0 ||
      pres <= 0.0;

  bool luxFault =
      !luxSensorOk ||
      !isfinite(lux) ||
      lux < 0.0;

  // FAN CONTROL WITH FAULT HANDLING
  if (bmeFault) {
    fanOn = false;  // Safe default for this prototype
  } else {
    if (temp > FAN_ON_TEMP) {
      fanOn = true;
    } else if (temp < FAN_OFF_TEMP) {
      fanOn = false;
    }
    // Between 26 and 28 C, retain previous fan state
  }

  // LIGHT CONTROL WITH FAULT HANDLING
  if (luxFault) {
    lightOn = false;  // Safe default for this prototype
  } else {
    if (lux < LIGHT_ON_LUX) {
      lightOn = true;
    } else if (lux > LIGHT_OFF_LUX) {
      lightOn = false;
    }
    // Between 20 and 40 lux, retain previous light state
  }

  // Apply relay commands
  digitalWrite(FAN_PIN, fanOn ? RELAY_ON : RELAY_OFF);
  digitalWrite(LIGHT_PIN, lightOn ? RELAY_ON : RELAY_OFF);

  // SERIAL MONITOR
  Serial.println();
  Serial.println("------------- BMS STATUS -------------");

  Serial.printf("Temperature: %.1f C\n", temp);
  Serial.printf("Humidity: %.1f %%\n", hum);
  Serial.printf("Pressure: %.1f hPa\n", pres);
  Serial.printf("Lux: %.1f lux\n", lux);

  Serial.printf("Fan command: %s\n", fanOn ? "ON" : "OFF");
  Serial.printf("Light command: %s\n", lightOn ? "ON" : "OFF");

  Serial.print("BME280 Status: ");
  Serial.println(bmeFault ? "SENSOR FAULT" : "OK");

  Serial.print("BH1750 Status: ");
  Serial.println(luxFault ? "SENSOR FAULT" : "OK");

  Serial.println("--------------------------------------");

  // OLED DISPLAY
  if (oledOk) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    display.printf("Temp : %.1f C\n", temp);
    display.printf("Hum  : %.1f %%\n", hum);
    display.printf("Pres : %.1f hPa\n", pres);
    display.printf("Lux  : %.1f\n", lux);
    display.printf("Fan  : %s\n", fanOn ? "ON" : "OFF");
    display.printf("Light: %s\n", lightOn ? "ON" : "OFF");

    display.setCursor(0, 56);

    if (bmeFault || luxFault) {
      display.print("FAULT: ");

      if (bmeFault) {
        display.print("BME ");
      }

      if (luxFault) {
        display.print("LIGHT");
      }
    } else {
      display.print("Sensors: OK");
    }

    display.display();
  }

  delay(1000);
}
