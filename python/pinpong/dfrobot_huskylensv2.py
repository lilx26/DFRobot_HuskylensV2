# -*- coding: utf-8 -*-
"""
Import this file to initialize ssd-object to control LCD-module.
Only support I2C-connection with  LCD-module.
Able to draw line, rectangle, circle, pixel, english and chinese text,
fill_rect, fill_circle, fill screen, etc.
"""
from pinpong.board import gboard,I2C,UART
import time
import ctypes
import queue
import math

LCD_WIDTH = 640
LCD_HEIGHT = 480

HEADER_0_INDEX = 0
HEADER_1_INDEX = 1
COMMAND_INDEX = 2
ALGO_INDEX = 3
CONTENT_SIZE_INDEX = 4
CONTENT_INDEX = 5
PROTOCOL_SIZE = 6

COMMAND_KNOCK = 0x20
COMMAND_GET_RESULT = 0x21
COMMAND_GET_INFO = 0x22
COMMAND_GET_RESULT_BY_ID = 0x23
COMMAND_GET_BLOCKS_BY_ID = 0x24
COMMAND_GET_ARROWS_BY_ID = 0x25
COMMAND_GET_SENSOR_LIST = 0x26
COMMAND_GET_RESULT_BY_INDEX = 0x27
COMMAND_GET_BLOCKS_BY_INDEX = 0x28
COMMAND_GET_ARROWS_BY_INDEX = 0x29

# RFU 0x2A - 0x2F
COMMAND_SET_ALGORITHM = 0x30
COMMAND_SET_NAME_BY_ID = 0x31
COMMAND_SET_MULTI_ALGORITHM = 0x32
COMMAND_SET_MULTI_ALGORITHM_RATIO = 0x33
COMMAND_SET_LEARN_BLOCK_POSITION = 0x34

# RFU 0x35 - 0x3F
COMMAND_RETURN_OK = 0x40
COMMAND_RETURN_ERROR = 0x41
COMMAND_RETURN_INFO = 0x42
COMMAND_RETURN_BLOCK = 0x43
COMMAND_RETURN_ARROW = 0x44
COMMAND_RETURN_SENSOR_LIST = 0x45

# RFU 0x46 - 0x4F
COMMAND_ACTION_TAKE_PHOTO = 0x50
COMMAND_ACTION_TAKE_SCREENSHOT = 0x51
COMMAND_ACTION_LEARN = 0x52
COMMAND_ACTION_FORGOT = 0x53
COMMAND_ACTION_SAVE_KNOWLEDGES = 0x54
COMMAND_ACTION_LOAD_KNOWLEDGES = 0x55
COMMAND_ACTION_DRAW_RECT = 0x56
COMMAND_ACTION_CLEAN_RECT = 0x57
COMMAND_ACTION_DRAW_TEXT = 0x58
COMMAND_ACTION_CLEAR_TEXT = 0x59
COMMAND_ACTION_PLAY_MUSIC = 0x5A

ALGORITHM_ANY = 0
ALGORITHM_FACE_RECOGNITION = 1
ALGORITHM_OBJECT_TRACKING = 2
ALGORITHM_OBJECT_RECOGNITION = 3
ALGORITHM_LINE_TRACKING = 4
ALGORITHM_COLOR_RECOGNITION = 5
ALGORITHM_TAG_RECOGNITION = 6
ALGORITHM_SELF_LEARNING_CLASSIFICATION = 7
ALGORITHM_OCR_RECOGNITION = 8
ALGORITHM_LICENSE_RECOGNITION=9
ALGORITHM_QRCODE_RECOGNITION=10
ALGORITHM_BARCODE_RECOGNITION=11
ALGORITHM_EMOTION_RECOGNITION=12
ALGORITHM_POSE_RECOGNITION=13
ALGORITHM_HAND_RECOGNITION=14
ALGORITHM_OBJECT_CLASSIFICATION=15        
ALGORITHM_BLINK_RECOGNITION=16            
ALGORITHM_GAZE_RECOGNITION=17             
ALGORITHM_FACE_ORIENTATION=18             
ALGORITHM_FALLDOWN_RECOGNITION=19         
ALGORITHM_SEGMENT=20                      
ALGORITHM_FACE_ACTION_RECOGNITION=20      
ALGORITHM_CUSTOM0=22                      
ALGORITHM_CUSTOM1=23                      
ALGORITHM_CUSTOM2=24                     
ALGORITHM_BUILTIN_COUNT=25                

ALGORITHM_CUSTOM_BEGIN = 128

CUSTOM_ALGORITHM_COUNT = 3

class UnionInt8_0(ctypes.Union):
    _fields_ = [
        ("ID", ctypes.c_int8),
        ("maxID", ctypes.c_int8),
        ("rfu0", ctypes.c_int8),
        ("boardType", ctypes.c_int8),
        ("totalSensors", ctypes.c_int8),
        ("multiAlgoNum", ctypes.c_int8),
    ]

class UnionInt8_1(ctypes.Union):
    _fields_ = [
        ("rfu1", ctypes.c_int8),
        ("level", ctypes.c_int8),
        ("confidence", ctypes.c_int8),
    ]

class UnionInt16_0(ctypes.Union):
    _fields_ = [
        ("first", ctypes.c_int16),
        ("xCenter", ctypes.c_int16),
        ("xTarget", ctypes.c_int16),
        ("algorithmType", ctypes.c_int16),
        ("sensor0ID", ctypes.c_int16),
        ("total_results", ctypes.c_int16),
    ]

class UnionInt16_1(ctypes.Union):
    _fields_ = [
        ("second", ctypes.c_int16),
        ("yCenter", ctypes.c_int16),
        ("yTarget", ctypes.c_int16),
        ("sensor1ID", ctypes.c_int16),
        ("total_results_learned", ctypes.c_int16),
    ]

class UnionInt16_2(ctypes.Union):
    _fields_ = [
        ("third", ctypes.c_int16),
        ("width", ctypes.c_int16),
        ("angle", ctypes.c_int16),
        ("sensor2ID", ctypes.c_int16),
        ("total_blocks", ctypes.c_int16),
    ]

class UnionInt16_3(ctypes.Union):
    _fields_ = [
        ("fourth", ctypes.c_int16),
        ("height", ctypes.c_int16),
        ("length", ctypes.c_int16),
        ("total_blocks_learned", ctypes.c_int16),
    ]

class PacketData_t(ctypes.Structure):
    _pack_ = 1
    _anonymous_ = ("u0", "u1", "u2", "u3", "u4", "u5")  # 关键点：匿名 union
    _fields_ = [
        ("u0", UnionInt8_0),
        ("u1", UnionInt8_1),
        ("u2", UnionInt16_0),
        ("u3", UnionInt16_1),
        ("u4", UnionInt16_2),
        ("u5", UnionInt16_3),
    ]
    def __init__(self, buf):
        super().__init__()
        
class Result(PacketData_t):
    def __init__(self, buf):
        super().__init__(buf)
        self.nameLength = 0
        self.contentLength = 0
        self.data =  buf
        self.name = ""
        self.content = ""
        self.algo = buf[ALGO_INDEX]
        self.dataLength = buf[CONTENT_SIZE_INDEX]
        if self.dataLength > 10:
            if len(buf) > CONTENT_INDEX + 10:
                self.nameLength = buf[CONTENT_INDEX + 10]
            if len(buf) > CONTENT_INDEX + 10 + 1 + self.nameLength:
                self.contentLength = buf[CONTENT_INDEX + 10 + 1 + self.nameLength]
        self.ID = buf[CONTENT_INDEX]
        self.level = buf[CONTENT_INDEX + 1]
        self.first = buf[CONTENT_INDEX + 2] + buf[CONTENT_INDEX + 3] * 256
        self.second = buf[CONTENT_INDEX + 4] + buf[CONTENT_INDEX + 5] * 256
        self.third = buf[CONTENT_INDEX + 6] + buf[CONTENT_INDEX + 7] * 256
        self.fourth = buf[CONTENT_INDEX + 8] + buf[CONTENT_INDEX + 9] * 256
        self.used = False
        if self.nameLength > 0:
            sub = buf[CONTENT_INDEX + 11:CONTENT_INDEX + 11 + self.nameLength]
            sub_bytes = bytes(sub) 
            self.name = sub_bytes.decode('utf-8', errors='ignore')
        if self.contentLength > 0:
            sub = buf[CONTENT_INDEX + 12 + self.nameLength:CONTENT_INDEX + 12 + self.nameLength+self.contentLength]
            sub_bytes = bytes(sub) 
            self.content = sub_bytes.decode('utf-8', errors='ignore')

        #print("algo =", self.algo)
        #print("dataLength =", self.dataLength)
        #print("ID =", self.ID)
        #print("level =",self.level)
        #print("maxID =",self.maxID)
        #print("rfu1 =",self.rfu1)
        #print("xCenter =",self.xCenter)
        #print("yCenter =",self.yCenter)
        #print("width =",self.width)
        #print("height =",self.height)
        #print("nameLength=",self.nameLength)
        #print("contentLength=",self.contentLength)
        #print("name=",self.name)
        #print("content=",self.content)
class FaceResult(Result):
    def __init__(self, buf):
        super().__init__(buf)
        data_index =CONTENT_INDEX + 12 + self.nameLength + self.contentLength;
        self.leye_x = buf[data_index] + buf[data_index+1]*256
        self.leye_y = buf[data_index+2] + buf[data_index+3]*256
        self.reye_x = buf[data_index+4] + buf[data_index+5]*256
        self.reye_y = buf[data_index+6] + buf[data_index+7]*256
        self.nose_x = buf[data_index+8] + buf[data_index+9]*256
        self.nose_y = buf[data_index+10] + buf[data_index+11]*256
        self.lmouth_x = buf[data_index+12] + buf[data_index+13]*256
        self.lmouth_y = buf[data_index+14] + buf[data_index+15]*256
        self.rmouth_x = buf[data_index+16] + buf[data_index+17]*256
        self.rmouth_y = buf[data_index+18] + buf[data_index+19]*256

class HandResult(Result):
    def __init__(self, buf):
        super().__init__(buf)
        data_index =CONTENT_INDEX + 12 + self.nameLength + self.contentLength;
        self.wrist_x = buf[data_index] + buf[data_index+1]*256
        self.wrist_y = buf[data_index+2] + buf[data_index+3]*256
        self.thumb_cmc_x = buf[data_index+4] + buf[data_index+5]*256
        self.thumb_cmc_y = buf[data_index+6] + buf[data_index+7]*256
        self.thumb_mcp_x = buf[data_index+8] + buf[data_index+9]*256
        self.thumb_mcp_y = buf[data_index+10] + buf[data_index+11]*256
        self.thumb_ip_x = buf[data_index+12] + buf[data_index+13]*256
        self.thumb_ip_y = buf[data_index+14] + buf[data_index+15]*256
        self.thumb_tip_x = buf[data_index+16] + buf[data_index+17]*256
        self.thumb_tip_y = buf[data_index+18] + buf[data_index+19]*256

        data_index = data_index + 20
        self.index_finger_mcp_x = buf[data_index] + buf[data_index+1]*256
        self.index_finger_mcp_y = buf[data_index+2] + buf[data_index+3]*256
        self.index_finger_pip_x = buf[data_index+4] + buf[data_index+5]*256
        self.index_finger_pip_y = buf[data_index+6] + buf[data_index+7]*256
        self.index_finger_dip_x = buf[data_index+8] + buf[data_index+9]*256
        self.index_finger_dip_y = buf[data_index+10] + buf[data_index+11]*256
        self.index_finger_tip_x = buf[data_index+12] + buf[data_index+13]*256
        self.index_finger_tip_y = buf[data_index+14] + buf[data_index+15]*256
        
        data_index = data_index + 16
        self.middle_finger_mcp_x = buf[data_index] + buf[data_index+1]*256
        self.middle_finger_mcp_y = buf[data_index+2] + buf[data_index+3]*256
        self.middle_finger_pip_x = buf[data_index+4] + buf[data_index+5]*256
        self.middle_finger_pip_y = buf[data_index+6] + buf[data_index+7]*256
        self.middle_finger_dip_x = buf[data_index+8] + buf[data_index+9]*256
        self.middle_finger_dip_y = buf[data_index+10] + buf[data_index+11]*256
        self.middle_finger_tip_x = buf[data_index+12] + buf[data_index+13]*256
        self.middle_finger_tip_y = buf[data_index+14] + buf[data_index+15]*256
        
        data_index = data_index + 16
        self.ring_finger_mcp_x = buf[data_index] + buf[data_index+1]*256
        self.ring_finger_mcp_y = buf[data_index+2] + buf[data_index+3]*256
        self.ring_finger_pip_x = buf[data_index+4] + buf[data_index+5]*256
        self.ring_finger_pip_y = buf[data_index+6] + buf[data_index+7]*256
        self.ring_finger_dip_x = buf[data_index+8] + buf[data_index+9]*256
        self.ring_finger_dip_y = buf[data_index+10] + buf[data_index+11]*256
        self.ring_finger_tip_x = buf[data_index+12] + buf[data_index+13]*256
        self.ring_finger_tip_y = buf[data_index+14] + buf[data_index+15]*256
        
        data_index = data_index + 16
        self.pinky_finger_mcp_x = buf[data_index] + buf[data_index+1]*256
        self.pinky_finger_mcp_y = buf[data_index+2] + buf[data_index+3]*256
        self.pinky_finger_pip_x = buf[data_index+4] + buf[data_index+5]*256
        self.pinky_finger_pip_y = buf[data_index+6] + buf[data_index+7]*256
        self.pinky_finger_dip_x = buf[data_index+8] + buf[data_index+9]*256
        self.pinky_finger_dip_y = buf[data_index+10] + buf[data_index+11]*256
        self.pinky_finger_tip_x = buf[data_index+12] + buf[data_index+13]*256
        self.pinky_finger_tip_y = buf[data_index+14] + buf[data_index+15]*256

class PoseResult(Result):
    def __init__(self, buf):
        super().__init__(buf)
        data_index =CONTENT_INDEX + 12 + self.nameLength + self.contentLength;
        self.nose_x = buf[data_index] + buf[data_index+1] * 256
        self.nose_y = buf[data_index+2] + buf[data_index+3] * 256
        self.leye_x = buf[data_index+4] + buf[data_index+5] * 256
        self.leye_y = buf[data_index+6] + buf[data_index+7] * 256
        self.reye_x = buf[data_index+8] + buf[data_index+9] * 256
        self.reye_y = buf[data_index+10] + buf[data_index+11] * 256
        self.lear_x = buf[data_index+12] + buf[data_index+13] * 256
        self.lear_y = buf[data_index+14] + buf[data_index+15] * 256
        self.rear_x = buf[data_index+16] + buf[data_index+17] * 256
        self.rear_y = buf[data_index+18] + buf[data_index+19] * 256


        data_index = data_index + 20
        self.lshoulder_x = buf[data_index] + buf[data_index+1]*256
        self.lshoulder_y = buf[data_index+2] + buf[data_index+3]*256
        self.rshoulder_x = buf[data_index+4] + buf[data_index+5]*256
        self.rshoulder_y = buf[data_index+6] + buf[data_index+7]*256
        self.lelbow_x = buf[data_index+8] + buf[data_index+9]*256
        self.lelbow_y = buf[data_index+10] + buf[data_index+11]*256
        self.relbow_x = buf[data_index+12] + buf[data_index+13]*256
        self.relbow_y = buf[data_index+14] + buf[data_index+15]*256
        self.lwrist_x = buf[data_index+16] + buf[data_index+17]*256
        self.lwrist_y = buf[data_index+18] + buf[data_index+19]*256
        

        data_index = data_index + 20
        self.rwrist_x = buf[data_index] + buf[data_index+1]*256
        self.rwrist_y = buf[data_index+2] + buf[data_index+3]*256
        self.lhip_x = buf[data_index+4] + buf[data_index+5]*256
        self.lhip_y = buf[data_index+6] + buf[data_index+7]*256
        self.rhip_x = buf[data_index+8] + buf[data_index+9]*256
        self.rhip_y = buf[data_index+10] + buf[data_index+11]*256
        self.lknee_x = buf[data_index+12] + buf[data_index+13]*256
        self.lknee_y = buf[data_index+14] + buf[data_index+15]*256
        self.rknee_x = buf[data_index+16] + buf[data_index+17]*256
        self.rknee_y = buf[data_index+18] + buf[data_index+19]*256

        data_index = data_index + 20
        self.lankle_x = buf[data_index] + buf[data_index+1]*256
        self.lankle_y = buf[data_index+2] + buf[data_index+3]*256
        self.rankle_x = buf[data_index+4] + buf[data_index+5]*256
        self.rankle_y = buf[data_index+6] + buf[data_index+7]*256

class ProtocolV2(object):
    def __init__(self):
        ERROR_COUNT = 0x05
        self.FRAME_BUFFER_SIZE = 1024
        self.receive_index = HEADER_0_INDEX
        self.receive_buffer = []
        self.connect = False
        self.commandHeader = [0x55,0xAA]
        self.customId=[None,None,None]
        self.result = {}
        self.send_buffer=[0]*128
        for i in range(0, ALGORITHM_BUILTIN_COUNT):
            self.result[i] = {"algo":i,"info":None,"blocks":[]}

    def rgb888_to_rgb565(self, rgb888:int):
        r = (rgb888 >> 16) & 0xFF  # 取高8位 (RR)
        g = (rgb888 >> 8)  & 0xFF  # 取中间8位 (GG)
        b = rgb888 & 0xFF          # 取低8位 (BB)
        
        # 将各分量从8位缩减到目标位数 (R:5, G:6, B:5) 并组合
        r_5 = (r >> 3) & 0x1F
        g_6 = (g >> 2) & 0x3F
        b_5 = (b >> 3) & 0x1F
        rgb565 = (r_5 << 11) | (g_6 << 5) | b_5
        
        return rgb565

    def toStoreAlgoIndex(self, algo:int):
        if algo >= ALGORITHM_CUSTOM_BEGIN:
            for i in range(0,len(self.customId)):
                if self.customId[i] == algo:
                    algo = ALGORITHM_CUSTOM0 + i
                    break;
        return algo

    def print_hex(self,cmd):
        hex_cmd = [hex(x) for x in cmd]
        # print(hex_cmd)

    def checksum(self,cmd):
        cs = 0
        for x in cmd:
            cs = cs + x
        return cs & 0xff

    def knock(self):
        self.husky_lens_protocol_write_begin(ALGORITHM_ANY, COMMAND_KNOCK)
        self.husky_lens_protocol_write_uint8(1)
        self.husky_lens_protocol_write_uint8(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_end()
        if self.executeCommand(wait_cmd=COMMAND_RETURN_OK) == False:
            return False
        return True

    def getResult(self, algo):
        self.husky_lens_protocol_write_begin(ALGORITHM_ANY, COMMAND_GET_RESULT)
        self.husky_lens_protocol_write_end()

        if self.executeCommand(wait_cmd=COMMAND_RETURN_INFO) == False:
            time.sleep(1)
            return None
        #print("getResult")
        #self.print_hex(self.receive_buffer[0:self.receive_index+1])

        #if self.receive_buffer[ALGO_INDEX] !=  algo:
            #读出所有缓存的错误数据
        #    print(f"self.receive_buffer[ALGO_INDEX]={self.receive_buffer[ALGO_INDEX]} algo={algo}")
        #    return None
        self.result[algo]["info"] = Result(self.receive_buffer[0:self.receive_index+1])
        self.result[algo]["info"].total_arrows = self.result[algo]["info"].total_results - self.result[algo]["info"].total_blocks
        self.result[algo]["info"].total_arrows_learned = self.result[algo]["info"].total_results_learned - self.result[algo]["info"].total_blocks_learned
        #print("info.maxID",self.result[algo]["info"].maxID)
        #print("info.total_results",self.result[algo]["info"].total_results)
        #print("info.total_results_learned",self.result[algo]["info"].total_results_learned)
        #print("info.total_blocks",self.result[algo]["info"].total_blocks)
        #print("info.total_blocks_learned",self.result[algo]["info"].total_blocks_learned)
        
        self.result[algo]["blocks"] = []

        for i in range(0,self.result[algo]["info"].total_blocks):
            if self.wait(COMMAND_RETURN_BLOCK) == False:
                return None
            #algo1 = self.receive_buffer[ALGO_INDEX]
            #print(f"L:{L},algo:{algo}")
            L = self.receive_buffer[CONTENT_SIZE_INDEX] + PROTOCOL_SIZE
            if algo == ALGORITHM_FACE_RECOGNITION:
                ret = FaceResult(self.receive_buffer[0:L])
            elif algo == ALGORITHM_POSE_RECOGNITION:
                ret = PoseResult(self.receive_buffer[0:L])
            elif algo == ALGORITHM_HAND_RECOGNITION:
                ret = HandResult(self.receive_buffer[0:L])
            else:
                ret = Result(self.receive_buffer[0:L])
            self.result[algo]["blocks"].append(ret)

        for i in range(0, self.result[algo]["info"].total_arrows):
            if self.wait(COMMAND_RETURN_ARROW) == False:
                return None
            L = self.receive_buffer[CONTENT_SIZE_INDEX] + PROTOCOL_SIZE
            if algo == ALGORITHM_LINE_TRACKING:
                ret = Result(self.receive_buffer[0:L])

            self.result[algo]["blocks"].append(ret)

    def wait(self, command):
        receiving = True
        self.receive_buffer = [0] * 1024
        self.receive_index = HEADER_0_INDEX
        start_ms = time.time_ns() // 1_000_000
        while receiving:
            now_ms = time.time_ns() // 1_000_000
            if now_ms - start_ms > 500:
                break
            c = self._read_from_huskyLens()
            if c == None:
                time.sleep(0.01)
                continue
            if self.husky_lens_protocol_receive(c):
                receiving = False
        if receiving:
            return False
        return command == self.receive_buffer[COMMAND_INDEX]

    def husky_lens_protocol_receive(self, data): 
        #print("self.receive_index=",self.receive_index, data)
        if self.receive_index ==  HEADER_0_INDEX:
            if data != 0x55:
                self.receive_index = HEADER_0_INDEX
                #time.sleep(100)
                return False
            self.receive_buffer[self.receive_index] = 0x55
        elif self.receive_index ==  HEADER_1_INDEX:
            if data != 0xaa:
                self.receive_index = HEADER_0_INDEX
                return False
            self.receive_buffer[self.receive_index] = 0xaa
        elif self.receive_index ==  COMMAND_INDEX:
            self.receive_buffer[self.receive_index] = data
        elif self.receive_index ==  ALGO_INDEX:
            self.receive_buffer[self.receive_index] = data
        elif self.receive_index ==  CONTENT_SIZE_INDEX:
            if (self.receive_index >= self.FRAME_BUFFER_SIZE - PROTOCOL_SIZE):
                self.receive_index = 0;
                return False
            self.receive_buffer[self.receive_index] = data
        else:
            self.receive_buffer[self.receive_index] = data
            if (self.receive_index == self.receive_buffer[CONTENT_SIZE_INDEX] + CONTENT_INDEX):
                #print("<--------self.receive_index=",self.receive_index)
                #self.print_hex(self.receive_buffer[0:self.receive_index+1])
                cs = self.checksum(self.receive_buffer[0:self.receive_index])
                #print("calc checksum = ", hex(cs))
                #print("protocol checksum = ", hex(self.receive_buffer[self.receive_index]))
                return cs == self.receive_buffer[self.receive_index]
        self.receive_index = self.receive_index + 1
        return False

    def available(self, algo):
        ret = False
        for i in range(0, len(self.result[algo]["blocks"])):
            if self.result[algo]["blocks"][i].used == False:
                ret = True;
                break;
        return ret

    def husky_lens_protocol_write_begin(self, algo, command):
        self.send_buffer = [0] * len(self.send_buffer)
        self.send_buffer[HEADER_0_INDEX] = 0x55;
        self.send_buffer[HEADER_1_INDEX] = 0xAA;
        self.send_buffer[COMMAND_INDEX] = command;
        self.send_buffer[ALGO_INDEX] = algo;
        self.send_index = CONTENT_INDEX;

    def husky_lens_protocol_write_uint8(self, content):
        self.send_buffer[self.send_index] = content
        self.send_index = self.send_index + 1

    def husky_lens_protocol_write_int16(self, content):
        self.send_buffer[self.send_index] = content&0xFF
        self.send_buffer[self.send_index+1] = (content>>8)&0xFF
        self.send_index = self.send_index + 2

    def husky_lens_protocol_write_end(self):
        self.send_buffer[CONTENT_SIZE_INDEX] = self.send_index - CONTENT_INDEX
        cs = 0
        for i in range(0, self.send_index):
            cs = cs + self.send_buffer[i]
        self.send_buffer[self.send_index] = cs & 0xFF
        self.send_index = self.send_index + 1

    '''
    def getResultByID(self, algo, ID):
        self.husky_lens_protocol_write_begin(algo, COMMAND_GET_RESULT)
        self.husky_lens_protocol_write_end()
        
        self._write_to_huskyLens(self.send_buffer[0, self.send_index])
        return self.wait(COMMAND_RETURN_OK)
    '''
    def setLearningRectPosition(self, algo, x1,  y1, x2,  y2):
        self.husky_lens_protocol_write_begin(algo, COMMAND_SET_LEARN_BLOCK_POSITION);
        self.husky_lens_protocol_write_uint8(0)
        self.husky_lens_protocol_write_uint8(0)
        self.husky_lens_protocol_write_int16(x1)
        self.husky_lens_protocol_write_int16(y1)
        self.husky_lens_protocol_write_int16(x2)
        self.husky_lens_protocol_write_int16(y2)
        self.husky_lens_protocol_write_end()
        
        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)
    
    def switchAlgorithm(self,algo):
        self.husky_lens_protocol_write_begin(ALGORITHM_ANY, COMMAND_SET_ALGORITHM);
        self.husky_lens_protocol_write_uint8(algo);
        self.husky_lens_protocol_write_uint8(0);
        self.husky_lens_protocol_write_int16(0);
        self.husky_lens_protocol_write_int16(0);
        self.husky_lens_protocol_write_int16(0);
        self.husky_lens_protocol_write_int16(0);
        self.husky_lens_protocol_write_end();
      
        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)

    def takePhoto(self):
        self.husky_lens_protocol_write_begin(ALGORITHM_ANY, COMMAND_ACTION_TAKE_PHOTO)
        self.husky_lens_protocol_write_end()
      
        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)

    def takeScreenshot(self):
        self.husky_lens_protocol_write_begin(ALGORITHM_ANY, COMMAND_ACTION_TAKE_SCREENSHOT)
        self.husky_lens_protocol_write_end()
        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)

    def learn(self,algo):
        self.husky_lens_protocol_write_begin(algo, COMMAND_ACTION_LEARN)
        self.husky_lens_protocol_write_end()
        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)

    def forgot(self,algo):
        self.husky_lens_protocol_write_begin(algo, COMMAND_ACTION_FORGOT)
        self.husky_lens_protocol_write_end()

        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)

    def drawRect(self, algo:int, color:int, x1:int, y1:int, x2:int, y2:int):
        color = self.rgb888_to_rgb565(color)
        self.husky_lens_protocol_write_begin(algo, COMMAND_ACTION_DRAW_RECT)
        self.husky_lens_protocol_write_int16(color)
        self.husky_lens_protocol_write_int16(x1)
        self.husky_lens_protocol_write_int16(y1)
        self.husky_lens_protocol_write_int16(x2)
        self.husky_lens_protocol_write_int16(y2)
        self.husky_lens_protocol_write_end();
      
        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)

    def clearRect(self, algo : int):
        self.husky_lens_protocol_write_begin(algo, COMMAND_ACTION_CLEAN_RECT)
        self.husky_lens_protocol_write_end()

        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)

    def drawText(self, algo : int, color : int, x : int, y : int, text : str):
        utf8_bytes = text.encode('utf-8')
        color = self.rgb888_to_rgb565(color)
        self.husky_lens_protocol_write_begin(algo, COMMAND_ACTION_DRAW_TEXT);
        self.husky_lens_protocol_write_int16(color);
      
        self.husky_lens_protocol_write_int16(x)
        self.husky_lens_protocol_write_int16(y)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_uint8(len(utf8_bytes))
        # print("len=",len(utf8_bytes))
        for c in utf8_bytes:
          # print(c)
          self.husky_lens_protocol_write_uint8(c)
        self.husky_lens_protocol_write_end();

        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)
        
    def clearText(self, algo : int):
        self.husky_lens_protocol_write_begin(algo, COMMAND_ACTION_CLEAR_TEXT);
        self.husky_lens_protocol_write_end()
        
        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)

    def saveKnowledges(self, algo : int, knowledgeID : int):
        self.husky_lens_protocol_write_begin(algo, COMMAND_ACTION_SAVE_KNOWLEDGES);
        self.husky_lens_protocol_write_uint8(knowledgeID);
        self.husky_lens_protocol_write_uint8(0)
        
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_end()

        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)

    def loadKnowledges(self, algo : int, knowledgeID : int):
        self.husky_lens_protocol_write_begin(algo, COMMAND_ACTION_LOAD_KNOWLEDGES)
        self.husky_lens_protocol_write_uint8(knowledgeID);
        self.husky_lens_protocol_write_uint8(0)

        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_end()

        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)

    def playMusic(self, name:str, volume:int):
        utf8_bytes = name.encode('utf-8')
        self.husky_lens_protocol_write_begin(ALGORITHM_ANY, COMMAND_ACTION_PLAY_MUSIC);
        self.husky_lens_protocol_write_uint8(0)
        self.husky_lens_protocol_write_uint8(0)
      
        self.husky_lens_protocol_write_int16(volume)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_uint8(len(utf8_bytes))
        for c in utf8_bytes:
            self.husky_lens_protocol_write_uint8(c)
        self.husky_lens_protocol_write_end()
      
        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)

    def setNameByID(self, algo : int, ID : int, name: str):
        utf8_bytes = name.encode('utf-8')
        self.husky_lens_protocol_write_begin(algo, COMMAND_SET_NAME_BY_ID);
        self.husky_lens_protocol_write_uint8(ID)
        self.husky_lens_protocol_write_uint8(0)
        
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_uint8(len(utf8_bytes))
        for c in utf8_bytes:
            self.husky_lens_protocol_write_uint8(c)
        self.husky_lens_protocol_write_end()
        
        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)

    def setMultiAlgorithm(self, algos:list):
        customAlgoNum = 0;
        self.customId = [None,None,None]
        for algo in algos:
            if algo >= ALGORITHM_CUSTOM_BEGIN:
                self.customId[customAlgoNum] = algo0
                customAlgoNum = + customAlgoNum + 1

        self.husky_lens_protocol_write_begin(ALGORITHM_ANY, COMMAND_SET_MULTI_ALGORITHM);
        self.husky_lens_protocol_write_uint8(len(algos))
        self.husky_lens_protocol_write_uint8(0)

        for algo in algos:
            self.husky_lens_protocol_write_int16(algo)
        for _ in range(0, 4-len(algos)):
            self.husky_lens_protocol_write_int16(0)
        self.husky_lens_protocol_write_end()

        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)

    def setMultiAlgorithmRatio(self,ratios : list):
        self.husky_lens_protocol_write_begin(ALGORITHM_ANY, COMMAND_SET_MULTI_ALGORITHM_RATIO);
        self.husky_lens_protocol_write_uint8(len(ratios))
        self.husky_lens_protocol_write_uint8(0)

        for ratio in ratios:
            self.husky_lens_protocol_write_int16(ratio)
        for _ in range(0, 4-len(ratios)):
            self.husky_lens_protocol_write_int16(0xFFFF)

        self.husky_lens_protocol_write_end()
        
        return self.executeCommand(wait_cmd=COMMAND_RETURN_OK)
    
    def executeCommand(self, wait_cmd):
        for _ in range(0,3):
            self._write_to_huskyLens()
            if self.wait(wait_cmd):
                return True
        return False

class HuskylensV2(ProtocolV2):
    def __init__(self):
        super().__init__()
    
    def begin(self):
        if self.knock():
            return True
        else:
            return False
        
    def popCachedResult(self, algo):
        for i in range(0,len(self.result[algo]["blocks"])):
            if self.result[algo]["blocks"][i].used:
                continue;
            self.result[algo]["blocks"][i].used = True;
            return self.result[algo]["blocks"][i];
        return None

    def getCachedCenterResult(self, algo):
        centerIndex = -1;
        minLen = 0x0FFFFFFF;
        for i in range(0,len(self.result[algo]["blocks"])):
            length = math.pow(self.result[algo]["blocks"][i].xCenter - LCD_WIDTH / 2,2) + \
                          math.pow(self.result[algo]["blocks"][i].yCenter - LCD_HEIGHT / 2,2);
            if length < minLen:
                minLen = length
                centerIndex = i;
        if centerIndex != -1:
            return self.result[algo]["blocks"][centerIndex]
        return None

    def getCachedResultByIndex(self, algo, index):
        if index >= len(self.result[algo]["blocks"]):
          return None
        return self.result[algo]["blocks"][index]

    def getCachedResultByID(self, algo, ID):
        # print("len(self.result[algo][blocks])=",len(self.result[algo]["blocks"]))
        for i in range(0,len(self.result[algo]["blocks"])):
            if self.result[algo]["blocks"][i].ID == ID:
                return self.result[algo]["blocks"][i]
        return None

    def getCachedResultNum(self, algo):
        return len(self.result[algo]["blocks"])

    def getCachedResultLearnedNum(self, algo):
        count = 0
        for i in range(0,len(self.result[algo]["blocks"])):
            if self.result[algo]["blocks"][i].ID != 0:
              count = count + 1
        return count
    
    def getCachedResultNumByID(self, algo, ID):
        count = 0
        for i in range(0,len(self.result[algo]["blocks"])):
            if ID == self.result[algo]["blocks"][i].ID:
                count = count + 1
        return count

    def getCachedIndexResultByID(self, algo:int, ID:int, index:int):
        # print("len(self.result[algo][blocks]=",len(self.result[algo]["blocks"]))
        _index = 0
        for i in range(0,len(self.result[algo]["blocks"])):
          # print("i=",i," ID=",self.result[algo]["blocks"][i].ID)
          if ID == self.result[algo]["blocks"][i].ID:
            if index == _index:
              # print(f"index={index}  i={i}")
              return self.result[algo]["blocks"][i]
            _index = _index + 1
        return None

    def getCachedResultMaxID(self,algo:int):
        return self.result[algo]["info"].maxID

    def getCurrentBranch(self, algo: int, attr: str):
        blocks = self.result[algo]["blocks"]
        # print("len(blocks)",len(blocks))
        if len(blocks) == 0:
            return 0
        if blocks[0].level == 1:
            return getattr(blocks[0], attr, 0)
        return 0

    def getUpcomingBranchCount(self, algo:int):
        count =  len(self.result[algo]["blocks"]) - 1
        if count < 0:
            count = 0
        return count

    def getBranch(self, algo: int, index: int, attr: str):
        blocks = self.result[algo]["blocks"]
        if len(blocks) - 1 - index > 0:
            return getattr(blocks[1 + index], attr, 0)
        return 0
    def createResult(self):
        return Result([0]*16)
class HuskylensV2_I2C(HuskylensV2):
    def __init__(self, board = None, bus_num=0):
        if isinstance(board, int):
            board = gboard
        elif board is None:
            board = gboard
        self._connect = 0
        self.board = board
        self.i2c = I2C(bus_num)
        self.i2c_addr = 0x50
        self.q = queue.Queue()
        super().__init__()

    def _write_to_huskyLens(self):
        self._connect = 0
        command = self.send_buffer[0:self.send_index]
        while True:
            try:
                self.i2c.writeto(self.i2c_addr, command)
                #print("_write_to_huskyLens ----->")
                #self.print_hex(command)
                time.sleep(0.5)
                return
            except:
                self._error_handling()
            if self._connect > self.ERROR_COUNT:
                raise ValueError("Please check the huskylens connection or Reconnection sensor!!!")

    def _read_from_huskyLens(self):
        self._connect = 0
        if self.q.empty():
            d = self.i2c.readfrom(self.i2c_addr, 32)
            for c in d:
                self.q.put(c)
        return self.q.get()

class HuskylensV2_UART(HuskylensV2):
    def __init__(self,board = None, tty_name="/dev/ttyS0", baudrate=115200):
        if isinstance(board, str):
            tty_name  = board
            board     = gboard
        elif board is None:
            board    = gboard
        self.q = queue.Queue()
        self.uart  = UART(tty_name=tty_name)
        self.uart.init(baud_rate = baudrate, bits=8, parity=0, stop = 1) 
        super().__init__()
  
    def _write_to_huskyLens(self):
        self._connect = 0
        command = self.send_buffer[0:self.send_index]
        while True:
            try:
                self.uart.write(command)
                #print("_write_to_huskyLens ----->")
                #self.print_hex(command)
                time.sleep(0.1)
                return
            except:
                self._error_handling()
            if self._connect > self.ERROR_COUNT:
                raise ValueError("Please check the huskylens connection or Reconnection sensor!!!")

    def _read_from_huskyLens(self):
        self._connect = 0
        if self.q.empty():
            d = self.uart.read(32)
            for c in d:
                self.q.put(c)
        if self.q.empty():
            return None
        return self.q.get()