/*
为ino提供接口
存储分析result
 */

#ifndef DFROBOT_HUSKYLENS_V2_H
#define DFROBOT_HUSKYLENS_V2_H
#include <Arduino.h>
#include <ProtocolV2.h>

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
  int16_t getCachedResultMaxID(void);

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