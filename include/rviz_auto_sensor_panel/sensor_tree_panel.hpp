#ifndef RVIZ_AUTO_SENSOR_PANEL__SENSOR_TREE_PANEL_HPP_
#define RVIZ_AUTO_SENSOR_PANEL__SENSOR_TREE_PANEL_HPP_

#include <memory>

#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QTreeWidget>

#include "rviz_common/panel.hpp"
#include "rviz_common/ros_integration/ros_node_abstraction_iface.hpp"

#include "rviz_auto_sensor_panel/auto_display_factory.hpp"
#include "rviz_auto_sensor_panel/display_registry.hpp"
#include "rviz_auto_sensor_panel/sensor_catalog.hpp"
#include "rviz_auto_sensor_panel/topic_scanner.hpp"

namespace rviz_auto_sensor_panel
{

class SensorTreePanel : public rviz_common::Panel
{
  Q_OBJECT

public:
  explicit SensorTreePanel(QWidget * parent = nullptr);
  ~SensorTreePanel() override;

  void onInitialize() override;

private Q_SLOTS:
  void refreshTopics();
  void handleRefreshClicked();
  void handleTreeItemChanged(QTreeWidgetItem * item, int column);

private:
  void buildUi();
  void rebuildTree();
  void updateStatusLabel();
  void setTreeSignalsBlocked(bool blocked);

  QLabel * status_label_;
  QPushButton * refresh_button_;
  QTreeWidget * tree_widget_;
  QTimer * refresh_timer_;

  rviz_common::ros_integration::RosNodeAbstractionIface::WeakPtr node_abstraction_;
  rclcpp::Node::SharedPtr node_;

  TopicScanner topic_scanner_;
  SensorCatalog sensor_catalog_;
  DisplayRegistry display_registry_;
  AutoDisplayFactory auto_display_factory_;
};

}  // namespace rviz_auto_sensor_panel

#endif  // RVIZ_AUTO_SENSOR_PANEL__SENSOR_TREE_PANEL_HPP_
