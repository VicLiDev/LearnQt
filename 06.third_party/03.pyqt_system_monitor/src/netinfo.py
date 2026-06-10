#!/usr/bin/env python
# -*- coding: utf-8 -*-

# 程序来源:
# http://www.py3study.com/Article/details/id/347.html

# 网上有资料说python3会自动对变量加锁,不需要手动加锁,这里仍然保留以作学习

try:
    import threading
    import time
    import psutil
    from PyQt5.QtCore import QThread
except ImportError:
    print('Error: psutil module not found!')
    exit()

class NetTraffic(QThread):
    def __init__(self):
        super().__init__()
        self.keyInfo = {}
        self.netIn = {}
        self.netOut = {}
        self.netInSum = float(0)
        self.netOutSum = float(0)
        self.m_lock = threading.Lock()
        self.start()

    def getKey(self):
        keyInfo = psutil.net_io_counters(pernic=True).keys()
        
        recv = {}
        sent = {}
        recvSum = {}
        sentSum = {}
        
        for key in keyInfo:
            recv.setdefault(key, psutil.net_io_counters(pernic=True).get(key).bytes_recv)
            sent.setdefault(key, psutil.net_io_counters(pernic=True).get(key).bytes_sent)
            recvSum = psutil.net_io_counters().bytes_recv
            sentSum = psutil.net_io_counters().bytes_sent
            
        return keyInfo, recv, sent, recvSum, sentSum
    
    def run(self):
        while(True):
            self.m_lock.acquire()
            self.keyInfo, old_recv, old_sent, old_recvSum, old_sentSum = self.getKey()
            self.m_lock.release()
            
            time.sleep(1)
            
            self.m_lock.acquire()
            self.keyInfo, now_recv, now_sent, now_recvSum, now_sentSum = self.getKey()

            for key in self.keyInfo:
                # float('%.2f' % a)
                self.netIn.setdefault(key, float('%.2f' %((now_recv.get(key) - old_recv.get(key)) / 1024)))
                self.netOut.setdefault(key, float('%.2f' %((now_sent.get(key) - old_sent.get(key)) / 1024)))
                self.netInSum = float('%.2f' %((now_recvSum - old_recvSum) / 1024))
                self.netOutSum = float('%.2f' %((now_sentSum - old_sentSum) / 1024))
            self.m_lock.release()
                
    def getRate(self):
        self.m_lock.acquire()
        keyInfoTmp = self.keyInfo
        netInTmp = self.netIn
        netOutTmp = self.netOut
        netInSumTmp = self.netInSum
        netOutSumTmp = self.netOutSum
        self.m_lock.release()
        return keyInfoTmp, netInTmp, netOutTmp, netInSumTmp, netOutSumTmp


def main():
    m_netTraffic = NetTraffic()
    while 1:
        time.sleep(1)
        try:
            key_info, net_in, net_out, net_inSum, net_outSum = m_netTraffic.getRate()
            for key in key_info:
            # lo 是linux的本机回环网卡，以太网是我win10系统的网卡名
                if key != 'lo' or key == '以太网':
                    print("%-9sInput:\t %-5sKB/s\tOutput:\t %-5sKB/s" % (key+":", net_in.get(key), net_out.get(key)))
            print("%-9sInput:\t %-5sKB/s\tOutput:\t %-5sKB/s" % ("==>All:", net_inSum, net_outSum))
        except KeyboardInterrupt:
            exit()

if __name__ == "__main__":
    main()
