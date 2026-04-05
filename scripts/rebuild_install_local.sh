#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "${ROOT_DIR}"

python3 -m pip install --target ./vendor -r requirements-dev.txt
./scripts/configure_local.sh
./scripts/build_local.sh
./scripts/install_local.sh
