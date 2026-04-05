#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${ROOT_DIR}/scripts/common_ros_env.sh"

ensure_ros_environment
setup_project_pythonpath "${ROOT_DIR}"

cmake --build "${ROOT_DIR}/build" -j4
