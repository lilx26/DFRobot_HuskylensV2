#include <DFRobot_HuskylensV2.h>

HuskylensV2::HuskylensV2() : result{} {}
HuskylensV2::~HuskylensV2() {
  for (uint8_t algo = 0; algo < ALGORITHM_COUNT; algo++) {
    for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
      if (result[algo][i]) {
        delete result[algo][i];
        result[algo][i] = NULL;
      }
    }
  }
}

bool HuskylensV2::begin(Stream &streamInput) {
  stream = &streamInput;
  wire = NULL;
  return knock();
}
bool HuskylensV2::begin(TwoWire &streamInput) {
  DBG("\n");
  wire = &streamInput;
  stream = NULL;
  return knock();
}

int8_t HuskylensV2::getResult(eAlgorithm_t algo) {
  eAlgorithm_t _algo = algo;
  // 所有结果返回到ProtocolV2内存
  // 然后将所有权转给HuskylensV2
#ifdef LARGE_MEMORY
  algo = toRealID(algo);
#else
  algo = (eAlgorithm_t)0;
#endif

  for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i]) {
      delete result[algo][i];
      result[algo][i] = NULL;
    }
  }

  int8_t count = ProtocolV2::getResult(_algo);

  for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    // DBG(i);
    // DBG((long long)ProtocolV2::result[i]);
    result[algo][i] = ProtocolV2::result[i];
    ProtocolV2::result[i] = NULL;
  }
  return count;
}

bool HuskylensV2::available(eAlgorithm_t algo) {
  bool ret = false;
#ifdef LARGE_MEMORY
  algo = toRealID(algo);
#else
  algo = (eAlgorithm_t)0;
#endif

  for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i] != NULL)
      if (!result[algo][i]->used) {
        DBG(i);
        ret = true;
        break;
      }
  }

  return ret;
}

Result *HuskylensV2::popCachedResult(eAlgorithm_t algo) {
  DBG("\n");
#ifdef LARGE_MEMORY
  algo = toRealID(algo);
#else
  algo = (eAlgorithm_t)0;
#endif
  for (int8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i]) {
      if (result[algo][i]->used) {
        continue;
      }

      result[algo][i]->used = 1;
      DBG_PRINT("return ");
      DBG_PRINTLN((unsigned long)result[algo][i]);
      DBG(i);
      return result[algo][i];
    }
  }

  return NULL;
}
#ifdef LARGE_MEMORY
eAlgorithm_t HuskylensV2::toRealID(uint8_t id) {
  eAlgorithm_t algo = ALGORITHM_ANY;
  if (id >= ALGORITHM_CUSTOM_BEGIN) {
    for (uint8_t i = 0; i < CUSTOM_ALGORITHM_COUNT; i++)
      if (customId[i] == algo) {
        algo = (eAlgorithm_t)((int)ALGORITHM_CUSTOM0 + i);
        break;
      }
  }
  return algo;
}
#endif
Result *HuskylensV2::getCachedCenterResult(eAlgorithm_t algo) {
  DBG("\n");
#ifdef LARGE_MEMORY
  algo = toRealID(algo);
#else
  algo = (eAlgorithm_t)0;
#endif

  int8_t centerIndex = -1;
  int32_t minLen = 0x0FFFFFFF;
  for (int8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i]) {
      int32_t len = SQUARE(result[algo][i]->xCenter - LCD_WIDTH / 2) +
                    SQUARE(result[algo][i]->yCenter - LCD_HEIGHT / 2);
      if (len < minLen) {
        minLen = len;
        centerIndex = i;
      }
    }
  }
  if (centerIndex != -1) {
    return result[algo][centerIndex];
  }
  return NULL;
}

Result *HuskylensV2::getCachedResultByIndex(eAlgorithm_t algo, int16_t index) {
  DBG("\n");
#ifdef LARGE_MEMORY
  algo = toRealID(algo);
#else
  algo = (eAlgorithm_t)0;
#endif
  if (index >= MAX_RESULT_NUM) {
    return NULL;
  }
  return result[algo][index];
}

Result *HuskylensV2::getCachedResultByID(eAlgorithm_t algo, int16_t ID) {
  DBG("\n");
#ifdef LARGE_MEMORY
  algo = toRealID(algo);
#else
  algo = (eAlgorithm_t)0;
#endif
  for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i] == NULL) {
      continue;
    }
    if (result[algo][i]->ID == ID) {
      return result[algo][i];
    }
  }
  return NULL;
}

int16_t HuskylensV2::getCachedResultNum(eAlgorithm_t algo) {
  DBG("\n");
  int16_t count = 0;
#ifdef LARGE_MEMORY
  algo = toRealID(algo);
#else
  algo = (eAlgorithm_t)0;
#endif

  for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i]) {
      count++;
    }
  }
  return count;
}

int16_t HuskylensV2::getCachedResultLearnedNum(eAlgorithm_t algo) {
  DBG("\n");
  int16_t count = 0;
#ifdef LARGE_MEMORY
  algo = toRealID(algo);
#else
  algo = (eAlgorithm_t)0;
#endif
  for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i] && result[algo][i]->ID) {
      count++;
    }
  }
  return count;
}

int16_t HuskylensV2::getCachedResultNumByID(eAlgorithm_t algo, uint8_t id) {
  DBG("\n");
  int16_t count = 0;
#ifdef LARGE_MEMORY
  algo = toRealID(algo);
#else
  algo = (eAlgorithm_t)0;
#endif
  for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i] && (id == result[algo][i]->ID)) {
      count++;
    }
  }
  return count;
}

Result *HuskylensV2::getCachedIndexResultByID(eAlgorithm_t algo, uint8_t id,
                                              uint8_t index) {
  DBG("\n");
  Result *rlt = NULL;
  uint8_t _index = 0;
#ifdef LARGE_MEMORY
  algo = toRealID(algo);
#else
  algo = (eAlgorithm_t)0;
#endif
  for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i] && (id == result[algo][i]->ID)) {
      if (_index == index) {
        return result[algo][i];
      }
      _index++;
    }
  }
  return rlt;
}

int16_t HuskylensV2::getCachedResultMaxID(void) { return maxID; }


Result *HuskylensV2::getCurrentBranch(eAlgorithm_t algo){
  DBG("\n");
  Result *rlt = NULL;
#ifdef LARGE_MEMORY
  algo = toRealID(algo);
#else
  algo = (eAlgorithm_t)0;
#endif
  if (result[algo][0] && (result[algo][0]->level==1)) {
    return result[algo][0];
  }
}

int8_t HuskylensV2::getUpcomingBranchCount(eAlgorithm_t algo){
  DBG("\n");
  int8_t count  = 0;
#ifdef LARGE_MEMORY
  algo = toRealID(algo);
#else
  algo = (eAlgorithm_t)0;
#endif
   for (uint8_t i = 0; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i]) {
      count++;
    }
  }
  return count>0 ? count-1 : 0;
}

Result *HuskylensV2::getBranch(eAlgorithm_t algo , int16_t index){
  DBG("\n");
  Result *rlt = NULL;
  index++;
#ifdef LARGE_MEMORY
  algo = toRealID(algo);
#else
  algo = (eAlgorithm_t)0;
#endif
   for (uint8_t i = 1; i < MAX_RESULT_NUM; i++) {
    if (result[algo][i]) {
      if(i == index){
        rlt = result[algo][i];
        break;
      }
    }
  }
  return rlt;
}

#ifdef LARGE_MEMORY
bool HuskylensV2::setMultiAlgorithm(eAlgorithm_t algo0, eAlgorithm_t algo1,
                                    eAlgorithm_t algo2, eAlgorithm_t algo3,
                                    eAlgorithm_t algo4) {
  DBG("\n");
  customAlgoNum = 0;
  memset(customId, 0, sizeof(customId));
  if (algo0 >= ALGORITHM_CUSTOM_BEGIN) {
    customId[customAlgoNum++] = algo0;
  }
  if (algo1 >= ALGORITHM_CUSTOM_BEGIN) {
    customId[customAlgoNum++] = algo1;
  }
  if (algo2 >= ALGORITHM_CUSTOM_BEGIN) {
    customId[customAlgoNum++] = algo2;
  }
  if (algo3 >= ALGORITHM_CUSTOM_BEGIN) {
    customId[customAlgoNum++] = algo3;
  }
  if (algo4 >= ALGORITHM_CUSTOM_BEGIN) {
    customId[customAlgoNum++] = algo4;
  }
  return doSetMultiAlgorithm(algo0, algo1, algo2, algo3, algo4);
}
#endif