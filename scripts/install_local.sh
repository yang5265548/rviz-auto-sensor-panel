#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ROS_PYTHONPATH="/Users/young/ros2_jazzy/install/lib/python3.11/site-packages"
PROJECT_PYTHONPATH="${ROOT_DIR}/vendor"

export PYTHONPATH="${ROS_PYTHONPATH}:${PROJECT_PYTHONPATH}:${PYTHONPATH:-}"

cmake --install "${ROOT_DIR}/build" --prefix "${ROOT_DIR}/install"
