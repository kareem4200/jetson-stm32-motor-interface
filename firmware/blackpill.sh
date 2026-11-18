#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

PROGRAMMER="/home/kareem/.local/share/stm32cube/bundles/programmer/2.23.0/bin/STM32_Programmer_CLI"

SWD_CONFIG="port=SWD freq=1000"

case "${1:-}" in
    flash)
        BUILD_TYPE="${2:-Debug}"

        if [[ "$BUILD_TYPE" != "Debug" && "$BUILD_TYPE" != "Release" ]]; then
            echo "Error: Invalid build type '$BUILD_TYPE'"
            echo "Valid options: Debug, Release"
            exit 1
        fi

        FIRMWARE="$SCRIPT_DIR/build/$BUILD_TYPE/firmware.elf"

        if [[ ! -f "$FIRMWARE" ]]; then
            echo "Error: Firmware not found: $FIRMWARE"
            echo "Build the project first."
            exit 1
        fi

        echo "Flashing $BUILD_TYPE firmware..."
        "$PROGRAMMER" -c $SWD_CONFIG -w "$FIRMWARE" -v -rst
        echo "Flash completed successfully."
        ;;

    erase)
        echo "Erasing STM32 Flash..."
        "$PROGRAMMER" -c $SWD_CONFIG -e all
        echo "Flash erased successfully."
        ;;

    reset)
        echo "Resetting STM32..."
        "$PROGRAMMER" -c $SWD_CONFIG -rst
        echo "Reset completed successfully."
        ;;

    *)
        echo "Usage: $0 {flash [Debug|Release]|reset|erase}"
        exit 1
        ;;
esac