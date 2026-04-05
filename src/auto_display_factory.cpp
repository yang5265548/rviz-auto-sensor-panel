#include "rviz_auto_sensor_panel/auto_display_factory.hpp"

namespace rviz_auto_sensor_panel
{

rviz_common::Display * AutoDisplayFactory::createDisplay(
  rviz_common::VisualizationManager * visualization_manager,
  const std::string & display_type,
  const std::string & topic_name,
  const std::string & message_type) const
{
  if (!visualization_manager || display_type.empty()) {
    return nullptr;
  }

  auto * display = visualization_manager->createDisplay(
    QString::fromStdString(display_type),
    buildDisplayName(display_type, topic_name),
    true);

  if (!display) {
    return nullptr;
  }

  display->setTopic(QString::fromStdString(topic_name), QString::fromStdString(message_type));
  display->setEnabled(true);
  return display;
}

QString AutoDisplayFactory::buildDisplayName(
  const std::string & display_type,
  const std::string & topic_name) const
{
  QString short_type = QString::fromStdString(display_type);
  const int slash_index = short_type.lastIndexOf('/');
  if (slash_index >= 0) {
    short_type = short_type.mid(slash_index + 1);
  }

  return QString("[Auto] %1: %2")
    .arg(short_type)
    .arg(QString::fromStdString(topic_name));
}

}  // namespace rviz_auto_sensor_panel
