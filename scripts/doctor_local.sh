#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ROS_SETUP="/Users/young/ros2_jazzy/install/setup.sh"

echo "Workspace: ${ROOT_DIR}"
echo

check_command() {
  local name="$1"
  if command -v "${name}" >/dev/null 2>&1; then
    echo "[OK] command found: ${name}"
  else
    echo "[WARN] command missing: ${name}"
  fi
}

check_path() {
  local label="$1"
  local path="$2"
  if [[ -e "${path}" ]]; then
    echo "[OK] ${label}: ${path}"
  else
    echo "[WARN] ${label}: ${path}"
  fi
}

check_command python3
check_command cmake
check_command rviz2
check_command ros2
echo

check_path "ROS setup" "${ROS_SETUP}"
check_path "Vendor catkin_pkg" "${ROOT_DIR}/vendor/catkin_pkg"
check_path "Build dir" "${ROOT_DIR}/build"
check_path "Install dir" "${ROOT_DIR}/install"
check_path "Plugin XML" "${ROOT_DIR}/rviz_common_plugins.xml"
check_path "Default preset" "${ROOT_DIR}/config/default_demo.rviz"
check_path "Installed plugin library" "${ROOT_DIR}/install/lib/librviz_auto_sensor_panel.dylib"
check_path "Installed demo publisher" "${ROOT_DIR}/install/lib/rviz_auto_sensor_panel/demo_sensor_publisher"

echo
echo "Suggested next commands:"
echo "  ./scripts/rebuild_install_local.sh"
echo "  ./scripts/run_demo_stack.sh"
