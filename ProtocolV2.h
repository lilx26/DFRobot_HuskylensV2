/*
纯底层通信，包含了通用通信协议的处理
 */
#ifndef DFROBOT_HUSKEYLENS_V2_PROTOCOL_H
#define DFROBOT_HUSKEYLENS_V2_PROTOCOL_H
#include <Result.h>

// 小内存（AVR）只使用基类的内容，大内存MCU（esp32 microbit）可以使用派生类内容
class ProtocolV2 {
public:
  ProtocolV2() : result{} {};
  ~ProtocolV2() {};

  void protocolWrite(uint8_t *buffer, int length);
  void timerBegin();
  bool timerAvailable();
  bool protocolAvailable();
  bool wait(uint8_t command = 0);
  bool knock(void);
  PacketData_t getInfo(eAlgorithm_t algo);
  int8_t getResult(eAlgorithm_t algo);
  int8_t getResultByID(eAlgorithm_t algo);
  bool setLearningRectPosition(eAlgorithm_t algo, int16_t x1, int16_t y1,
                               int16_t x2, int16_t y2);
  bool switchAlgorithm(eAlgorithm_t algo);
  bool takePhoto(eAlgorithm_t algo);
  bool takeScreenshot(eAlgorithm_t algo);
  bool learn(eAlgorithm_t algo);
  bool forgot(eAlgorithm_t algo);
  bool drawRect(eAlgorithm_t algo, uint8_t colorID, int16_t x1, int16_t y1,
                int16_t x2, int16_t y2);
  bool clearRect(eAlgorithm_t algo);
  bool drawText(eAlgorithm_t algo, uint8_t colorID, int16_t x, int16_t y,
                String text);
  bool clearText(eAlgorithm_t algo);
  bool saveKnowledges(eAlgorithm_t algo, uint8_t knowledgeID);
  bool loadKnowledges(eAlgorithm_t algo, uint8_t knowledgeID);
  bool playMusic(String name, int16_t volume);
  bool setNameByID(eAlgorithm_t algo, uint8_t id, String name);

#ifdef LARGE_MEMORY
  bool doSetMultiAlgorithm(eAlgorithm_t algo0, eAlgorithm_t algo1,
                           eAlgorithm_t algo2 = ALGORITHM_ANY,
                           eAlgorithm_t algo3 = ALGORITHM_ANY,
                           eAlgorithm_t algo4 = ALGORITHM_ANY);
  bool setMultiAlgorithmRatio(int8_t ratio0, int8_t ratio1, int8_t ratio2 = -1,
                              int8_t ratio3 = -1, int8_t ratio4 = -1);
#endif
public:
  int16_t maxID;
  int16_t total_results;
  int16_t total_blocks;
  int16_t total_arrows;
  Result *result[MAX_RESULT_NUM];
  int16_t retry = 5;
  TwoWire *wire = NULL;
  Stream *stream = NULL;
  unsigned long timeOutDuration = 2000;
  unsigned long timeOutTimer;
};

#endif // DFROBOT_HUSKEYLENS_V2_RESULT_H