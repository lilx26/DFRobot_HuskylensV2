#ifndef DFROBOT_HUSKEYLENS_V2_COMMON_H
#define DFROBOT_HUSKEYLENS_V2_COMMON_H
#include <Arduino.h>
#include <HardwareSerial.h>
#include <Stream.h>
#if 1
#define DBG(x)                                                                 \
  do {                                                                         \
    Serial.print(__FUNCTION__);                                                \
    Serial.print("():");                                                       \
    Serial.print(__LINE__);                                                    \
    Serial.print(":");                                                         \
    Serial.println(x);                                                         \
  } while (0)
#define DBG_KV(k, v)                                                           \
  do {                                                                         \
    Serial.print(__FUNCTION__);                                                \
    Serial.print("():");                                                       \
    Serial.print(__LINE__);                                                    \
    Serial.print(":");                                                         \
    Serial.print(k);                                                           \
    Serial.println(v);                                                         \
  } while (0)
#define DBG_HEX(x)                                                             \
  do {                                                                         \
    Serial.print(__FUNCTION__);                                                \
    Serial.print("():");                                                       \
    Serial.print(__LINE__);                                                    \
    Serial.print(":");                                                         \
    Serial.println(x, HEX);                                                    \
  } while (0)

#define DBG_HEX_ARRAY(x, len)                                                  \
  do {                                                                         \
    for (uint16_t i = 0; i < len; i++) {                                       \
      Serial.print(x[i], HEX);                                                 \
      Serial.print(" ");                                                       \
    }                                                                          \
  } while (0)
#define DBG_PRINT(x) Serial.print(x)
#define DBG_PRINTLN(x) Serial.println(x)
#else
#define DBG(x)
#define DBG_KV(k, v)
#define DBG_HEX(x)
#define DBG_HEX_ARRAY(x, len)
#define DBG_PRINT(x)
#define DBG_PRINTLN(x)
#endif

#include "Wire.h"

#if defined(ESP32) || defined(NRF5) || defined(ESP8266)
#define LARGE_MEMORY 1
#endif

//  小内存（AVR）只使用基类的内容，大内存MCU（esp32 microbit）可以使用派生类内容
#ifdef LARGE_MEMORY
#define FRAME_BUFFER_SIZE 512
#define MAX_RESULT_NUM 10
#define CMD_BUFFER_SIZE 512
#else
#define FRAME_BUFFER_SIZE 128
#define MAX_RESULT_NUM 6
#define CMD_BUFFER_SIZE 32
#endif

#define LCD_WIDTH 640
#define LCD_HEIGHT 480

#define HEADER_0_INDEX 0
#define HEADER_1_INDEX 1
#define COMMAND_INDEX 2
#define ALGO_INDEX 3
#define CONTENT_SIZE_INDEX 4
#define CONTENT_INDEX 5
#define PROTOCOL_SIZE 6

#define SQUARE(x) ((x) * (x))

enum protocolCommand {
  COMMAND_KNOCK = 0x20,
  COMMAND_GET_RESULT = 0x21,
  COMMAND_GET_INFO = 0x22,
  COMMAND_GET_RESULT_BY_ID = 0x23,
  COMMAND_GET_BLOCKS_BY_ID = 0x24,
  COMMAND_GET_ARROWS_BY_ID = 0x25,
  COMMAND_GET_SENSOR_LIST = 0x26,
  COMMAND_GET_RESULT_BY_INDEX = 0x27,
  COMMAND_GET_BLOCKS_BY_INDEX = 0x28,
  COMMAND_GET_ARROWS_BY_INDEX = 0x29,
  // RFU 0x2A - 0x2F

  COMMAND_SET_ALGORITHM = 0x30,
  COMMAND_SET_NAME_BY_ID = 0x31,
  COMMAND_SET_MULTI_ALGORITHM = 0x32,
  COMMAND_SET_MULTI_ALGORITHM_RATIO = 0x33,
  // RFU 0x35 - 0x3F

  COMMAND_RETURN_ARGS = 0x40, // OK ERROR
  COMMAND_RETURN_INFO = 0x41,
  COMMAND_RETURN_BLOCK = 0x42,
  COMMAND_RETURN_ARROW = 0x43,
  // RFU 0x46 - 0x4F

  COMMAND_ACTION_TAKE_PHOTO = 0x50,
  COMMAND_ACTION_TAKE_SCREENSHOT = 0x51,
  COMMAND_ACTION_LEARN = 0x52,
  COMMAND_ACTION_FORGOT = 0x53,

  COMMAND_ACTION_SAVE_KNOWLEDGES = 0x54,
  COMMAND_ACTION_LOAD_KNOWLEDGES = 0x55,

  COMMAND_ACTION_DRAW_RECT = 0x56,
  COMMAND_ACTION_CLEAN_RECT = 0x57,
  COMMAND_ACTION_DRAW_TEXT = 0x58,
  COMMAND_ACTION_CLEAR_TEXT = 0x59,
  COMMAND_ACTION_PLAY_MUSIC = 0x5A,
  COMMAND_ACTION_EXIT = 0x5B,
  COMMAND_ACTION_LEARN_BLOCK = 0x5C,
  COMMAND_ACTION_DRAW_UNIQUE_RECT = 0x5D,
};

typedef enum {
  ALGORITHM_ANY = 0,                      // 0
  ALGORITHM_FACE_RECOGNITION = 1,         // 1
  ALGORITHM_OBJECT_TRACKING,              // 2
  ALGORITHM_OBJECT_RECOGNITION,           // 3
  ALGORITHM_LINE_TRACKING,                // 6
  ALGORITHM_COLOR_RECOGNITION,            // 5
  ALGORITHM_TAG_RECOGNITION,              // 6
  ALGORITHM_SELF_LEARNING_CLASSIFICATION, // 7
  ALGORITHM_OCR_RECOGNITION,              // 8
  ALGORITHM_LICENSE_RECOGNITION,          // 9
  ALGORITHM_QRCODE_RECOGNITION,           // 10
  ALGORITHM_BARCODE_RECOGNITION,          // 11
  ALGORITHM_EMOTION_RECOGNITION,          // 12
  ALGORITHM_POSE_RECOGNITION,             // 13
  ALGORITHM_HAND_RECOGNITION,             // 14
  ALGORITHM_OBJECT_CLASSIFICATION,        // 15
  ALGORITHM_BLINK_RECOGNITION,            // 16
  ALGORITHM_GAZE_RECOGNITION,             // 17
  ALGORITHM_FACE_ORIENTATION,             // 18
  ALGORITHM_FALLDOWN_RECOGNITION,         // 19
  ALGORITHM_SEGMENT,                      // 20
  ALGORITHM_FACE_ACTION_RECOGNITION,      // 21
  ALGORITHM_CUSTOM0,                      // 22
  ALGORITHM_CUSTOM1,                      // 23
  ALGORITHM_CUSTOM2,                      // 24
  ALGORITHM_BUILTIN_COUNT,                // 25

  ALGORITHM_CUSTOM_BEGIN = 128, // 128

} eAlgorithm_t;

#ifdef LARGE_MEMORY
#define ALGORITHM_COUNT ALGORITHM_BUILTIN_COUNT
#define CUSTOM_ALGORITHM_COUNT 3
#else
#define ALGORITHM_COUNT 1
#define CUSTOM_ALGORITHM_COUNT 1
#endif

typedef enum BoardType {
  BOARD_SMALL_RAM = 0x0,
  BOARD_LARGE_RAM = 0x1
} eBoardType_t;

typedef enum ResultType {
  Result_Block = 0x0,
  Result_Arrow = 0x1,
  Result_Unknown
} eResultType_t;

typedef struct __attribute__((packed)) {
  uint8_t head55;
  uint8_t headaa;
  uint8_t cmd;
  uint8_t algo_id;
  uint8_t data_length;
  uint8_t data[0];
} PacketHead_t;

typedef struct __attribute__((packed)) {
  uint8_t length;
  int8_t data[0];
  // 成员函数方式
  String toString() const {
    if (length == 0) {
      return String("");
    }
    char *buf = (char *)malloc(length + 1);
    if (buf == NULL) {
      return String("");
    }
    memcpy(buf, data, length);
    buf[length] = 0;
    String str(buf);
    free(buf);
    return str;
  }
} String_t;

typedef struct __attribute__((packed)) {
  union {
    uint8_t ID;
    uint8_t maxID;
    uint8_t colorID;
    uint8_t rfu0;
    uint8_t boardType;
    uint8_t totalSensors;
    uint8_t multiAlgoNum;
  };
  union {
    int8_t rfu1;
    int8_t level;
    int8_t retValue;
    int8_t lineWidth;
    int8_t confidence;
    int8_t currSensorIndex;
  };
  union {
    int16_t first;
    int16_t xCenter;
    int16_t xTarget;
    int16_t algorithmType;
    int16_t classID;
    int16_t sensor0ID;
    int16_t total_results;
  };
  union {
    int16_t second;
    int16_t yCenter;
    int16_t yTarget;
    int16_t sensor1ID;
    int16_t total_results_learned;
  };
  union {
    int16_t third;
    int16_t width;
    int16_t angle;
    int16_t sensor2ID;
    int16_t total_blocks;
  };
  union {
    int16_t fourth;
    int16_t height;
    int16_t length;
    int16_t total_blocks_learned;
  };
  int8_t payload[0];
} PacketData_t;

class Result : public PacketData_t {
public:
  Result(const void *buf);
  ~Result();

public:
  int8_t type = -1;
  int8_t used = 0;
  String name;
  String content;
};

#ifdef LARGE_MEMORY
class FaceResult : public Result {
public:
  FaceResult(const void *buf);

public:
  int16_t leye_x;
  int16_t leye_y;
  int16_t reye_x;
  int16_t reye_y;
  int16_t nose_x;
  int16_t nose_y;
  int16_t lmouth_x;
  int16_t lmouth_y;
  int16_t rmouth_x;
  int16_t rmouth_y;
};

class HandResult : public Result {
public:
  HandResult(const void *buf);

public:
  int16_t wrist_x, wrist_y;         // 手腕
  int16_t thumb_cmc_x, thumb_cmc_y; // 拇指根部
  int16_t thumb_mcp_x, thumb_mcp_y; // 拇指中间关节
  int16_t thumb_ip_x, thumb_ip_y;   // 拇指第二关节
  int16_t thumb_tip_x, thumb_tip_y; // 拇指指尖

  int16_t index_finger_mcp_x, index_finger_mcp_y;   // 食指根部
  int16_t index_finger_pip_x, index_finger_pip_y;   // 食指第一关节
  int16_t index_finger_dip_x, index_finger_dip_y;   // 食指第二关节
  int16_t index_finger_tip_x, index_finger_tip_y;   // 食指指尖
  int16_t middle_finger_mcp_x, middle_finger_mcp_y; // 中指根部

  int16_t middle_finger_pip_x, middle_finger_pip_y; // 中指第一关节
  int16_t middle_finger_dip_x, middle_finger_dip_y; // 中指第二关节
  int16_t middle_finger_tip_x, middle_finger_tip_y; // 中指指尖
  int16_t ring_finger_mcp_x, ring_finger_mcp_y;     // 无名指根部
  int16_t ring_finger_pip_x, ring_finger_pip_y;     // 无名指第一关节

  int16_t ring_finger_dip_x, ring_finger_dip_y;   // 无名指第二关节
  int16_t ring_finger_tip_x, ring_finger_tip_y;   // 无名指指尖
  int16_t pinky_finger_mcp_x, pinky_finger_mcp_y; // 小指根部
  int16_t pinky_finger_pip_x, pinky_finger_pip_y; // 小指第一关节
  int16_t pinky_finger_dip_x, pinky_finger_dip_y; // 小指第二关节

  int16_t pinky_finger_tip_x, pinky_finger_tip_y; // 小指指尖
};

class PoseResult : public Result {
public:
  PoseResult(const void *buf);

public:
  int16_t nose_x, nose_y;
  int16_t leye_x, leye_y;
  int16_t reye_x, reye_y;
  int16_t lear_x, lear_y;
  int16_t rear_x, rear_y;

  int16_t lshoulder_x, lshoulder_y;
  int16_t rshoulder_x, rshoulder_y;
  int16_t lelbow_x, lelbow_y;
  int16_t relbow_x, relbow_y;
  int16_t lwrist_x, lwrist_y;

  int16_t rwrist_x, rwrist_y;
  int16_t lhip_x, lhip_y;
  int16_t rhip_x, rhip_y;
  int16_t lknee_x, lknee_y;
  int16_t rknee_x, rknee_y;

  int16_t lankle_x, lankle_y;
  int16_t rankle_x, rankle_y;
};
#endif // LARGE_MEMORY
#endif // DFROBOT_HUSKEYLENS_V2_RESULT_H