#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${ROOT_DIR}/scripts/common_ros_env.sh"
RVIZ_CONFIG="${1:-${ROOT_DIR}/config/default_demo.rviz}"
RVIZ_HOME_DIR="${ROOT_DIR}/.rviz_home"

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

exec rviz2 -d "${RVIZ_CONFIG}"
