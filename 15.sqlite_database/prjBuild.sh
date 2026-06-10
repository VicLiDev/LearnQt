#!/bin/bash
# prjBuild.sh - Build script for sqlite_database
# Usage: ./prjBuild.sh [build|run|clean|rebuild]

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
PROJECT_NAME="sqlite_database"

case "$1" in
    build)
        mkdir -p "${BUILD_DIR}"
        cd "${BUILD_DIR}"
        cmake ..
        make -j$(nproc)
        echo "Build complete: ${BUILD_DIR}/${PROJECT_NAME}"
        ;;
    run)
        $0 build
        cd "${BUILD_DIR}"
        echo "Running ${PROJECT_NAME}..."
        ./${PROJECT_NAME}
        ;;
    clean)
        rm -rf "${BUILD_DIR}"
        echo "Clean complete."
        ;;
    rebuild)
        $0 clean
        $0 build
        ;;
    *)
        echo "Usage: $0 {build|run|clean|rebuild}"
        exit 1
        ;;
esac
