#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "${ROOT_DIR}"

if [[ ! -d "${ROOT_DIR}/vendor/catkin_pkg" ]]; then
  python3 -m pip install --target ./vendor -r requirements-dev.txt
else
  echo "Using existing local Python dev dependencies from ./vendor"
fi

./scripts/configure_local.sh
./scripts/build_local.sh
./scripts/install_local.sh
