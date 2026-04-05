#include "rviz_auto_sensor_panel/sensor_tree_panel.hpp"

#include <utility>

#include <QHBoxLayout>
#include <QSignalBlocker>
#include <QString>
#include <QVBoxLayout>

#include "pluginlib/class_list_macros.hpp"
#include "rviz_common/display_context.hpp"
#include "rviz_common/visualization_manager.hpp"

namespace rviz_auto_sensor_panel
{

namespace
{

constexpr int kRefreshIntervalMs = 1000;
constexpr int kTreeColumnCount = 2;
constexpr int kNameColumn = 0;
constexpr int kStateColumn = 1;
constexpr char kEnabledTopicsKey[] = "EnabledTopics";
constexpr char kSuppressedTopicsKey[] = "SuppressedTopics";
constexpr char kExpandedGroupsKey[] = "ExpandedGroups";
constexpr char kTopicNameKey[] = "TopicName";
constexpr char kGroupKey[] = "GroupKey";
constexpr char kExpandedKey[] = "Expanded";
constexpr char kAutoEnableNewTopicsKey[] = "AutoEnableNewTopics";
constexpr int kTreeItemTypeRole = Qt::UserRole + 10;
constexpr int kTreeItemGroupKeyRole = Qt::UserRole + 11;

enum class TreeItemType
{
  Category,
  Group,
  Topic
};

QString publisherSummary(const DiscoveredTopic & topic)
{
  if (!topic.is_available) {
    return "Offline";
  }

  return QString("%1 publisher%2")
         .arg(static_cast<qulonglong>(topic.publisher_count))
         .arg(topic.publisher_count == 1 ? "" : "s");
}

}  // namespace

SensorTreePanel::SensorTreePanel(QWidget * parent)
: rviz_common::Panel(parent),
  status_label_(nullptr),
  refresh_button_(nullptr),
  enable_all_button_(nullptr),
  disable_all_button_(nullptr),
  auto_enable_checkbox_(nullptr),
  tree_widget_(nullptr),
  refresh_timer_(nullptr),
  topic_scanner_(SensorClassifier()),
  auto_enable_new_topics_(true)
{
  buildUi();
}

SensorTreePanel::~SensorTreePanel() = default;

void SensorTreePanel::onInitialize()
{
  node_abstraction_ = getDisplayContext()->getRosNodeAbstraction();
  auto node_abstraction = node_abstraction_.lock();
  if (node_abstraction) {
    node_ = node_abstraction->get_raw_node();
  }

  refreshTopics();
  if (refresh_timer_) {
    refresh_timer_->start(kRefreshIntervalMs);
  }
}

void SensorTreePanel::load(const rviz_common::Config & config)
{
  rviz_common::Panel::load(config);

  persisted_enabled_topics_.clear();
  suppressed_auto_enable_topics_.clear();
  persisted_group_expansion_.clear();

  const auto enabled_topics = config.mapGetChild(kEnabledTopicsKey);
  for (int index = 0; index < enabled_topics.listLength(); ++index) {
    QString topic_name;
    if (enabled_topics.listChildAt(index).mapGetString(kTopicNameKey, &topic_name)) {
      persisted_enabled_topics_.insert(topic_name.toStdString());
    }
  }

  const auto suppressed_topics = config.mapGetChild(kSuppressedTopicsKey);
  for (int index = 0; index < suppressed_topics.listLength(); ++index) {
    QString topic_name;
    if (suppressed_topics.listChildAt(index).mapGetString(kTopicNameKey, &topic_name)) {
      suppressed_auto_enable_topics_.insert(topic_name.toStdString());
    }
  }

  const auto expanded_groups = config.mapGetChild(kExpandedGroupsKey);
  for (int index = 0; index < expanded_groups.listLength(); ++index) {
    auto entry = expanded_groups.listChildAt(index);
    QString group_key;
    bool is_expanded = true;

    if (!entry.mapGetString(kGroupKey, &group_key)) {
      continue;
    }

    entry.mapGetBool(kExpandedKey, &is_expanded);
    persisted_group_expansion_[group_key.toStdString()] = is_expanded;
  }

  config.mapGetBool(kAutoEnableNewTopicsKey, &auto_enable_new_topics_);
  if (auto_enable_checkbox_) {
    auto_enable_checkbox_->setChecked(auto_enable_new_topics_);
  }

  rebuildTree();
  updateStatusLabel();
}

void SensorTreePanel::save(rviz_common::Config config) const
{
  rviz_common::Panel::save(config);

  auto enabled_topics = config.mapMakeChild(kEnabledTopicsKey);
  for (const auto & topic_name : persisted_enabled_topics_) {
    auto entry = enabled_topics.listAppendNew();
    entry.mapSetValue(kTopicNameKey, QString::fromStdString(topic_name));
  }

  auto expanded_groups = config.mapMakeChild(kExpandedGroupsKey);
  for (const auto & [group_key, expanded] : persisted_group_expansion_) {
    auto entry = expanded_groups.listAppendNew();
    entry.mapSetValue(kGroupKey, QString::fromStdString(group_key));
    entry.mapSetValue(kExpandedKey, expanded);
  }

  auto suppressed_topics = config.mapMakeChild(kSuppressedTopicsKey);
  for (const auto & topic_name : suppressed_auto_enable_topics_) {
    auto entry = suppressed_topics.listAppendNew();
    entry.mapSetValue(kTopicNameKey, QString::fromStdString(topic_name));
  }

  config.mapSetValue(kAutoEnableNewTopicsKey, auto_enable_new_topics_);
}

void SensorTreePanel::refreshTopics()
{
  syncEnabledTopicsFromDisplays();

  std::set<std::string> known_topics;
  for (const auto & topic : sensor_catalog_.allTopics()) {
    known_topics.insert(topic.name);
  }

  const auto latest_topics = topic_scanner_.scan(node_);
  applyAutoEnableForNewTopics(
    known_topics,
    latest_topics,
    auto_enable_new_topics_,
    suppressed_auto_enable_topics_,
    &persisted_enabled_topics_);

  sensor_catalog_.update(latest_topics);
  rememberGroupExpansionStates();
  rebuildTree();
  reconcileDesiredDisplays();
  updateStatusLabel();
}

void SensorTreePanel::syncEnabledTopicsFromDisplays()
{
  for (const auto & topic : sensor_catalog_.allTopics()) {
    if (!display_registry_.hasDisplay(topic.name)) {
      continue;
    }

    const bool enabled = display_registry_.isEnabled(topic.name);
    rememberTopicEnabledState(
      topic.name,
      enabled,
      &persisted_enabled_topics_,
      &suppressed_auto_enable_topics_);
  }
}

void SensorTreePanel::handleRefreshClicked()
{
  refreshTopics();
}

void SensorTreePanel::handleTreeItemChanged(QTreeWidgetItem * item, int column)
{
  if (!item || column != kNameColumn) {
    return;
  }

  const auto item_type = static_cast<TreeItemType>(
    item->data(kNameColumn, kTreeItemTypeRole).toInt());
  const auto desired_state = item->checkState(kNameColumn);

  if (item_type != TreeItemType::Topic) {
    if (desired_state == Qt::PartiallyChecked) {
      return;
    }

    setDescendantTopicsEnabled(item, desired_state == Qt::Checked);
    rememberGroupExpansionStates();
    rebuildTree();
    reconcileDesiredDisplays();
    updateStatusLabel();
    Q_EMIT configChanged();
    return;
  }

  const auto topic_name = item->data(kNameColumn, Qt::UserRole).toString().toStdString();
  const auto message_type = item->data(kNameColumn, Qt::UserRole + 1).toString().toStdString();
  applyTopicEnabledState(topic_name, message_type, desired_state == Qt::Checked, item);
  rememberGroupExpansionStates();
  rebuildTree();
  updateStatusLabel();
  Q_EMIT configChanged();
}

void SensorTreePanel::applyTopicEnabledState(
  const std::string & topic_name,
  const std::string & message_type,
  bool enabled,
  QTreeWidgetItem * item)
{
  rememberTopicEnabledState(
    topic_name, enabled, &persisted_enabled_topics_, &suppressed_auto_enable_topics_);

  if (display_registry_.hasDisplay(topic_name)) {
    display_registry_.setEnabled(topic_name, enabled);
    return;
  }

  if (!enabled) {
    return;
  }

  auto * visualization_manager =
    dynamic_cast<rviz_common::VisualizationManager *>(getDisplayContext());
  if (!visualization_manager) {
    return;
  }

  SensorClassifier classifier;
  const std::string display_type = classifier.lookupDisplayType(message_type);
  auto * display = auto_display_factory_.createDisplay(
    visualization_manager, display_type, topic_name, message_type);

  if (!display) {
    persisted_enabled_topics_.erase(topic_name);
    if (item) {
      item->setCheckState(kNameColumn, Qt::Unchecked);
    }
    return;
  }

  display_registry_.registerDisplay(
    ManagedDisplay{topic_name, message_type, display_type, display->getName().toStdString(), true},
    display);
}

void SensorTreePanel::setDescendantTopicsEnabled(QTreeWidgetItem * root_item, bool enabled)
{
  if (!root_item) {
    return;
  }

  for (int child_index = 0; child_index < root_item->childCount(); ++child_index) {
    auto * child = root_item->child(child_index);
    if (!child) {
      continue;
    }

    const auto item_type = static_cast<TreeItemType>(
      child->data(kNameColumn, kTreeItemTypeRole).toInt());

    if (item_type == TreeItemType::Topic) {
      const auto topic_name = child->data(kNameColumn, Qt::UserRole).toString().toStdString();
      const auto message_type =
        child->data(kNameColumn, Qt::UserRole + 1).toString().toStdString();
      applyTopicEnabledState(topic_name, message_type, enabled, child);
      continue;
    }

    setDescendantTopicsEnabled(child, enabled);
  }
}

Qt::CheckState SensorTreePanel::determineAggregateCheckState(const QTreeWidgetItem * item) const
{
  if (!item || item->childCount() == 0) {
    return Qt::Unchecked;
  }

  int checked_children = 0;
  int unchecked_children = 0;
  for (int child_index = 0; child_index < item->childCount(); ++child_index) {
    const auto * child = item->child(child_index);
    if (!child) {
      continue;
    }

    const auto child_state = child->checkState(kNameColumn);
    if (child_state == Qt::PartiallyChecked) {
      return Qt::PartiallyChecked;
    }

    if (child_state == Qt::Checked) {
      ++checked_children;
    } else {
      ++unchecked_children;
    }
  }

  if (checked_children > 0 && unchecked_children > 0) {
    return Qt::PartiallyChecked;
  }

  return checked_children > 0 ? Qt::Checked : Qt::Unchecked;
}

void SensorTreePanel::buildUi()
{
  auto * layout = new QVBoxLayout(this);

  status_label_ = new QLabel("No supported topics discovered yet.");
  refresh_button_ = new QPushButton("Refresh");
  enable_all_button_ = new QPushButton("Enable All");
  disable_all_button_ = new QPushButton("Disable All");
  auto_enable_checkbox_ = new QCheckBox("Auto-enable new topics");
  tree_widget_ = new QTreeWidget();
  refresh_timer_ = new QTimer(this);

  auto_enable_checkbox_->setChecked(auto_enable_new_topics_);

  tree_widget_->setColumnCount(kTreeColumnCount);
  tree_widget_->setHeaderLabels(QStringList() << "Sensor / Topic" << "State");
  tree_widget_->setRootIsDecorated(true);
  tree_widget_->setAlternatingRowColors(true);

  auto * header_layout = new QHBoxLayout();
  header_layout->addWidget(status_label_);
  header_layout->addStretch();
  header_layout->addWidget(auto_enable_checkbox_);
  header_layout->addWidget(enable_all_button_);
  header_layout->addWidget(disable_all_button_);
  header_layout->addWidget(refresh_button_);

  layout->addLayout(header_layout);
  layout->addWidget(tree_widget_);

  QObject::connect(refresh_button_, &QPushButton::clicked, this, &SensorTreePanel::handleRefreshClicked);
  QObject::connect(enable_all_button_, &QPushButton::clicked, this, &SensorTreePanel::handleEnableAllClicked);
  QObject::connect(disable_all_button_, &QPushButton::clicked, this, &SensorTreePanel::handleDisableAllClicked);
  QObject::connect(auto_enable_checkbox_, &QCheckBox::toggled, this, &SensorTreePanel::handleAutoEnableToggled);
  QObject::connect(refresh_timer_, &QTimer::timeout, this, &SensorTreePanel::refreshTopics);
  QObject::connect(
    tree_widget_, &QTreeWidget::itemChanged, this, &SensorTreePanel::handleTreeItemChanged);
  QObject::connect(tree_widget_, &QTreeWidget::itemExpanded, this, [this](QTreeWidgetItem *) {
    rememberGroupExpansionStates();
    Q_EMIT configChanged();
  });
  QObject::connect(tree_widget_, &QTreeWidget::itemCollapsed, this, [this](QTreeWidgetItem *) {
    rememberGroupExpansionStates();
    Q_EMIT configChanged();
  });
}

void SensorTreePanel::handleEnableAllClicked()
{
  setAllTopicsEnabled(true);
}

void SensorTreePanel::handleDisableAllClicked()
{
  setAllTopicsEnabled(false);
}

void SensorTreePanel::handleAutoEnableToggled(bool checked)
{
  auto_enable_new_topics_ = checked;
  Q_EMIT configChanged();
}

void SensorTreePanel::rebuildTree()
{
  const QSignalBlocker blocker(tree_widget_);
  tree_widget_->clear();

  const auto groups = sensor_catalog_.groupedTopics();

  for (const auto category : supportedCategories()) {
    auto * category_item = new QTreeWidgetItem(tree_widget_);
    category_item->setText(kNameColumn, QString::fromStdString(toString(category)));
    category_item->setText(kStateColumn, "Category");
    category_item->setFlags(category_item->flags() | Qt::ItemIsUserCheckable);
    category_item->setData(
      kNameColumn, kTreeItemTypeRole, static_cast<int>(TreeItemType::Category));
    category_item->setData(
      kNameColumn, kTreeItemGroupKeyRole, QString::fromStdString(toString(category)));

    const auto group = groups.find(category);
    if (group == groups.end()) {
      category_item->setExpanded(shouldGroupStartExpanded(toString(category)));
      continue;
    }

    int available_in_category = 0;
    for (const auto & device_group : group->second) {
      auto * group_item = new QTreeWidgetItem(category_item);
      group_item->setText(kNameColumn, QString::fromStdString(device_group.label));
      group_item->setFlags(group_item->flags() | Qt::ItemIsUserCheckable);
      group_item->setData(
        kNameColumn, kTreeItemTypeRole, static_cast<int>(TreeItemType::Group));
      group_item->setData(
        kNameColumn, kTreeItemGroupKeyRole, QString::fromStdString(device_group.key));

      int available_in_group = 0;
      for (const auto & topic : device_group.topics) {
        auto * topic_item = new QTreeWidgetItem(group_item);
        topic_item->setFlags(topic_item->flags() | Qt::ItemIsUserCheckable);
        topic_item->setText(
          kNameColumn,
          QString::fromStdString(topic.topic_label + (topic.is_available ? "" : " (offline)")));
        topic_item->setText(kStateColumn, publisherSummary(topic));
        topic_item->setData(kNameColumn, Qt::UserRole, QString::fromStdString(topic.name));
        topic_item->setData(kNameColumn, Qt::UserRole + 1, QString::fromStdString(topic.message_type));
        topic_item->setData(
          kNameColumn, kTreeItemTypeRole, static_cast<int>(TreeItemType::Topic));
        topic_item->setToolTip(
          kNameColumn,
          QString("Topic: %1\nType: %2\nPublishers: %3")
          .arg(QString::fromStdString(topic.name))
          .arg(QString::fromStdString(topic.message_type))
          .arg(static_cast<qulonglong>(topic.publisher_count)));
        topic_item->setCheckState(
          kNameColumn,
          shouldTopicStartEnabled(topic.name) ? Qt::Checked : Qt::Unchecked);

        if (topic.is_available) {
          ++available_in_group;
          ++available_in_category;
        }
      }

      group_item->setText(
        kStateColumn,
        QString("%1 total / %2 available")
        .arg(static_cast<int>(device_group.topics.size()))
        .arg(available_in_group));
      group_item->setToolTip(
        kNameColumn,
        QString("Device group: %1\nTopics: %2\nAvailable: %3")
        .arg(QString::fromStdString(device_group.label))
        .arg(static_cast<int>(device_group.topics.size()))
        .arg(available_in_group));
      group_item->setCheckState(kNameColumn, determineAggregateCheckState(group_item));
      group_item->setExpanded(shouldGroupStartExpanded(device_group.key));
    }

    category_item->setText(
      kStateColumn,
      QString("%1 total / %2 available")
      .arg([&group]() {
        int total = 0;
        for (const auto & device_group : group->second) {
          total += static_cast<int>(device_group.topics.size());
        }
        return total;
      }())
      .arg(available_in_category));
    category_item->setToolTip(
      kNameColumn,
      QString("Sensor category: %1\nAvailable topics: %2")
      .arg(QString::fromStdString(toString(category)))
      .arg(available_in_category));
    category_item->setCheckState(kNameColumn, determineAggregateCheckState(category_item));
    category_item->setExpanded(shouldGroupStartExpanded(toString(category)));
  }

  tree_widget_->resizeColumnToContents(kNameColumn);
  syncEnabledTopicsFromTree();
}

void SensorTreePanel::updateStatusLabel()
{
  const auto topics = sensor_catalog_.allTopics();

  std::size_t available_count = 0;
  std::size_t enabled_count = 0;
  for (const auto & topic : topics) {
    if (topic.is_available) {
      ++available_count;
    }
    if (display_registry_.isEnabled(topic.name)) {
      ++enabled_count;
    }
  }

  status_label_->setText(
    QString("Supported topics: %1 | Available: %2 | Enabled displays: %3")
    .arg(static_cast<int>(topics.size()))
    .arg(static_cast<int>(available_count))
    .arg(static_cast<int>(enabled_count)));
}

void SensorTreePanel::syncEnabledTopicsFromTree()
{
  persisted_enabled_topics_.clear();

  if (!tree_widget_) {
    return;
  }

  std::vector<QTreeWidgetItem *> pending_items;
  for (int index = 0; index < tree_widget_->topLevelItemCount(); ++index) {
    if (auto * item = tree_widget_->topLevelItem(index)) {
      pending_items.push_back(item);
    }
  }

  while (!pending_items.empty()) {
    auto * item = pending_items.back();
    pending_items.pop_back();
    if (!item) {
      continue;
    }

    const auto item_type = static_cast<TreeItemType>(
      item->data(kNameColumn, kTreeItemTypeRole).toInt());
    if (item_type == TreeItemType::Topic && item->checkState(kNameColumn) == Qt::Checked) {
      const auto topic_name = item->data(kNameColumn, Qt::UserRole).toString().toStdString();
      if (!topic_name.empty()) {
        persisted_enabled_topics_.insert(topic_name);
      }
    }

    for (int child_index = 0; child_index < item->childCount(); ++child_index) {
      pending_items.push_back(item->child(child_index));
    }
  }
}

void SensorTreePanel::setAllTopicsEnabled(bool enabled)
{
  for (const auto & topic : sensor_catalog_.allTopics()) {
    if (display_registry_.hasDisplay(topic.name)) {
      display_registry_.setEnabled(topic.name, enabled);
    }
  }

  setAllTopicStates(
    sensor_catalog_.allTopics(),
    enabled,
    &persisted_enabled_topics_,
    &suppressed_auto_enable_topics_);

  rebuildTree();
  reconcileDesiredDisplays();
  updateStatusLabel();
  Q_EMIT configChanged();
}

void SensorTreePanel::reconcileDesiredDisplays()
{
  auto * visualization_manager =
    dynamic_cast<rviz_common::VisualizationManager *>(getDisplayContext());
  if (!visualization_manager) {
    return;
  }

  SensorClassifier classifier;
  for (const auto & topic : sensor_catalog_.allTopics()) {
    if (!topic.is_available || !shouldTopicStartEnabled(topic.name)) {
      continue;
    }

    if (display_registry_.hasDisplay(topic.name)) {
      display_registry_.setEnabled(topic.name, true);
      continue;
    }

    const std::string display_type = classifier.lookupDisplayType(topic.message_type);
    auto * display = auto_display_factory_.createDisplay(
      visualization_manager,
      display_type,
      topic.name,
      topic.message_type);

    if (!display) {
      continue;
    }

    display_registry_.registerDisplay(
      ManagedDisplay{
        topic.name,
        topic.message_type,
        display_type,
        display->getName().toStdString(),
        true
      },
      display);
    persisted_enabled_topics_.insert(topic.name);
  }
}

bool SensorTreePanel::shouldTopicStartEnabled(const std::string & topic_name) const
{
  return display_registry_.isEnabled(topic_name) || persisted_enabled_topics_.count(topic_name) > 0;
}

bool SensorTreePanel::shouldGroupStartExpanded(const std::string & group_key) const
{
  const auto iter = persisted_group_expansion_.find(group_key);
  if (iter == persisted_group_expansion_.end()) {
    return true;
  }

  return iter->second;
}

void SensorTreePanel::rememberGroupExpansionStates()
{
  if (!tree_widget_) {
    return;
  }

  std::vector<QTreeWidgetItem *> pending_items;
  for (int index = 0; index < tree_widget_->topLevelItemCount(); ++index) {
    if (auto * item = tree_widget_->topLevelItem(index)) {
      pending_items.push_back(item);
    }
  }

  while (!pending_items.empty()) {
    auto * item = pending_items.back();
    pending_items.pop_back();
    if (!item) {
      continue;
    }

    if (item->childCount() > 0) {
      const auto group_key = item->data(kNameColumn, kTreeItemGroupKeyRole).toString().toStdString();
      if (!group_key.empty()) {
        persisted_group_expansion_[group_key] = item->isExpanded();
      }
    }

    for (int index = 0; index < item->childCount(); ++index) {
      pending_items.push_back(item->child(index));
    }
  }
}

}  // namespace rviz_auto_sensor_panel

PLUGINLIB_EXPORT_CLASS(rviz_auto_sensor_panel::SensorTreePanel, rviz_common::Panel)
