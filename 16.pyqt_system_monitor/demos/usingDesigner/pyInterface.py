# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'Interface.ui'
#
# Created by: PyQt5 UI code generator 5.5.1
#
# WARNING! All changes made in this file will be lost!

import sys,os
from PyQt5 import QtCore, QtGui, QtWidgets

sys.path.append(os.getcwd())
import getInfo

class Ui_MainWindow(object):

    def __init__(self):
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.timeOut)

    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(150, 50)
        MainWindow.setWindowTitle("MainWindow")
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.gridLayout = QtWidgets.QGridLayout(self.centralwidget)
        self.gridLayout.setObjectName("gridLayout")
        self.horizontalLayout = QtWidgets.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.label = QtWidgets.QLabel(self.centralwidget)
        self.label.setText("")
        self.label.setObjectName("label")
        self.horizontalLayout.addWidget(self.label)
        self.label_2 = QtWidgets.QLabel(self.centralwidget)
        self.label_2.setText("")
        self.label_2.setObjectName("label_2")
        self.horizontalLayout.addWidget(self.label_2)
        self.gridLayout.addLayout(self.horizontalLayout, 0, 0, 1, 1)
        MainWindow.setCentralWidget(self.centralwidget)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

        # My set
        # 设置窗体无边框
        # self.setWindowFlags(Qt.FramelessWindowHint)
        # 设置背景透明
        # self.setAttribute(Qt.WA_TranslucentBackground)
        # 设置label颜色
        self.label.setStyleSheet("background-color:rgb(0, 255, 255, 255);color:rgb(255, 0, 255, 255)")
        self.label_2.setStyleSheet("background-color:rgb(255, 255, 0, 255);color:rgb(255, 0, 255, 255)")
        self.timer.start(200)

    def retranslateUi(self, MainWindow):
        pass

    def setShowValue(self, CPUValue, MemValue):
        self.label.setText(CPUValue)
        self.label_2.setText(MemValue)

    def timeOut(self):
        Cpuinfo = "CPU\n " + getInfo.getCpuInfo()
        Meminfo = "Mem\n " + getInfo.getMemInfo()
        self.label.setText(Cpuinfo)
        self.label_2.setText(Meminfo)

def main():
	import sys,os
	from PyQt5.QtWidgets import QApplication
	from PyQt5.QtWidgets import QMainWindow

	app = QApplication(sys.argv)
	Form = QMainWindow()
	w = Ui_MainWindow()
	w.setupUi(Form)
	Form.show()

	w.setShowValue("CPUValue", "MemValue")
	
	exit(app.exec_())

if __name__ == '__main__':
	main()
