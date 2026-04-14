#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${ROOT_DIR}/scripts/common_ros_env.sh"
RVIZ_CONFIG="${ROOT_DIR}/config/default_demo.rviz"
RVIZ_HOME_DIR="${ROOT_DIR}/.rviz_home"
FIXED_FRAME_OVERRIDE="${RVIZ_AUTO_SENSOR_PANEL_FIXED_FRAME:-}"
SLAM_MODE_OVERRIDE="${RVIZ_AUTO_SENSOR_PANEL_SLAM_MODE:-}"
PROMPT_FOR_MODE="true"

usage() {
  echo "Usage: ./scripts/run_rviz_preset.sh [--fixed-frame FRAME] [--slam-mode|--no-slam-mode] [config.rviz]"
}

normalize_boolean() {
  local value="${1:-}"
  value="$(printf '%s' "${value}" | tr '[:upper:]' '[:lower:]')"
  case "${value}" in
    1|true|yes|y|on) echo "true" ;;
    0|false|no|n|off) echo "false" ;;
    *)
      return 1
      ;;
  esac
}

resolve_fixed_frame() {
  if [[ -n "${FIXED_FRAME_OVERRIDE}" ]]; then
    printf '%s\n' "${FIXED_FRAME_OVERRIDE}"
    return 0
  fi

  if [[ "${PROMPT_FOR_MODE}" == "true" && -t 0 && -t 1 ]]; then
    local answer=""
    printf 'Use SLAM mode fixed frame (map)? [Y/n]: ' > /dev/tty
    read -r answer < /dev/tty || true
    answer="$(printf '%s' "${answer}" | tr '[:upper:]' '[:lower:]')"
    case "${answer}" in
      ""|y|yes)
        printf '%s\n' "map"
        return 0
        ;;
      n|no)
        printf '%s\n' "base_link"
        return 0
        ;;
      *)
        printf '%s\n' "base_link"
        return 0
        ;;
    esac
  fi

  if [[ -n "${SLAM_MODE_OVERRIDE}" ]]; then
    local normalized
    if normalized="$(normalize_boolean "${SLAM_MODE_OVERRIDE}")"; then
      if [[ "${normalized}" == "true" ]]; then
        printf '%s\n' "map"
      else
        printf '%s\n' "base_link"
      fi
      return 0
    fi
    echo "Invalid RVIZ_AUTO_SENSOR_PANEL_SLAM_MODE value: ${SLAM_MODE_OVERRIDE}" >&2
    exit 1
  fi

  printf '%s\n' "map"
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --fixed-frame)
      if [[ $# -lt 2 ]]; then
        usage
        exit 1
      fi
      FIXED_FRAME_OVERRIDE="$2"
      shift 2
      ;;
    --slam-mode)
      SLAM_MODE_OVERRIDE="true"
      PROMPT_FOR_MODE="false"
      shift
      ;;
    --no-slam-mode)
      SLAM_MODE_OVERRIDE="false"
      PROMPT_FOR_MODE="false"
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      if [[ "${RVIZ_CONFIG}" != "${ROOT_DIR}/config/default_demo.rviz" ]]; then
        usage
        exit 1
      fi
      RVIZ_CONFIG="$1"
      shift
      ;;
  esac
done

if [[ ! -f "${RVIZ_CONFIG}" ]]; then
  echo "RViz config not found: ${RVIZ_CONFIG}"
  exit 1
fi

mkdir -p "${ROOT_DIR}/.roslog"
rm -rf "${RVIZ_HOME_DIR}"
mkdir -p "${RVIZ_HOME_DIR}/.rviz2"

export ROS_LOG_DIR="${ROOT_DIR}/.roslog"
export AMENT_PREFIX_PATH="${ROOT_DIR}/install:${AMENT_PREFIX_PATH:-}"
export CMAKE_PREFIX_PATH="${ROOT_DIR}/install:${CMAKE_PREFIX_PATH:-}"

ensure_ros_environment
source_project_install_setup "${ROOT_DIR}"
export HOME="${RVIZ_HOME_DIR}"

FIXED_FRAME="$(resolve_fixed_frame)"
ACTIVE_RVIZ_CONFIG="${RVIZ_HOME_DIR}/active_config.rviz"

awk -v frame="${FIXED_FRAME}" '
  /^[[:space:]]*Fixed Frame:/ {
    sub(/Fixed Frame: .*/, "Fixed Frame: " frame)
  }
  { print }
' "${RVIZ_CONFIG}" > "${ACTIVE_RVIZ_CONFIG}"

echo "Starting RViz with Fixed Frame = ${FIXED_FRAME}"

exec rviz2 -d "${ACTIVE_RVIZ_CONFIG}"
