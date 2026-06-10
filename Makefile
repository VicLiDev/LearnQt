# LearnQt - Qt Learning Project
# Top-level Makefile - delegates to each demo's prjBuild.sh

DEMOS := 01.hello_qt \
         02.layouts \
         03.basic_widgets \
         04.signals_slots \
         05.mainwindow \
         06.dialogs \
         07.events \
         08.painting \
         09.timer_thread \
         10.model_view \
         11.file_io \
         12.style_qss \
         13.qwt_plotting \
         14.custom_plot_advanced \
         15.sqlite_database \
         16.pyqt_system_monitor \
         17.mqtt \
         18.json_xml \
         19.network \
         20.graphics_view \
         21.animation \
         22.concurrent \
         23.ipc \
         24.custom_widget

.PHONY: all build clean list help $(DEMOS)

all: build

build:
	@for d in $(DEMOS); do \
		echo ">>> Building $$d"; \
		$(MAKE) -C $$d build || true; \
	done
	@echo ""
	@echo "All demos built."

clean:
	@for d in $(DEMOS); do \
		$(MAKE) -C $$d clean 2>/dev/null || true; \
	done
	@echo "All demos cleaned."

list:
	@echo "Available demos:"
	@for d in $(DEMOS); do \
		echo "  $$d"; \
	done

help:
	@echo "LearnQt - Qt5 Learning Project"
	@echo ""
	@echo "Usage:"
	@echo "  make              Build all demos"
	@echo "  make clean        Clean all build artifacts"
	@echo "  make list         List all demos"
	@echo "  make <demo>       Build specific demo"
	@echo "  make run-<demo>   Build and run specific demo"
	@echo ""
	@echo "Demos:"
	@echo "  01-12  Qt C++ tutorials (basic to advanced)"
	@echo "  13-14  QWT/QCustomPlot plotting (requires QWT)"
	@echo "  15     SQLite database"
	@echo "  16     PyQt5 system monitor (Python)"
	@echo "  17     Qt MQTT 综合学习 (requires libqt5mqtt5-dev)"
	@echo "  18     JSON/XML 数据解析"
	@echo "  19     网络编程 (QTcpSocket/QUdpSocket/QNetworkAccessManager)"
	@echo "  20     Graphics View 图形视图框架"
	@echo "  21     Qt 动画框架 (QPropertyAnimation/动画组)"
	@echo "  22     Qt 并发编程 (QtConcurrent/QFuture/线程池)"
	@echo "  23     进程间通信 (QProcess/QLocalSocket/共享内存)"
	@echo "  24     自定义控件 (绘制/委托/组合控件)"

%:
	@if [ -d "$@" ] && [ -f "$@/prjBuild.sh" ]; then \
		cd "$@" && ./prjBuild.sh build; \
	else \
		echo "Unknown target: $@. Run 'make help' for usage."; \
	fi

run-%:
	@if [ -d "$@" ] && [ -f "$@/prjBuild.sh" ]; then \
		cd "$@" && ./prjBuild.sh run; \
	else \
		echo "Unknown demo: $@. Run 'make list' to see available demos."; \
	fi
