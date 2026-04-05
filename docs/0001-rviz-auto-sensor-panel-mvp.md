# RViz Auto Sensor Panel MVP Baseline

## 1. Project Summary

This project adds an automation-oriented plugin to RViz for ROS 2.

The plugin will automatically discover sensor-related topics in the running ROS graph, classify them by message type, and present them in a structured tree view. Instead of manually adding and configuring each display one by one, the user can expand a sensor group and enable the desired items directly from the panel.

Working project name:

- `rviz_auto_sensor_panel`

## 2. Problem Statement

RViz is powerful, but the default workflow becomes repetitive in sensor-heavy systems.

Common pain points:

- Users need to manually add each display type.
- Users need to manually set the topic for each display.
- Multi-sensor robots often have many similar topics.
- Repeating setup across sessions is slow and error-prone.
- New users may not know which RViz display matches which ROS topic type.

This plugin is intended to reduce setup friction and make multi-sensor visualization feel closer to "plug and inspect" than "manually configure every display".

## 3. Product Goal

Create an RViz plugin that automatically detects common sensor topics and exposes them in a tree panel, so users can inspect a robot's available sensors with minimal manual setup.

The goal is not to replace RViz rendering, but to automate display discovery and creation on top of RViz's existing capabilities.

## 4. Target Users

- ROS 2 developers
- Robotics students
- Integration engineers
- Test and demo operators
- Teams working with robots that expose multiple cameras, lidars, and point cloud sources

## 5. Primary User Stories

1. As a developer, I want sensor topics to appear automatically in RViz so I do not need to manually add displays one by one.
2. As a user working with multiple similar sensors, I want them grouped clearly so I can understand what is available at a glance.
3. As a beginner, I want the system to choose the correct default display type for a topic so I do not need deep RViz knowledge to get started.
4. As an operator, I want to enable or disable discovered sensors quickly from one panel during testing or demos.

## 6. MVP Scope

The MVP will include the following capabilities.

### 6.1 Topic Discovery

- Discover ROS 2 topics from the active graph.
- Filter and keep only supported sensor topic types.
- Refresh the panel when supported topics appear or disappear.

### 6.2 Supported Topic Types

Initial supported message types:

- `sensor_msgs/msg/LaserScan`
- `sensor_msgs/msg/Image`
- `sensor_msgs/msg/PointCloud2`

These types were selected because they are common, visually meaningful, and already well supported by RViz.

### 6.3 Classification

- Classify topics by message type.
- Map each supported message type to a sensor category:
  - `LaserScan` -> Lidar
  - `Image` -> Camera
  - `PointCloud2` -> Point Cloud

### 6.4 Tree UI

- Provide an RViz panel plugin with a tree view.
- Group discovered topics by sensor category.
- Show each topic as a child item under its category.
- Show basic state such as enabled or disabled.

Example MVP tree:

- Lidar
- `/front/scan`
- `/rear/scan`
- Camera
- `/camera/left/image_raw`
- `/camera/right/image_raw`
- Point Cloud
- `/lidar/points`

### 6.5 Automatic Display Creation

- When a supported topic is enabled from the panel, automatically create the matching RViz display.
- Bind the display to the discovered topic with a default configuration.
- Reuse RViz's existing display implementations wherever possible.

Initial type mapping:

- `LaserScan` topic -> RViz LaserScan display
- `Image` topic -> RViz Image display
- `PointCloud2` topic -> RViz PointCloud2 display

### 6.6 State Handling

- Track whether a discovered item is currently enabled.
- Avoid duplicate display creation for the same topic while enabled.
- Mark topics that disappear from the graph as unavailable or offline.

## 7. Out of Scope for MVP

The following will not be part of the first version.

- Complex device-level grouping such as combining `image_raw` and `camera_info` into one camera device
- Automatic pairing of related topics
- Deep topic naming heuristics
- Per-project sensor templates
- Advanced parameter editing inside the custom panel
- Custom rendering implementations
- Support for every RViz display type
- Automatic layout or viewport arrangement
- Full persistence of all user customizations
- Non-ROS data sources

## 8. Functional Requirements

### 8.1 Discovery

- The plugin shall inspect the ROS 2 graph for available topics.
- The plugin shall identify the message type for each topic.
- The plugin shall include only supported message types in the panel.

### 8.2 Presentation

- The plugin shall show supported topics in a tree grouped by sensor category.
- The plugin shall update the tree when topics are added or removed.
- The plugin shall show enough information for the user to identify the topic quickly.

### 8.3 Display Management

- The plugin shall create an RViz display when the user enables a topic.
- The plugin shall configure the topic property automatically.
- The plugin shall not create duplicate active displays for the same topic and display type.
- The plugin shall allow the user to disable an auto-created display from the panel.

### 8.4 Error Tolerance

- The plugin shall fail gracefully if a display cannot be created.
- The plugin shall keep unsupported or malformed topics out of the managed list.
- The plugin shall remain usable when topics appear or disappear during runtime.

## 9. Non-Functional Requirements

- Startup experience should feel immediate for common topic counts.
- UI interactions should remain responsive during graph updates.
- The plugin should prefer RViz-native extension points instead of patching RViz internals.
- The code should be modular enough to support more sensor types later.
- The implementation should be testable without requiring a full custom RViz fork.

## 10. Proposed Technical Direction

The first implementation should use an RViz `Panel` plugin plus a small management layer that creates standard RViz displays programmatically.

Core modules for MVP:

- `TopicScanner`
- `SensorClassifier`
- `DisplayRegistry`
- `AutoDisplayFactory`
- `SensorTreePanel`

This is the preferred direction because:

- it minimizes custom rendering work
- it reuses stable RViz display plugins
- it focuses engineering effort on the actual product differentiation: automation

## 11. Success Criteria

The MVP is successful if all of the following are true:

- A user can launch RViz with the plugin enabled.
- Supported topics appear automatically in the panel.
- The user can enable a discovered topic without manually adding a display.
- The plugin creates the correct RViz display for each supported message type.
- A multi-sensor setup with at least 3 lidars and 2 cameras can be inspected with substantially less manual setup than stock RViz.

## 12. Key Risks

### 12.1 Topic Grouping Ambiguity

Topic names vary across projects, so meaningful device grouping may be unreliable if introduced too early.

MVP response:

- group only by message type
- delay device inference to a later milestone

### 12.2 RViz Integration Complexity

Programmatic display creation may involve internal assumptions in RViz APIs.

MVP response:

- rely on documented plugin extension points
- keep the first implementation narrow and use standard display classes only

### 12.3 Dynamic Graph Changes

Topics may appear, disappear, or restart while the UI is open.

MVP response:

- design for repeated refresh
- track managed displays by stable topic identity and display type

### 12.4 Default Configuration Quality

Poor defaults could make automatic creation feel noisy or confusing.

MVP response:

- keep default settings minimal
- expose simple enable and disable control first

## 13. Open Decisions for the Next Phase

These questions do not block the MVP baseline, but they must be resolved during architecture design:

- How should the plugin discover graph changes: polling, graph events, or a hybrid approach?
- How should the plugin identify and own only the displays it created?
- Should disabled topics remain visible as placeholders when they go offline?
- What metadata should be persisted between sessions in V1?
- How should naming and ordering be normalized in the tree?

## 14. Exit Criteria for Phase 1

Phase 1 is complete when:

- the team agrees with the product goal
- the MVP scope is stable
- the plugin approach is accepted
- the next phase can move into architecture and package structure design
