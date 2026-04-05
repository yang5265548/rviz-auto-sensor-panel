cmake_minimum_required(VERSION 3.16)
project(rviz_auto_sensor_panel)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(visualization_msgs REQUIRED)

if(APPLE AND EXISTS "/opt/homebrew/opt/qt@5")
  list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@5")
endif()

find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(rviz_common REQUIRED)

set(QT_WIDGETS_TARGET Qt5::Widgets)

set(PUBLIC_HEADERS
  include/rviz_auto_sensor_panel/auto_display_factory.hpp
  include/rviz_auto_sensor_panel/display_registry.hpp
  include/rviz_auto_sensor_panel/sensor_catalog.hpp
  include/rviz_auto_sensor_panel/sensor_classifier.hpp
  include/rviz_auto_sensor_panel/sensor_tree_panel.hpp
  include/rviz_auto_sensor_panel/topic_state_logic.hpp
  include/rviz_auto_sensor_panel/topic_scanner.hpp
  include/rviz_auto_sensor_panel/types.hpp
)

add_library(${PROJECT_NAME} SHARED
  ${PUBLIC_HEADERS}
  src/auto_display_factory.cpp
  src/display_registry.cpp
  src/sensor_catalog.cpp
  src/sensor_classifier.cpp
  src/sensor_tree_panel.cpp
  src/topic_state_logic.cpp
  src/topic_scanner.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  RVIZ_AUTO_SENSOR_PANEL_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(${PROJECT_NAME}
  ament_index_cpp
  geometry_msgs
  nav_msgs
  rclcpp
  sensor_msgs
  tf2_ros
  visualization_msgs
)

target_link_libraries(${PROJECT_NAME}
  ${QT_WIDGETS_TARGET}
  pluginlib::pluginlib
  rviz_common::rviz_common
)

pluginlib_export_plugin_description_file(rviz_common rviz_common_plugins.xml)

add_executable(demo_sensor_publisher
  src/demo_sensor_publisher.cpp
)

target_include_directories(demo_sensor_publisher PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(demo_sensor_publisher
  geometry_msgs
  nav_msgs
  rclcpp
  sensor_msgs
  tf2_ros
  visualization_msgs
)

install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(TARGETS demo_sensor_publisher
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include
)

install(FILES rviz_common_plugins.xml
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config
)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(sensor_classifier_test
    test/sensor_classifier_test.cpp
    src/sensor_classifier.cpp
  )

  if(TARGET sensor_classifier_test)
    target_include_directories(sensor_classifier_test PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
    )
    ament_target_dependencies(sensor_classifier_test
      ament_index_cpp
      geometry_msgs
      nav_msgs
      rclcpp
      sensor_msgs
      tf2_ros
      visualization_msgs
    )
    target_compile_definitions(sensor_classifier_test PRIVATE
      RVIZ_AUTO_SENSOR_PANEL_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )
  endif()

  ament_add_gtest(sensor_catalog_test
    test/sensor_catalog_test.cpp
    src/sensor_catalog.cpp
    src/sensor_classifier.cpp
  )

  if(TARGET sensor_catalog_test)
    target_include_directories(sensor_catalog_test PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
    )
    ament_target_dependencies(sensor_catalog_test
      ament_index_cpp
      geometry_msgs
      nav_msgs
      rclcpp
      sensor_msgs
      tf2_ros
      visualization_msgs
    )
    target_compile_definitions(sensor_catalog_test PRIVATE
      RVIZ_AUTO_SENSOR_PANEL_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )
  endif()

  ament_add_gtest(display_registry_test
    test/display_registry_test.cpp
    src/display_registry.cpp
  )

  if(TARGET display_registry_test)
    target_include_directories(display_registry_test PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
    )
    ament_target_dependencies(display_registry_test
      ament_index_cpp
      geometry_msgs
      nav_msgs
      rclcpp
      sensor_msgs
      tf2_ros
      visualization_msgs
    )
    target_link_libraries(display_registry_test
      rviz_common::rviz_common
    )
  endif()

  ament_add_gtest(topic_state_logic_test
    test/topic_state_logic_test.cpp
    src/topic_state_logic.cpp
  )

  if(TARGET topic_state_logic_test)
    target_include_directories(topic_state_logic_test PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
    )
    ament_target_dependencies(topic_state_logic_test
      ament_index_cpp
      geometry_msgs
      nav_msgs
      rclcpp
      sensor_msgs
      tf2_ros
      visualization_msgs
    )
  endif()
endif()

ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_package()
