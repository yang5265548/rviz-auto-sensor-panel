#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

rm -rf "${ROOT_DIR}/.roslog" "${ROOT_DIR}/.rviz_home"

echo "Cleaned local runtime state:"
echo "  - .roslog"
echo "  - .rviz_home"
