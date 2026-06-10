# LearnQt - Qt Learning Project
# Top-level Makefile
# 单个 demo: cd <category>/<demo> && ./prjBuild.sh build

DEMOS := 01.basics/01.hello_qt \
         01.basics/02.layouts \
         01.basics/03.basic_widgets \
         01.basics/04.signals_slots \
         01.basics/05.mainwindow \
         01.basics/06.dialogs \
         01.basics/07.events \
         02.intermediate/01.painting \
         02.intermediate/02.timer_thread \
         02.intermediate/03.model_view \
         02.intermediate/04.file_io \
         02.intermediate/05.style_qss \
         03.data/01.sqlite_database \
         03.data/02.json_xml \
         04.graphics_animation/01.graphics_view \
         04.graphics_animation/02.animation \
         04.graphics_animation/03.custom_widget \
         05.network_concurrent/01.mqtt \
         05.network_concurrent/02.network \
         05.network_concurrent/03.concurrent \
         05.network_concurrent/04.ipc \
         06.third_party/01.qwt_plotting \
         06.third_party/02.custom_plot_advanced \
         06.third_party/03.pyqt_system_monitor

.PHONY: all build clean list help

all: build

build:
	@for d in $(DEMOS); do \
		echo ">>> Building $$d"; \
		(cd "$$d" && ./prjBuild.sh build) || true; \
	done

clean:
	@for d in $(DEMOS); do \
		(cd "$$d" && ./prjBuild.sh clean 2>/dev/null) || true; \
	done

list:
	@echo "Available demos:"
	@for d in $(DEMOS); do \
		echo "  $$d"; \
	done

help:
	@echo ""
	@echo "make all         Build all demos"
	@echo "make clean       Clean all demos"
	@echo "make list        List all demos"
