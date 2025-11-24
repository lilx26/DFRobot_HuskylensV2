# -*- coding: utf-8 -*-

#实验效果：学习一次ID
#接线：使用windows或linux电脑连接一块树，树莓派主控板，哈士奇接到I2C口SCL SDA
import time
import tkinter as tk
from pinpong.board import Board
from pinpong.libs.dfrobot_huskylensv2 import *
import threading
import sys
sys.stdout.reconfigure(encoding='utf-8')

Board("UNIHIKER").begin()  #初始化

# 创建主窗口
root = tk.Tk()
root.title("面部特征绘制")
root.geometry("640x480")  # 设置窗口大小

class ProtocolThread(threading.Thread):
    def __init__(self):
        super().__init__()
        self.daemon = True

    def run(self):
        while True:
            husky = HuskylensV2_UART(tty_name="/dev/ttySP0",baudrate=115200)
            husky.knock()
            husky.switchAlgorithm(128)
            while True:
                husky.getResult(128)
                while husky.available(128):
                    result = husky.popCachedResult(128)
                    if result:
                        print("result.ID=",result.ID)
                        print("result.name=",result.name)
                    else:
                        print(None)

ProtocolThread().start()
root.mainloop()
