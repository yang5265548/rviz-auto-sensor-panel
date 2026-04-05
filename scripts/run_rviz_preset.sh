#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
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
export HOME="${RVIZ_HOME_DIR}"

set +u
source /Users/young/ros2_jazzy/install/setup.sh
set -u

exec rviz2 -d "${RVIZ_CONFIG}"
