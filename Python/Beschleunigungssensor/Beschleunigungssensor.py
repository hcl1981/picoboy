from picoboy import PicoBoy
import time
import random
import _thread

pb = PicoBoy()

while True:
    print(pb.xAcc()*9.81)
    print(pb.yAcc()*9.81)
    print(pb.zAcc()*9.81)
    time.sleep_ms(100)
