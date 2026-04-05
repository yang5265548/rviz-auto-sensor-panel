#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
mkdir -p "${ROOT_DIR}/.roslog"

export ROS_LOG_DIR="${ROOT_DIR}/.roslog"

set +u
source /Users/young/ros2_jazzy/install/setup.sh
source "${ROOT_DIR}/install/setup.sh"
set -u

exec rviz2
