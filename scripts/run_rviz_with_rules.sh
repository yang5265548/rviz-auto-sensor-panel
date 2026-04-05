#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RULES_FILE="${1:-${ROOT_DIR}/config/example_topic_grouping_rules.ini}"

if [[ ! -f "${RULES_FILE}" ]]; then
  echo "Rules file not found: ${RULES_FILE}"
  exit 1
fi

export RVIZ_AUTO_SENSOR_PANEL_RULES_FILE="${RULES_FILE}"
echo "Using grouping rules: ${RVIZ_AUTO_SENSOR_PANEL_RULES_FILE}"

exec "${ROOT_DIR}/scripts/run_rviz_local.sh"
