#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${ROOT_DIR}/scripts/common_ros_env.sh"
mkdir -p "${ROOT_DIR}/.roslog"

export ROS_LOG_DIR="${ROOT_DIR}/.roslog"
export AMENT_PREFIX_PATH="${ROOT_DIR}/install:${AMENT_PREFIX_PATH:-}"
export CMAKE_PREFIX_PATH="${ROOT_DIR}/install:${CMAKE_PREFIX_PATH:-}"

ensure_ros_environment
source_project_install_setup "${ROOT_DIR}"

exec rviz2
