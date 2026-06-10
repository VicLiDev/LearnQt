#!/usr/bin/env python
# -*- coding: utf-8 -*-

try:
    import sys,os,datetime,time
    import psutil
    from PyQt5.QtCore import QThread
except ImportError:
    print("Error: getInfo import modules error")


class Temperature(QThread):
    def __init__(self):
        super().__init__()
        self.CpuTemp = "--"

        if not hasattr(psutil, "sensors_temperatures"):
            sys.exit("platform not supported")

        self.start()
        
    def getCpuTemp(self):
        return self.CpuTemp
    
    def run(self):
        while(True):
            temps = psutil.sensors_temperatures()
            if not temps:
                sys.exit("can't read any temperature")
                
            for name, entries in temps.items():
                if name == "coretemp":
                    self.CpuTemp = entries[0].current
                    break
                else:
                    self.CpuTemp = "--"
            
def main():
    m_temp = Temperature()
    while(True):
        import time
        time.sleep(1)
        cpuTemp = m_temp.getCpuTemp()
        print("CPU temperature: %s °C" % cpuTemp)

if __name__ == "__main__":
    main()
