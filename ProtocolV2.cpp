#include <ProtocolV2.h>

static uint8_t send_buffer[CMD_BUFFER_SIZE];
static uint8_t receive_buffer[FRAME_BUFFER_SIZE];

static short send_index = 0;
static short receive_index = 0;

static bool send_fail = false;
static bool receive_fail = false;

static short content_current = 0;
static short content_end = 0;
static bool content_read_end = false;

uint8_t *husky_lens_protocol_write_begin(eAlgorithm_t algo, uint8_t command) {
  send_fail = false;
  send_buffer[HEADER_0_INDEX] = 0x55;
  send_buffer[HEADER_1_INDEX] = 0xAA;
  send_buffer[COMMAND_INDEX] = command;
  send_buffer[ALGO_INDEX] = algo;
  send_index = CONTENT_INDEX;
  return (uint8_t *)send_buffer;
}

void husky_lens_protocol_write_uint8(uint8_t content) {
  if (send_index + sizeof(content) >= CMD_BUFFER_SIZE) {
    send_fail = true;
    return;
  }
  memcpy(send_buffer + send_index, &content, sizeof(content));
  send_index += sizeof(content);
}

void husky_lens_protocol_write_int16(int16_t content) {
  if (send_index + sizeof(content) >= CMD_BUFFER_SIZE) {
    send_fail = true;
    return;
  }
  memcpy(send_buffer + send_index, &content, sizeof(content));
  send_index += sizeof(content);
}

void husky_lens_protocol_write_int32(int32_t content) {
  if (send_index + sizeof(content) >= CMD_BUFFER_SIZE) {
    send_fail = true;
    return;
  }
  memcpy(send_buffer + send_index, &content, sizeof(content));
  send_index += sizeof(content);
}

void husky_lens_protocol_write_float(float content) {
  if (send_index + sizeof(content) >= CMD_BUFFER_SIZE) {
    send_fail = true;
    return;
  }
  memcpy(send_buffer + send_index, &content, sizeof(content));
  send_index += sizeof(content);
}

void husky_lens_protocol_write_buffer_uint8(uint8_t *content, uint32_t length) {
  if (send_index + length >= CMD_BUFFER_SIZE) {
    send_fail = true;
    return;
  }
  memcpy(send_buffer + send_index, content, length);
  send_index += length;
}

int husky_lens_protocol_write_end() {
  if (send_fail) {
    return 0;
  }
  if (send_index + 1 >= CMD_BUFFER_SIZE) {
    return 0;
  }
  send_buffer[CONTENT_SIZE_INDEX] = send_index - CONTENT_INDEX;
  uint8_t sum = 0;
  for (int i = 0; i < send_index; i++) {
    sum += send_buffer[i];
  }
  send_buffer[send_index] = sum;
  send_index++;
  return send_index;
}
bool validateCheckSum() {
  uint8_t stackSumIndex = receive_buffer[CONTENT_SIZE_INDEX] + CONTENT_INDEX;
  uint8_t sum = 0;
  uint8_t i;
  // calculate the checksum
  for (i = 0; i < stackSumIndex; i++) {
    sum += receive_buffer[i];
  }
  if (sum != receive_buffer[stackSumIndex]) {
    DBG("checksum error [expected] [received]\n");
    DBG_HEX(sum);
    DBG_HEX(receive_buffer[stackSumIndex]);
  }
  return (sum == receive_buffer[stackSumIndex]);
}
bool husky_lens_protocol_receive(uint8_t data) {
  // DBG_HEX(receive_index);
  switch (receive_index) {
  case HEADER_0_INDEX:
    if (data != 0x55) {
      receive_index = 0;
      return false;
    }
    receive_buffer[HEADER_0_INDEX] = 0x55;
    break;
  case HEADER_1_INDEX:
    if (data != 0xaa) {
      receive_index = 0;
      return false;
    }
    receive_buffer[HEADER_1_INDEX] = 0xaa;
    break;
  case COMMAND_INDEX:
    receive_buffer[COMMAND_INDEX] = data;
    break;
  case ALGO_INDEX:
    receive_buffer[ALGO_INDEX] = data;
    break;
  case CONTENT_SIZE_INDEX:
    if ((int)receive_index >= FRAME_BUFFER_SIZE - PROTOCOL_SIZE) {
      receive_index = 0;
      return false;
    }
    receive_buffer[CONTENT_SIZE_INDEX] = data;
    break;
  default:
    receive_buffer[receive_index] = data;
    if (receive_index == receive_buffer[CONTENT_SIZE_INDEX] + CONTENT_INDEX) {
      content_end = receive_index;
      receive_index = 0;
      DBG_PRINTLN("<--");
      DBG_HEX_ARRAY(receive_buffer,
                    receive_buffer[CONTENT_SIZE_INDEX] + PROTOCOL_SIZE);
      DBG_PRINT("\n");
      return validateCheckSum();
    }
    break;
  }
  receive_index++;
  return false;
}

bool ProtocolV2::wait(uint8_t command) {
  timerBegin();
  while (!timerAvailable()) {
    if (protocolAvailable()) {
      if (command == receive_buffer[COMMAND_INDEX]) {
        DBG("return true\n");
        return true;
      } else {
        DBG("return false\n");
        DBG_HEX(command);
        DBG_HEX(receive_buffer[COMMAND_INDEX]);
        return false;
      }
    }
  }
  DBG("\n");
  return false;
}

void ProtocolV2::protocolWrite(uint8_t *buffer, int length) {
  if (wire) {
    DBG_PRINTLN("-->");
    DBG_HEX_ARRAY(buffer, length);
    DBG_PRINT("\n");
    timeOutDuration = 2000;
    wire->setClock(100000);
    wire->beginTransmission(0x50);
    wire->write(buffer, length);
    wire->endTransmission();
  } else if (stream) {
    DBG_PRINTLN("-->");
    DBG_HEX_ARRAY(buffer, length);
    DBG_PRINT("\n");
    timeOutDuration = 5000;
    stream->write(buffer, length);
    stream->flush();
  }
}
void ProtocolV2::timerBegin() { timeOutTimer = millis(); }
bool ProtocolV2::timerAvailable() {
  return (millis() - timeOutTimer > timeOutDuration);
}
bool ProtocolV2::protocolAvailable() {
  if (wire) {
    if (!wire->available()) {
      wire->setClock(100000);
      wire->requestFrom(0x50, 16);
    }
    while (wire->available()) {
      uint8_t result = wire->read();
      if (husky_lens_protocol_receive(result)) {
        DBG("\n");
        return true;
      }
    }
  } else if (stream) {
    while (stream->available()) {
      uint8_t c = stream->read();
      if (husky_lens_protocol_receive(c)) {
        DBG("full packet\n");
        return true;
      }
    }
  }

  return false;
}
bool ProtocolV2::knock(void) {
  DBG("\n");
  uint8_t *buffer =
      husky_lens_protocol_write_begin(ALGORITHM_ANY, COMMAND_KNOCK);
#ifdef LARGE_MEMORY
  husky_lens_protocol_write_uint8(BOARD_LARGE_RAM);
#else
  husky_lens_protocol_write_uint8(BOARD_SMALL_RAM);
#endif
  for (uint8_t i = 0; i < 9; i++) {
    husky_lens_protocol_write_uint8(0);
  }
  int length = husky_lens_protocol_write_end();

  for (uint8_t i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    delay(10);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}

bool ProtocolV2::switchAlgorithm(eAlgorithm_t algo) {
  DBG("\n");
  uint8_t *buffer =
      husky_lens_protocol_write_begin(ALGORITHM_ANY, COMMAND_SET_ALGORITHM);

  husky_lens_protocol_write_uint8((uint8_t)algo);
  husky_lens_protocol_write_uint8(0);
  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_int16(0);
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}

// get info 获取统计信息
PacketData_t ProtocolV2::getInfo(eAlgorithm_t algo) {
  uint8_t *buffer = husky_lens_protocol_write_begin(algo, COMMAND_GET_INFO);
  int length = husky_lens_protocol_write_end();
  int ret = -1;
  PacketData_t info;
  memset(&info, 0, sizeof(PacketData_t));
  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_INFO)) {
      ret = 0;
    }
  }
  if (ret == 0) {
    PacketHead_t *head = (PacketHead_t *)receive_buffer;
    PacketData_t *packet = (PacketData_t *)head->data;

    info.total_results = packet->total_results;
    info.total_results_learned = packet->total_results_learned;
    info.total_blocks = packet->total_blocks;
    info.total_blocks_learned = packet->total_blocks_learned;
  }
  return info;
}
// get result ,获取所有result，将统计信息保存heap中
// 返回result的数目
int8_t ProtocolV2::getResult(eAlgorithm_t algo) {
  DBG("\n");
  uint8_t *buffer = husky_lens_protocol_write_begin(algo, COMMAND_GET_RESULT);
  int length = husky_lens_protocol_write_end();
  int8_t _count = 0;
  int8_t i = 0;
  PacketData_t info;
  PacketHead_t *head = (PacketHead_t *)receive_buffer;
  for (i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_INFO)) {
      PacketData_t *packet = (PacketData_t *)head->data;
      maxID = packet->maxID;
      info.total_results = packet->total_results;
      info.total_results_learned = packet->total_results_learned;
      info.total_blocks = packet->total_blocks;
      info.total_blocks_learned = packet->total_blocks_learned;
      DBG_PRINT("info.total_results=");
      DBG(info.total_results);
      DBG_PRINT("info.total_blocks=");
      DBG(info.total_blocks);
      if (info.total_results > MAX_RESULT_NUM) {
        info.total_results = MAX_RESULT_NUM;
      }
      if (info.total_blocks > MAX_RESULT_NUM) {
        info.total_blocks = MAX_RESULT_NUM;
      }

      break;
    }
  }
  if (i == retry) {
    DBG("i==retry");
    return -1;
  }

  for (int i = 0; i < info.total_blocks; i++) {
    DBG("\n");
    if (wait(COMMAND_RETURN_BLOCK)) {
      _count++;
#ifdef LARGE_MEMORY
      if (algo == ALGORITHM_FACE_RECOGNITION) {
        DBG("new Face Result\n");
        result[i] = new FaceResult((const void *)receive_buffer);
      } else if (algo == ALGORITHM_HAND_RECOGNITION) {
        DBG("new Hand Result\n");
        result[i] = new HandResult((const void *)receive_buffer);
      } else if (algo == ALGORITHM_POSE_RECOGNITION) {
        DBG("new Pose Result\n");
        result[i] = new PoseResult((const void *)receive_buffer);
      } else {
        DBG("new Result\n");
        result[i] = new Result((const void *)receive_buffer);
      }
#else
      result[i] = new Result(receive_buffer);
#endif
    }
  }
  for (int i = info.total_blocks; i < info.total_results; i++) {
    DBG("\n");
    if (wait(COMMAND_RETURN_ARROW)) {
      _count++;
#ifdef LARGE_MEMORY
      if (algo == ALGORITHM_FACE_RECOGNITION) {
        DBG("new Face Result\n");
        result[i] = new FaceResult((const void *)receive_buffer);
      } else if (algo == ALGORITHM_HAND_RECOGNITION) {
        DBG("new Hand Result\n");
        result[i] = new HandResult((const void *)receive_buffer);
      } else if (algo == ALGORITHM_POSE_RECOGNITION) {
        DBG("new Pose Result\n");
        result[i] = new PoseResult((const void *)receive_buffer);
      } else {
        DBG("new Result\n");
        result[i] = new Result((const void *)receive_buffer);
      }
#else
      result[i] = new Result(receive_buffer);
#endif
    }
  }
  DBG("ProtocolV2 return");
  DBG_HEX(_count);
  return _count;
}

int8_t ProtocolV2::getResultByID(eAlgorithm_t algo) {
  uint8_t *buffer = husky_lens_protocol_write_begin(algo, COMMAND_GET_RESULT);
  int length = husky_lens_protocol_write_end();

  for (int i; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return 1;
    }
  }
  return 0;
}

#ifdef LARGE_MEMORY
bool ProtocolV2::doSetMultiAlgorithm(eAlgorithm_t algo0, eAlgorithm_t algo1,
                                     eAlgorithm_t algo2, eAlgorithm_t algo3,
                                     eAlgorithm_t algo4) {
  DBG("\n");
  uint8_t *buffer = husky_lens_protocol_write_begin(
      ALGORITHM_ANY, COMMAND_SET_MULTI_ALGORITHM);
  uint8_t multiAlgoNum = 0;
  int16_t algos[5] = {ALGORITHM_ANY, ALGORITHM_ANY, ALGORITHM_ANY,
                      ALGORITHM_ANY, ALGORITHM_ANY};
  if (algo0 != ALGORITHM_ANY) {
    multiAlgoNum++;
    algos[0] = algo0;
  }
  if (algo1 != ALGORITHM_ANY) {
    multiAlgoNum++;
    algos[1] = algo1;
  }
  if (algo2 != ALGORITHM_ANY) {
    multiAlgoNum++;
    algos[2] = algo2;
  }
  if (algo3 != ALGORITHM_ANY) {
    multiAlgoNum++;
    algos[3] = algo3;
  }
  if (algo4 != ALGORITHM_ANY) {
    multiAlgoNum++;
    algos[4] = algo4;
  }

  husky_lens_protocol_write_uint8(multiAlgoNum);
  husky_lens_protocol_write_uint8(0);
  multiAlgoNum = multiAlgoNum < 4 ? 4 : multiAlgoNum;
  for (uint8_t i = 0; i < multiAlgoNum; i++) {
    husky_lens_protocol_write_int16(algos[i]);
  }
  DBG("----");
  DBG_HEX_ARRAY(buffer, 20);
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}

bool ProtocolV2::setMultiAlgorithmRatio(int8_t ratio0, int8_t ratio1,
                                        int8_t ratio2, int8_t ratio3,
                                        int8_t ratio4) {
  DBG("\n");
  uint8_t *buffer = husky_lens_protocol_write_begin(
      ALGORITHM_ANY, COMMAND_SET_MULTI_ALGORITHM_RATIO);
  uint8_t multiAlgoNum = 0;
  int16_t ratios[5];
  if (ratio0 != -1) {
    multiAlgoNum++;
    ratios[0] = ratio0;
  }
  if (ratio1 != -1) {
    multiAlgoNum++;
    ratios[1] = ratio1;
  }
  if (ratio2 != -1) {
    multiAlgoNum++;
    ratios[2] = ratio2;
  }
  if (ratio3 != -1) {
    multiAlgoNum++;
    ratios[3] = ratio3;
  }
  if (ratio4 != -1) {
    multiAlgoNum++;
    ratios[4] = ratio4;
  }
  husky_lens_protocol_write_uint8(multiAlgoNum);
  husky_lens_protocol_write_uint8(0);
  multiAlgoNum = multiAlgoNum < 4 ? 4 : multiAlgoNum;
  for (uint8_t i = 0; i < multiAlgoNum; i++) {
    husky_lens_protocol_write_int16(ratios[i]);
  }
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}
#endif
bool ProtocolV2::setLearningRectPosition(eAlgorithm_t algo, int16_t x1,
                                         int16_t y1, int16_t x2, int16_t y2) {
  DBG("\n");
  uint8_t *buffer =
      husky_lens_protocol_write_begin(algo, COMMAND_SET_LEARN_BLOCK_POSITION);
  husky_lens_protocol_write_uint8(0);
  husky_lens_protocol_write_uint8(0);

  husky_lens_protocol_write_int16(x1);
  husky_lens_protocol_write_int16(y1);
  husky_lens_protocol_write_int16(x2);
  husky_lens_protocol_write_int16(y2);
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}

bool ProtocolV2::takePhoto(eAlgorithm_t algo) {
  DBG("\n");
  uint8_t *buffer =
      husky_lens_protocol_write_begin(algo, COMMAND_ACTION_TAKE_PHOTO);
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}

bool ProtocolV2::takeScreenshot(eAlgorithm_t algo) {
  DBG("\n");
  uint8_t *buffer =
      husky_lens_protocol_write_begin(algo, COMMAND_ACTION_TAKE_SCREENSHOT);
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}

bool ProtocolV2::learn(eAlgorithm_t algo) {
  DBG("\n");
  uint8_t *buffer = husky_lens_protocol_write_begin(algo, COMMAND_ACTION_LEARN);
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}

bool ProtocolV2::forgot(eAlgorithm_t algo) {
  DBG("\n");
  uint8_t *buffer =
      husky_lens_protocol_write_begin(algo, COMMAND_ACTION_FORGOT);
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}

bool ProtocolV2::drawRect(eAlgorithm_t algo, uint8_t colorID, int16_t x1,
                          int16_t y1, int16_t x2, int16_t y2) {
  DBG("\n");
  uint8_t *buffer =
      husky_lens_protocol_write_begin(algo, COMMAND_ACTION_DRAW_RECT);
  husky_lens_protocol_write_uint8(colorID);
  husky_lens_protocol_write_uint8(0);

  husky_lens_protocol_write_int16(x1);
  husky_lens_protocol_write_int16(y1);
  husky_lens_protocol_write_int16(x2);
  husky_lens_protocol_write_int16(y2);
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}

bool ProtocolV2::clearRect(eAlgorithm_t algo) {
  DBG("\n");
  uint8_t *buffer =
      husky_lens_protocol_write_begin(algo, COMMAND_ACTION_CLEAN_RECT);
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}

bool ProtocolV2::drawText(eAlgorithm_t algo, uint8_t colorID, int16_t x,
                          int16_t y, String text) {
  DBG("\n");
  uint8_t *buffer =
      husky_lens_protocol_write_begin(algo, COMMAND_ACTION_DRAW_TEXT);
  husky_lens_protocol_write_uint8(colorID);
  husky_lens_protocol_write_uint8(0);

  husky_lens_protocol_write_int16(x);
  husky_lens_protocol_write_int16(y);
  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_uint8(text.length());
  for (uint8_t i = 0; i < text.length(); i++) {
    husky_lens_protocol_write_uint8(text[i]);
  }
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}

bool ProtocolV2::clearText(eAlgorithm_t algo) {
  DBG("\n");
  uint8_t *buffer =
      husky_lens_protocol_write_begin(algo, COMMAND_ACTION_CLEAR_TEXT);
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}

bool ProtocolV2::saveKnowledges(eAlgorithm_t algo, uint8_t knowledgeID) {
  DBG("\n");
  uint8_t *buffer =
      husky_lens_protocol_write_begin(algo, COMMAND_ACTION_SAVE_KNOWLEDGES);
  husky_lens_protocol_write_uint8(knowledgeID);
  husky_lens_protocol_write_uint8(0);

  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_int16(0);
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}
bool ProtocolV2::loadKnowledges(eAlgorithm_t algo, uint8_t knowledgeID) {
  DBG("\n");
  uint8_t *buffer =
      husky_lens_protocol_write_begin(algo, COMMAND_ACTION_LOAD_KNOWLEDGES);
  husky_lens_protocol_write_uint8(knowledgeID);
  husky_lens_protocol_write_uint8(0);

  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_int16(0);
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}

bool ProtocolV2::playMusic(String name, int16_t volume) {
  DBG("\n");
  uint8_t *buffer =
      husky_lens_protocol_write_begin(ALGORITHM_ANY, COMMAND_ACTION_PLAY_MUSIC);
  husky_lens_protocol_write_uint8(0);
  husky_lens_protocol_write_uint8(0);

  husky_lens_protocol_write_int16(volume);
  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_uint8(name.length());
  husky_lens_protocol_write_buffer_uint8((uint8_t *)name.c_str(),
                                         name.length());
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}

bool ProtocolV2::setNameByID(eAlgorithm_t algo, uint8_t id, String name) {
  DBG("\n");
  uint8_t *buffer =
      husky_lens_protocol_write_begin(algo, COMMAND_SET_NAME_BY_ID);
  husky_lens_protocol_write_uint8(0);
  husky_lens_protocol_write_uint8(0);

  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_int16(0);
  husky_lens_protocol_write_uint8(name.length());
  husky_lens_protocol_write_buffer_uint8((uint8_t *)name.c_str(),
                                         name.length());
  int length = husky_lens_protocol_write_end();

  for (int i = 0; i < retry; i++) {
    protocolWrite(buffer, length);
    if (wait(COMMAND_RETURN_OK)) {
      return true;
    }
  }
  return false;
}