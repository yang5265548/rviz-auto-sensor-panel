#ifndef RVIZ_AUTO_SENSOR_PANEL__SENSOR_TREE_PANEL_HPP_
#define RVIZ_AUTO_SENSOR_PANEL__SENSOR_TREE_PANEL_HPP_

#include <map>
#include <memory>
#include <set>
#include <string>

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
  void load(const rviz_common::Config & config) override;
  void save(rviz_common::Config config) const override;

private Q_SLOTS:
  void refreshTopics();
  void handleRefreshClicked();
  void handleTreeItemChanged(QTreeWidgetItem * item, int column);

private:
  void buildUi();
  void rebuildTree();
  void updateStatusLabel();
  void syncEnabledTopicsFromTree();
  void reconcileDesiredDisplays();
  bool shouldTopicStartEnabled(const std::string & topic_name) const;
  bool shouldCategoryStartExpanded(SensorCategory category) const;
  void rememberCategoryExpansionStates();

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
  std::set<std::string> persisted_enabled_topics_;
  std::map<std::string, bool> persisted_category_expansion_;
};

}  // namespace rviz_auto_sensor_panel

#endif  // RVIZ_AUTO_SENSOR_PANEL__SENSOR_TREE_PANEL_HPP_
