#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "${ROOT_DIR}"

echo "[1/5] Doctor"
./scripts/doctor_local.sh
echo

echo "[2/5] Build"
./scripts/build_local.sh
echo

echo "[3/5] Test"
./scripts/test_local.sh
echo

echo "[4/5] Install"
./scripts/install_local.sh
echo

echo "[5/5] Smoke Test"
./scripts/smoke_test_local.sh
echo

echo "Preflight completed successfully."
