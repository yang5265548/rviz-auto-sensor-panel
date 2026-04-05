#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "${ROOT_DIR}"

has_python_module() {
  local module_name="$1"
  python3 -c "import ${module_name}" >/dev/null 2>&1
}

if [[ -d "${ROOT_DIR}/vendor/catkin_pkg" ]]; then
  echo "Using existing local Python dev dependencies from ./vendor"
elif has_python_module catkin_pkg; then
  echo "Using catkin_pkg from the current Python environment"
elif python3 -m pip --version >/dev/null 2>&1; then
  python3 -m pip install --target ./vendor -r requirements-dev.txt
else
  echo "Python module 'catkin_pkg' is required, but pip is not available."
  echo "Install one of the following, then rerun this script:"
  echo "  Ubuntu/Debian: sudo apt install python3-pip"
  echo "  or: sudo apt install python3-catkin-pkg"
  exit 1
fi

./scripts/configure_local.sh
./scripts/build_local.sh
./scripts/install_local.sh
