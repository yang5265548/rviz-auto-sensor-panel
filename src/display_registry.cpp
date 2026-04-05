#include "rviz_auto_sensor_panel/display_registry.hpp"

namespace rviz_auto_sensor_panel
{

bool DisplayRegistry::hasDisplay(const std::string & topic_name) const
{
  return entries_.find(topic_name) != entries_.end();
}

rviz_common::Display * DisplayRegistry::getDisplay(const std::string & topic_name) const
{
  const auto entry = entries_.find(topic_name);
  if (entry == entries_.end()) {
    return nullptr;
  }

  return entry->second.display;
}

void DisplayRegistry::registerDisplay(
  const ManagedDisplay & managed_display,
  rviz_common::Display * display)
{
  entries_[managed_display.topic_name] = Entry{managed_display, display};
}

void DisplayRegistry::setEnabled(const std::string & topic_name, bool enabled)
{
  const auto entry = entries_.find(topic_name);
  if (entry == entries_.end()) {
    return;
  }

  entry->second.managed_display.is_enabled = enabled;
  if (entry->second.display) {
    entry->second.display->setEnabled(enabled);
  }
}

bool DisplayRegistry::isEnabled(const std::string & topic_name) const
{
  const auto entry = entries_.find(topic_name);
  if (entry == entries_.end()) {
    return false;
  }

  if (entry->second.display) {
    if (auto * enabled_property = entry->second.display->subProp("Value")) {
      const auto value = enabled_property->getValue();
      if (value.isValid()) {
        return value.toBool();
      }
    }

    return entry->second.display->isEnabled();
  }

  return entry->second.managed_display.is_enabled;
}

}  // namespace rviz_auto_sensor_panel
