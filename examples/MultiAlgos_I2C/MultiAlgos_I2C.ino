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

void setup() {
  Serial.begin(115200);
  Wire.begin();
  while (!huskylens.begin(Wire)) {
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS "
                     "(General Settings>>Protocol Type>>I2C)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }
  huskylens.setMultiAlgorithm(ALGORITHM_FACE_RECOGNITION,
                              ALGORITHM_OBJECT_TRACKING,
                              ALGORITHM_HAND_RECOGNITION);
  delay(5000);
  huskylens.setMultiAlgorithmRatio(0, 0, 1);
  delay(1000);
}

int x1_, y1_, x2_, y2_;
bool start = false;
bool learned = false;

void loop() {
  while (!huskylens.getResult(ALGORITHM_HAND_RECOGNITION)) {
    delay(100);
  }
  HandResult *handResult = static_cast<HandResult *>(
      huskylens.popCachedResult(ALGORITHM_HAND_RECOGNITION));
  uint32_t distance =
      SQUARE(handResult->index_finger_tip_x - handResult->thumb_tip_x) +
      SQUARE(handResult->index_finger_tip_y - handResult->thumb_tip_y);

  Serial.print("distance=");
  Serial.println(distance);

  if (!start && distance < 300) {
    huskylens.clearText();
    huskylens.drawText(COLOR_RED, 20, 10, 10, "Start Draw Rect");
    x1_ = ((int)handResult->index_finger_tip_x + handResult->thumb_tip_x) / 2;
    y1_ = ((int)handResult->index_finger_tip_y + handResult->thumb_tip_y) / 2;
    start = true;
    Serial.print("x1=");
    Serial.println(x1_);
    Serial.print("y1=");
    Serial.println(y1_);
    return;
  }
  if (start && (distance < 3000)) {
    x2_ = ((int)handResult->index_finger_tip_x + handResult->thumb_tip_x) / 2;
    y2_ = ((int)handResult->index_finger_tip_y + handResult->thumb_tip_y) / 2;
    huskylens.drawUniqueRect(COLOR_GREEN, 2, x1_, y1_, x2_ - x1_, y2_ - y1_);
    Serial.print("x2=");
    Serial.println(x2_);
    Serial.print("y2=");
    Serial.println(y2_);
    return;
  }
  if (start && distance > 10000) {
    huskylens.clearText();
    huskylens.drawText(COLOR_RED, 20, 10, 10, "End Draw Rect, Start Learn");
    huskylens.setMultiAlgorithmRatio(0, 1, 0);
    huskylens.learnBlock(ALGORITHM_OBJECT_TRACKING, x1_, y1_, x2_ - x1_,
                         y2_ - y1_);
    learned = true;
    start = false;
  }
  if (!learned)
    return;
  while (1) {
    if (!huskylens.getResult(ALGORITHM_OBJECT_TRACKING)) {
      continue;
    }
    if (!huskylens.available(ALGORITHM_OBJECT_TRACKING)) {
      continue;
    }
    Result *result = (huskylens.popCachedResult(ALGORITHM_OBJECT_TRACKING));
    Serial.print("result->ID=");
    Serial.println(result->ID, HEX);
    Serial.print("result->xCenter=");
    Serial.println(result->xCenter);
    Serial.print("result->yCenter=");
    Serial.println(result->yCenter);
    Serial.print("result->width=");
    Serial.println(result->width);
    Serial.print("result->height=");
    Serial.println(result->height);
    Serial.print("result->name=");
    Serial.println(result->name);
    Serial.print("result->content=");
    Serial.println(result->content);
  }
}