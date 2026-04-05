#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
mkdir -p "${ROOT_DIR}/.roslog"

export ROS_LOG_DIR="${ROOT_DIR}/.roslog"

set +u
source /Users/young/ros2_jazzy/install/setup.sh
set -u

if [[ -x "${ROOT_DIR}/install/lib/rviz_auto_sensor_panel/demo_sensor_publisher" ]]; then
  exec "${ROOT_DIR}/install/lib/rviz_auto_sensor_panel/demo_sensor_publisher"
fi

exec "${ROOT_DIR}/build/demo_sensor_publisher"
