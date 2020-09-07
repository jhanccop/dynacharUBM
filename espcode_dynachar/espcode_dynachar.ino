#include <Average.h>

#include "Wire.h"
#include <MPU6050_light.h>

#define BUTTON_PIN_BITMASK 0x200000000 // 2^39 in hex

MPU6050 mpu(Wire);
unsigned long#include <Average.h>

#include "Wire.h"
#include <MPU6050_light.h>

#define BUTTON_PIN_BITMASK 0x200000000 // 2^39 in hex

MPU6050 mpu(Wire);
unsigned long timer = 0;
int rate = 20; // time for get data for sensors
int sample = 500; // numbers of points

// stroke < (rate + sample)*1.2

int count_stroke = 0;
int stroke_for_stop = 3;
int batch = 10; //

RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : {
        Serial.println("Wakeup caused by external signal using RTC_IO");
        delay(2000);
        break;
      }
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : {
        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
        setup_sensor();
        break;
      }
  }
}

void setup_sensor() {
  Wire.begin();
  mpu.begin();
  Serial.println(F("Calculating gyro offset, do not move MPU6050"));
  delay(1000);
  mpu.calcGyroOffsets();
  Serial.println("Done!\n");
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  print_wakeup_reason();
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 0); //1 = High, 0 = Low

}

void get_data() {
  int minat = 0;
  int maxat = 0;

  Average<float> angle(sample);
  Average<float> acc(sample);

  //obteniendo datos de aceleración e inclinación
  for (int i = 0; i < sample; i++) {
    mpu.update();
    int temp = round(mpu.getAngleX() * 10);
    angle.push(temp * 0.1);
    acc.push(analogRead(A0));
    delay(rate);
  }

  // determinar el estado de la unidad: reposo o actividad
  float angle_diff = angle.maximum(&maxat) - angle.minimum(&minat);
  if (angle_diff < abs(2.0)) {
    count_stroke += 1;
    if (stroke_for_stop == count_stroke) {
      count_stroke = 0;
      Serial.println("Device sleep mode");
      esp_deep_sleep_start();
    }
    Serial.println("UBM stopped!!");
  } else {
    int counter = 0;
    float last_ang_batch = angle.get(0);
    float last_ang = angle.get(0);
    float act_ang = 0;
    float min_ang = 90;
    int index_min = 0;
    int index_min_all[3] = {0, 0, 0};
    String dir = ""; // 0: bajada  1: subida
    float pen = 0;

    for (int i = 1; i < sample + 1; i++) {

      if (counter >= batch) {
        act_ang = angle.get(i);
        pen = act_ang - last_ang_batch;
        Serial.println(pen);
        if ( pen < -2 ) {
          dir = "down";
        } else if (pen >= -2 && pen <= 2 && dir == "down") {
          count_stroke = 0;
          dir = "transition";
          Serial.println(dir);
          if (index_min_all[0] == 0)
            index_min_all[0] = index_min;
          else {
            index_min_all[1] = index_min;
          }
        } else {
          dir = "up";
        }

        last_ang = act_ang;
        counter = 0;
      } else {
        act_ang = angle.get(i);
        if (act_ang <= last_ang) {
          index_min = i;
          min_ang = act_ang;
        }
        act_ang = last_ang;
      }
      counter++;

      //Serial.print(angle.get(i));
      //Serial.print(" ");
    }

    int diff_index = index_min_all[1] - index_min_all[0];
    if (diff_index > 30) {
      Serial.println("******************** CHART OK ************************");
      Serial.print("min 1: ");
      Serial.print(index_min_all[0]);
      Serial.print("    min 2: ");
      Serial.println(index_min_all[1]);


    } else {
      Serial.println("XXXXXXXXXXXXXXXXXX CHART FAIL XXXXXXXXXXXXXXXXXXXXXXXXXX");
    }

    /*
      for (int i = index_min_all[0]; i < index_min_all[1] + 1; i++) {
      Serial.print(angle.get(i));
      Serial.print(" ");
      }
      Serial.println();
      for (int i = index_min_all[0]; i < index_min_all[1] + 1; i++) {
      Serial.print(acc.get(i));
      Serial.print(" ");
      }
      Serial.println();*/
  }
}

void loop() {
  get_data();

}
 timer = 0;
int rate = 20; // time for get data for sensors
int sample = 500; // numbers of points

// stroke < (rate + sample)*1.2

int count_stroke = 0;
int stroke_for_stop = 3;
int batch = 10; //

RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : {
        Serial.println("Wakeup caused by external signal using RTC_IO");
        delay(2000);
        break;
      }
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : {
        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
        setup_sensor();
        break;
      }
  }
}

void setup_sensor() {
  Wire.begin();
  mpu.begin();
  Serial.println(F("Calculating gyro offset, do not move MPU6050"));
  delay(1000);
  mpu.calcGyroOffsets();
  Serial.println("Done!\n");
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  print_wakeup_reason();
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 0); //1 = High, 0 = Low

}

void get_data() {
  int minat = 0;
  int maxat = 0;

  Average<float> angle(sample);
  Average<float> acc(sample);

  //obteniendo datos de aceleración e inclinación
  for (int i = 0; i < sample; i++) {
    mpu.update();
    int temp = round(mpu.getAngleX() * 10);
    angle.push(temp * 0.1);
    acc.push(analogRead(A0));
    delay(rate);
  }

  // determinar el estado de la unidad: reposo o actividad
  float angle_diff = angle.maximum(&maxat) - angle.minimum(&minat);
  if (angle_diff < abs(2.0)) {
    count_stroke += 1;
    if (stroke_for_stop == count_stroke) {
      count_stroke = 0;
      Serial.println("Device sleep mode");
      esp_deep_sleep_start();
    }
    Serial.println("UBM stopped!!");
  } else {
    int counter = 0;
    float last_ang_batch = angle.get(0);
    float last_ang = angle.get(0);
    float act_ang = 0;
    float min_ang = 90;
    int index_min = 0;
    int index_min_all[3] = {0, 0, 0};
    String dir = ""; // 0: bajada  1: subida
    float pen = 0;

    for (int i = 1; i < sample + 1; i++) {

      if (counter >= batch) {
        act_ang = angle.get(i);
        pen = act_ang - last_ang_batch;
        Serial.println(pen);
        if ( pen < -2 ) {
          dir = "down";
        } else if (pen >= -2 && pen <= 2 && dir == "down") {
          count_stroke = 0;
          dir = "transition";
          Serial.println(dir);
          if (index_min_all[0] == 0)
            index_min_all[0] = index_min;
          else {
            index_min_all[1] = index_min;
          }
        } else {
          dir = "up";
        }

        last_ang = act_ang;
        counter = 0;
      } else {
        act_ang = angle.get(i);
        if (act_ang <= last_ang) {
          index_min = i;
          min_ang = act_ang;
        }
        act_ang = last_ang;
      }
      counter++;

      //Serial.print(angle.get(i));
      //Serial.print(" ");
    }

    int diff_index = index_min_all[1] - index_min_all[0];
    if (diff_index > 30) {
      Serial.println("******************** CHART OK ************************");
      Serial.print("min 1: ");
      Serial.print(index_min_all[0]);
      Serial.print("    min 2: ");
      Serial.println(index_min_all[1]);


    } else {
      Serial.println("XXXXXXXXXXXXXXXXXX CHART FAIL XXXXXXXXXXXXXXXXXXXXXXXXXX");
    }

    /*
      for (int i = index_min_all[0]; i < index_min_all[1] + 1; i++) {
      Serial.print(angle.get(i));
      Serial.print(" ");
      }
      Serial.println();
      for (int i = index_min_all[0]; i < index_min_all[1] + 1; i++) {
      Serial.print(acc.get(i));
      Serial.print(" ");
      }
      Serial.println();*/
  }
}

void loop() {
  get_data();

}
