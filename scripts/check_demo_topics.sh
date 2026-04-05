#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${ROOT_DIR}/scripts/common_ros_env.sh"
mkdir -p "${ROOT_DIR}/.roslog"

export ROS_LOG_DIR="${ROOT_DIR}/.roslog"

ensure_ros_environment
setup_runtime_library_path

TOPICS="$(ros2 topic list --no-daemon | grep -E '^/(demo|tf|tf_static)' || true)"

if [[ -z "${TOPICS}" ]]; then
  echo "No demo topics found."
  echo "Start the demo publisher first:"
  echo "  ./scripts/run_demo_publisher.sh"
  exit 1
fi

echo "${TOPICS}"
