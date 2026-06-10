#!/bin/bash
# prjBuild.sh - Run script for pyqt_system_monitor
# Usage: ./prjBuild.sh [build|run|clean]

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SRC_DIR="${SCRIPT_DIR}/src"
MAIN_PY="${SRC_DIR}/main.py"

case "$1" in
    build)
        echo "Python project -- no compilation needed."
        echo "Checking dependencies..."
        python3 -c "import PyQt5" 2>/dev/null || { echo "ERROR: PyQt5 not installed. Run: pip3 install PyQt5"; exit 1; }
        python3 -c "import psutil" 2>/dev/null || { echo "ERROR: psutil not installed. Run: pip3 install psutil"; exit 1; }
        echo "All dependencies satisfied."
        ;;
    run)
        $0 build
        cd "${SRC_DIR}"
        echo "Running PyQt5 system monitor..."
        python3 "${MAIN_PY}"
        ;;
    clean)
        find "${SCRIPT_DIR}" -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null
        echo "Clean complete."
        ;;
    *)
        echo "Usage: $0 {build|run|clean}"
        exit 1
        ;;
esac
