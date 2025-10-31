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

// HUSKYLENS green line >> SDA; blue line >> SCL
HuskylensV2 huskylens;
#define RX_PIN_P0 1
#define TX_PIN_P1 2

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RX_PIN_P0, TX_PIN_P1);
  while (!huskylens.begin(Serial1)) {
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS "
                     "(General Settings>>Protocol Type>>I2C)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }
  huskylens.setMultiAlgorithm(ALGORITHM_FACE_RECOGNITION,
                              ALGORITHM_OBJECT_RECOGNITION,
                              ALGORITHM_HAND_RECOGNITION);
  huskylens.setMultiAlgorithmRatio(0, 0, 1);
}

void loop() {
  while (!huskylens.getResult(ALGORITHM_HAND_RECOGNITION)) {
    delay(100);
  }

  HandResult *handResult = static_cast<HandResult *>(
      huskylens.popCachedResult(ALGORITHM_HAND_RECOGNITION));
  int x1 = handResult->index_finger_tip_x;
  int y1 = handResult->index_finger_tip_y;
  int x2 = handResult->thumb_tip_x;
  int y2 = handResult->thumb_tip_y;
  huskylens.setMultiAlgorithmRatio(1, 0, 0);
  huskylens.setLearningRectPosition(ALGORITHM_OBJECT_RECOGNITION, x1, y1, x2,
                                    y2);
  while (1) {
    if (!huskylens.getResult(ALGORITHM_OBJECT_RECOGNITION)) {
      Serial.println(
          F("Fail to request data from HUSKYLENS, recheck the connection!"));
      continue;
    }
    if (!huskylens.available(ALGORITHM_OBJECT_RECOGNITION)) {
      continue;
    }
    Result *result = (huskylens.popCachedResult(ALGORITHM_OBJECT_RECOGNITION));
    Serial.print("result->ID=");
    Serial.println(result->ID, HEX);
    Serial.print("result->xCenter=");
    Serial.println(result->xCenter, HEX);
    Serial.print("result->yCenter=");
    Serial.println(result->yCenter, HEX);
    Serial.print("result->width=");
    Serial.println(result->width, HEX);
    Serial.print("result->height=");
    Serial.println(result->height, HEX);
    Serial.print("result->name=");
    Serial.println(result->name);
    Serial.print("result->content=");
    Serial.println(result->content);
  }
}