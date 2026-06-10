# -*- coding: utf-8 -*-

try:
    import sys,os
    from PyQt5 import Qt, QtCore, QtGui, QtWidgets
    from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QDesktopWidget
except ImportError:
	print('modules not found!')
	exit()

sys.path.append(os.getcwd())

try:
    import pyqtCpuMemWidget
    import pyqtNetWidget
    import pyqtCpuTempWidget
except ImportError:
	print('modules not found!')
	exit()


class MWidget(QWidget):
    def __init__(self):
        super().__init__()
        self.setupUi()

    def setupUi(self):
        self.setWindowTitle("")
        # 设置窗口自适应
        self.setSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Preferred)
        # 设置窗口
        self.setWindowFlags(Qt.Qt.FramelessWindowHint # 去掉窗体边框
            | QtCore.Qt.Tool # 隐藏任务栏图标
            | QtCore.Qt.WindowStaysOnTopHint) # 窗口置顶
        # 设置背景透明
        self.setAttribute(Qt.Qt.WA_TranslucentBackground)
        # 去掉缝隙
        self.setContentsMargins(0, 0, 0, 0)
        # 移动窗口
        self.move(1300, 50)
        self.setStyleSheet("background-color:rgb(100, 255, 0, 255);color:rgb(255, 0, 255, 255);border:0px")
        
        # 添加组件
        self.widget_1 = pyqtCpuMemWidget.MonitorCPUMem()
        self.widget_2 = pyqtNetWidget.NetTraffic()
        self.widget_3 = pyqtCpuTempWidget.MCpuTemp()
        self.gridLayout = QtWidgets.QGridLayout(self)
        self.gridLayout.setSpacing(0)  #消除添加组件之间的缝隙
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.gridLayout.addWidget(self.widget_1, 0, 0, 1, 1)
        self.gridLayout.addWidget(self.widget_2, 1, 0, 1, 1)
        self.gridLayout.addWidget(self.widget_3, 2, 0, 1, 1)
        # self.widget_1.setFixedSize(90, 30)
        # self.widget_2.setFixedSize(115, 35)
        # self.widget_3.setFixedSize(115, 35)
        self.widget_1.setSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Preferred)
        self.widget_2.setSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Preferred)
        self.widget_3.setSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Preferred)

        # 设置label颜色及固定大小
        self.widget_1.setStyleSheet("background-color:rgb(100, 255, 0, 255);color:rgb(255, 0, 255, 255);border:0px")
        self.widget_2.setStyleSheet("background-color:rgb(100, 200, 0, 255);color:rgb(255, 0, 255, 255);border:0px")
        self.widget_3.setStyleSheet("background-color:rgb(100, 200, 0, 255);color:rgb(255, 0, 255, 255);border:0px")
        # self.widget_1.setFixedSize(130, 40)
        # self.widget_2.setFixedSize(130, 40)
        # self.widget_3.setFixedSize(130, 40)
        self.widget_1.label.installEventFilter(self)
        self.widget_1.label_2.installEventFilter(self)
        self.widget_2.label.installEventFilter(self)
        self.widget_2.label_2.installEventFilter(self)
        self.widget_3.label.installEventFilter(self)

# 以下事件函数未必在本程序中用到,放在这里只是作为学习
    def event(self, event):
        # print("this is event fun")
        return super().event(event)

    def eventFilter(self, watched, event):
        if ((watched == self.widget_1.label) or (watched == self.widget_1.label_2)
        	or (watched == self.widget_2.label) or (watched == self.widget_2.label_2)
            or (watched == self.widget_3.label)) :
            if  event.type() == Qt.QEvent.MouseButtonPress :
                # self.__mousePressPos = event.globalPos()
                self.__mouseMovePos = event.globalPos()
                return True
            return False
        else:
            return super().eventFilter(watched, event)

    def mousePressEvent(self, event):
        pass

    # 1. 相对位置：每个Qwidget都能通过pos()获取到相对自己父类窗口的位置
    # 2. 绝对位置：pWidget->mapToGlobal(QPoint(0,0)) ;将当前控件的相对位置转换为屏幕绝对位置
    # 3. 绝对位置转为相对位置： pWidget->mapFromGlobal(QPoint(0,0)), 将绝对位置对应到控件的相对位置
    def mouseMoveEvent(self, event):
        currPos = self.mapToGlobal(self.pos())
        diff = event.globalPos() - self.__mouseMovePos
        newPos = self.mapFromGlobal(currPos + diff)
        self.move(newPos)
        self.__mouseMovePos = event.globalPos()

    def keyPressEvent(self, event):
        if event.key() == Qt.Qt.Key_Escape:
        	self.close()
        	exit()


def main():
    app = QApplication(sys.argv)
    w = MWidget()
    w.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()