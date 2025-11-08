/*
为ino提供接口
存储分析result
 */

#ifndef DFROBOT_HUSKYLENS_V2_H
#define DFROBOT_HUSKYLENS_V2_H
#include <Arduino.h>
#include <ProtocolV2.h>


#define COLOR_WHITE      0   // 白色 (255, 255, 255)
#define COLOR_RED        1   // 红色 (255, 0, 0)
#define COLOR_ORANGE     2   // 橙色 (255, 165, 0)
#define COLOR_YELLOW     3   // 黄色 (255, 255, 0)
#define COLOR_GREEN      4   // 绿色 (0, 255, 0)
#define COLOR_CYAN       5   // 青色 (0, 255, 255)
#define COLOR_BLUE       6   // 蓝色 (0, 0, 255)
#define COLOR_PURPLE     7   // 紫色 (128, 0, 128)
#define COLOR_PINK       8   // 粉色 (255, 192, 203)
#define COLOR_GRAY       9   // 灰色 (128, 128, 128)
#define COLOR_BLACK      10  // 黑色 (0, 0, 0)
#define COLOR_BROWN      11  // 棕色 (165, 42, 42)
#define COLOR_OLIVE      12  // 橄榄绿 (128, 128, 0)
#define COLOR_TEAL       13  // 蓝绿色 (0, 128, 128)
#define COLOR_INDIGO     14  // 靛蓝色 (75, 0, 130)
#define COLOR_MAGENTA    15  // 洋红色 (255, 0, 255)

class HuskylensV2 : public ProtocolV2 {

public:
  HuskylensV2();
  ~HuskylensV2();

  bool begin(Stream &streamInput);
  bool begin(TwoWire &streamInput);

  int8_t getResult(eAlgorithm_t algo);
  bool available(eAlgorithm_t algo);

  Result *popCachedResult(eAlgorithm_t algo);
  Result *getCachedCenterResult(eAlgorithm_t algo);
  Result *getCachedResultByIndex(eAlgorithm_t algo, int16_t index);
  Result *getCachedResultByID(eAlgorithm_t algo, int16_t ID);
  int16_t getCachedResultNum(eAlgorithm_t algo);
  int16_t getCachedResultLearnedNum(eAlgorithm_t algo);

  int16_t getCachedResultNumByID(eAlgorithm_t algo, uint8_t id);
  Result *getCachedIndexResultByID(eAlgorithm_t algo, uint8_t id,
                                   uint8_t index);
  int16_t getCachedResultMaxID(eAlgorithm_t algo);

  Result *getCurrentBranch(eAlgorithm_t algo);
  int8_t getUpcomingBranchCount(eAlgorithm_t algo);
  Result *getBranch(eAlgorithm_t algo , int16_t index);
#ifdef LARGE_MEMORY
  bool setMultiAlgorithm(eAlgorithm_t algo0, eAlgorithm_t algo1,
                         eAlgorithm_t algo2 = ALGORITHM_ANY,
                         eAlgorithm_t algo3 = ALGORITHM_ANY,
                         eAlgorithm_t algo4 = ALGORITHM_ANY);
#endif
public:
  Result *result[ALGORITHM_COUNT][MAX_RESULT_NUM];

#ifdef LARGE_MEMORY
  uint8_t customId[5];
  uint8_t customAlgoNum;
  eAlgorithm_t toRealID(uint8_t id);
#endif
};

#define RET_ITEM_NUM(func, type, item)                                         \
  [](type *rlt) { return rlt ? rlt->item : -1; }(static_cast<type *>(func))

#define RET_ITEM_STR(func, type, item)                                         \
  [](type *rlt) { return rlt ? rlt->item : ""; }(static_cast<type *>(func))

#endif // DFROBOT_HUSKYLENS_V2_H