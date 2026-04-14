#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

"${ROOT_DIR}/scripts/rebuild_install_local.sh"

echo
echo "User-local install completed."
echo "Next steps:"
echo "  1. Start your ROS 2 topics, or run: ./scripts/run_demo_publisher.sh"
echo "  2. Start RViz with the preset: ./scripts/run_rviz_preset.sh"
echo "  3. The preset includes SensorTreePanel and asks for SLAM mode unless a fixed frame is provided"
echo "     Examples:"
echo "       ./scripts/run_rviz_preset.sh --slam-mode"
echo "       ./scripts/run_rviz_preset.sh --no-slam-mode"
echo "       RVIZ_AUTO_SENSOR_PANEL_FIXED_FRAME=odom ./scripts/run_rviz_preset.sh"
