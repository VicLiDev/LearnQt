# -*- coding: utf-8 -*-
# 如果想写中文注释需要加上上边这句
# 先下载psutil库:pip install psutil
try:
    import os,datetime,time
    import psutil
    from PyQt5.QtCore import QThread
except ImportError:
    print("Error: getInfo import modules error")

class CpuMemInfo(QThread):
    def __init__(self):
        super().__init__()
        self.CpuInfo = "--"
        self.MemInfo = "--"
        self.start()
        
    def getCpuInfo(self):
        return self.CpuInfo

    def getMemInfo(self):
        return self.MemInfo
    
    def run(self):
        while(True):
            # CpuInfo
            self.CpuInfo = "%0.2f" %psutil.cpu_percent(interval=1)
            # MemInfo
            data = psutil.virtual_memory()
            total = data.total  #总内存,单位为byte
            free = data.available #可用内存
            self.MemInfo = "%d" %(int(round(data.percent)))
            
def main():
    m_CpuMemInfo = CpuMemInfo()
    while(True):
        Cpuinfo = "CPU: " + str(m_CpuMemInfo.getCpuInfo()) + "%"
        Meminfo = "Mem: " + str(m_CpuMemInfo.getMemInfo()) + "%"
        time.sleep(0.2)
        print ("%-10s\t%s" % (Cpuinfo, Meminfo))

if __name__ == "__main__":
    main()
