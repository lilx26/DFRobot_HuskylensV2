/***************************************************
 HUSKYLENS V2 An Easy-to-use AI Machine Vision Sensor
 <https://www.dfrobot.com/product-1922.html>

 ***************************************************
 This example shows the basic function of library for HUSKYLENS V2 via I2c.

 Created 2025-07-04
 By [Ouki Wang](ouki.wang@dfrobot.com)

 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/***********Notice and Trouble shooting***************
 1.Connection and Diagram can be found here
 <https://wiki.dfrobot.com/HUSKYLENS_V1.0_SKU_SEN0305_SEN0336#target_23>
 2.This code is tested on Arduino Uno, Leonardo, Mega boards.
 ****************************************************/

#include "DFRobot_HuskylensV2.h"

#define PRINT_ITEM(item)                 \
    Serial.print("result->" #item "=("); \
    Serial.print(result->item##_x);      \
    Serial.print(",");                   \
    Serial.print(result->item##_y);      \
    Serial.println(")");

// HUSKYLENS green line >> SDA/TX; blue line >> SCL/RX
HuskylensV2 huskylens;
#define RX_PIN_P0 1
#define TX_PIN_P1 2

void setup()  {
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, RX_PIN_P0, TX_PIN_P1);
    while (!huskylens.begin(Serial1)) {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (System Settings>>Protocol Type>> I2C/UART)"));
        Serial.println(F("2.Please recheck the connection."));
        Serial.println(F("\tgreen line >> SDA/TX; blue line >> SCL/RX"));
        delay(100);
    }
}

void loop() {
    while (!huskylens.getResult(ALGORITHM_FACE_RECOGNITION)) {
        delay(100);
    }

    while (huskylens.available(ALGORITHM_FACE_RECOGNITION)) {
        FaceResult *result = static_cast<FaceResult *>(huskylens.popCachedResult(ALGORITHM_FACE_RECOGNITION));

        Serial.print("result->ID=");
        Serial.println(result->ID, HEX);

        Serial.print("result->Center=(");
        Serial.print(result->xCenter, HEX);
        Serial.print(",");
        Serial.print(result->yCenter, HEX);
        Serial.println(")");

        Serial.println(result->width, HEX);
        Serial.print("result->height=");
        Serial.println(result->height, HEX);
        Serial.print("result->name=");
        Serial.println(result->name);
        Serial.print("result->content=");
        Serial.println(result->content);

        PRINT_ITEM(leye);
        PRINT_ITEM(reye);
        PRINT_ITEM(nose);
        PRINT_ITEM(lmouth);
        PRINT_ITEM(rmouth);
    }
    delay(1000);
}