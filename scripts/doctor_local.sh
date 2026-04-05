#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${ROOT_DIR}/scripts/common_ros_env.sh"
ROS_SETUP="$(detect_ros_setup_path || true)"

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

if [[ -n "${ROS_SETUP}" ]]; then
  check_path "ROS setup" "${ROS_SETUP}"
else
  echo "[WARN] ROS setup: not detected automatically"
  echo "       Set RVIZ_AUTO_SENSOR_PANEL_ROS_SETUP=/absolute/path/to/setup.bash"
fi
check_path "Vendor catkin_pkg" "${ROOT_DIR}/vendor/catkin_pkg"
check_path "Build dir" "${ROOT_DIR}/build"
check_path "Install dir" "${ROOT_DIR}/install"
check_path "Plugin XML" "${ROOT_DIR}/rviz_common_plugins.xml"
check_path "Default preset" "${ROOT_DIR}/config/default_demo.rviz"
if [[ "$(uname -s)" == "Darwin" ]]; then
  check_path "Installed plugin library" "${ROOT_DIR}/install/lib/librviz_auto_sensor_panel.dylib"
else
  check_path "Installed plugin library" "${ROOT_DIR}/install/lib/librviz_auto_sensor_panel.so"
fi
check_path "Installed demo publisher" "${ROOT_DIR}/install/lib/rviz_auto_sensor_panel/demo_sensor_publisher"

echo
echo "Suggested next commands:"
echo "  ./scripts/rebuild_install_local.sh"
echo "  ./scripts/run_demo_stack.sh"
