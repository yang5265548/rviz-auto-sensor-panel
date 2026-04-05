# rviz-auto-sensor-panel

An automation-oriented RViz plugin for ROS 2.

This project aims to reduce repetitive RViz setup by automatically discovering supported sensor topics, organizing them in a tree panel, and creating matching RViz displays on demand.

## Vision

Instead of manually adding and configuring each display, users should be able to:

- see discovered sensors automatically
- browse them in a structured panel
- enable the sensor they want with minimal setup

## MVP

The first version focuses on:

- automatic discovery of supported ROS 2 topics
- classification by message type
- a tree-based RViz panel
- automatic creation of matching RViz displays

Initial supported message types:

- `sensor_msgs/msg/LaserScan`
- `sensor_msgs/msg/Image`
- `sensor_msgs/msg/PointCloud2`

## Project Status

The project is currently in the planning and architecture phase.

Requirements baseline:

- [MVP Baseline](./docs/0001-rviz-auto-sensor-panel-mvp.md)

## Planned Structure

- `docs/` for product and architecture documents
- `src/` for implementation packages
- `test/` for validation and integration support

## Next Steps

- finalize plugin architecture
- define package structure
- scaffold the ROS 2 package
- implement topic discovery and auto-display management
