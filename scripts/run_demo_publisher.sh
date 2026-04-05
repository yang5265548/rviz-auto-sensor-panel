#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${ROOT_DIR}/scripts/common_ros_env.sh"
mkdir -p "${ROOT_DIR}/.roslog"

export ROS_LOG_DIR="${ROOT_DIR}/.roslog"

ensure_ros_environment
source_project_install_setup "${ROOT_DIR}"

if [[ -x "${ROOT_DIR}/install/lib/rviz_auto_sensor_panel/demo_sensor_publisher" ]]; then
  exec "${ROOT_DIR}/install/lib/rviz_auto_sensor_panel/demo_sensor_publisher"
fi

exec "${ROOT_DIR}/build/demo_sensor_publisher"
