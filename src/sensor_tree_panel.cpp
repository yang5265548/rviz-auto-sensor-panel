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
constexpr char kExpandedCategoriesKey[] = "ExpandedCategories";
constexpr char kTopicNameKey[] = "TopicName";
constexpr char kCategoryNameKey[] = "CategoryName";
constexpr char kExpandedKey[] = "Expanded";
constexpr char kAutoEnableNewTopicsKey[] = "AutoEnableNewTopics";

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
  persisted_category_expansion_.clear();

  const auto enabled_topics = config.mapGetChild(kEnabledTopicsKey);
  for (int index = 0; index < enabled_topics.listLength(); ++index) {
    QString topic_name;
    if (enabled_topics.listChildAt(index).mapGetString(kTopicNameKey, &topic_name)) {
      persisted_enabled_topics_.insert(topic_name.toStdString());
    }
  }

  const auto expanded_categories = config.mapGetChild(kExpandedCategoriesKey);
  for (int index = 0; index < expanded_categories.listLength(); ++index) {
    auto entry = expanded_categories.listChildAt(index);
    QString category_name;
    bool is_expanded = true;

    if (!entry.mapGetString(kCategoryNameKey, &category_name)) {
      continue;
    }

    entry.mapGetBool(kExpandedKey, &is_expanded);
    persisted_category_expansion_[category_name.toStdString()] = is_expanded;
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

  auto expanded_categories = config.mapMakeChild(kExpandedCategoriesKey);
  for (const auto & [category_name, expanded] : persisted_category_expansion_) {
    auto entry = expanded_categories.listAppendNew();
    entry.mapSetValue(kCategoryNameKey, QString::fromStdString(category_name));
    entry.mapSetValue(kExpandedKey, expanded);
  }

  config.mapSetValue(kAutoEnableNewTopicsKey, auto_enable_new_topics_);
}

void SensorTreePanel::refreshTopics()
{
  std::set<std::string> known_topics;
  for (const auto & topic : sensor_catalog_.allTopics()) {
    known_topics.insert(topic.name);
  }

  const auto latest_topics = topic_scanner_.scan(node_);
  if (auto_enable_new_topics_) {
    for (const auto & topic : latest_topics) {
      if (known_topics.count(topic.name) == 0) {
        persisted_enabled_topics_.insert(topic.name);
      }
    }
  }

  sensor_catalog_.update(latest_topics);
  rememberCategoryExpansionStates();
  rebuildTree();
  reconcileDesiredDisplays();
  updateStatusLabel();
}

void SensorTreePanel::handleRefreshClicked()
{
  refreshTopics();
}

void SensorTreePanel::handleTreeItemChanged(QTreeWidgetItem * item, int column)
{
  if (!item || column != kNameColumn || item->childCount() > 0) {
    return;
  }

  const auto topic_name = item->data(kNameColumn, Qt::UserRole).toString().toStdString();
  const auto message_type = item->data(kNameColumn, Qt::UserRole + 1).toString().toStdString();

  const bool should_enable = item->checkState(kNameColumn) == Qt::Checked;
  if (should_enable) {
    persisted_enabled_topics_.insert(topic_name);
  } else {
    persisted_enabled_topics_.erase(topic_name);
  }

  if (display_registry_.hasDisplay(topic_name)) {
    display_registry_.setEnabled(topic_name, should_enable);
    Q_EMIT configChanged();
    updateStatusLabel();
    return;
  }

  if (!should_enable) {
    Q_EMIT configChanged();
    updateStatusLabel();
    return;
  }

  auto * visualization_manager =
    dynamic_cast<rviz_common::VisualizationManager *>(getDisplayContext());
  if (!visualization_manager) {
    Q_EMIT configChanged();
    updateStatusLabel();
    return;
  }

  SensorClassifier classifier;
  const std::string display_type = classifier.lookupDisplayType(message_type);
  auto * display = auto_display_factory_.createDisplay(
    visualization_manager, display_type, topic_name, message_type);

  if (!display) {
    persisted_enabled_topics_.erase(topic_name);
    item->setCheckState(kNameColumn, Qt::Unchecked);
    updateStatusLabel();
    return;
  }

  display_registry_.registerDisplay(
    ManagedDisplay{topic_name, message_type, display_type, display->getName().toStdString(), true},
    display);
  Q_EMIT configChanged();
  updateStatusLabel();
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
    rememberCategoryExpansionStates();
    Q_EMIT configChanged();
  });
  QObject::connect(tree_widget_, &QTreeWidget::itemCollapsed, this, [this](QTreeWidgetItem *) {
    rememberCategoryExpansionStates();
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
    category_item->setFlags(category_item->flags() & ~Qt::ItemIsUserCheckable);

    const auto group = groups.find(category);
    if (group == groups.end()) {
      category_item->setExpanded(shouldCategoryStartExpanded(category));
      continue;
    }

    int available_in_group = 0;
    for (const auto & topic : group->second) {
      auto * topic_item = new QTreeWidgetItem(category_item);
      topic_item->setFlags(topic_item->flags() | Qt::ItemIsUserCheckable);
      topic_item->setText(
        kNameColumn,
        QString::fromStdString(topic.name + (topic.is_available ? "" : " (offline)")));
      topic_item->setText(kStateColumn, topic.is_available ? "Available" : "Offline");
      topic_item->setData(kNameColumn, Qt::UserRole, QString::fromStdString(topic.name));
      topic_item->setData(kNameColumn, Qt::UserRole + 1, QString::fromStdString(topic.message_type));
      topic_item->setCheckState(
        kNameColumn,
        shouldTopicStartEnabled(topic.name) ? Qt::Checked : Qt::Unchecked);

      if (topic.is_available) {
        ++available_in_group;
      }
    }

    category_item->setText(
      kStateColumn,
      QString("%1 total / %2 available")
      .arg(static_cast<int>(group->second.size()))
      .arg(available_in_group));
    category_item->setExpanded(shouldCategoryStartExpanded(category));
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
  const auto previously_enabled_topics = persisted_enabled_topics_;
  persisted_enabled_topics_.clear();

  const auto groups = sensor_catalog_.groupedTopics();
  for (const auto & [_, topics] : groups) {
    for (const auto & topic : topics) {
      if (display_registry_.isEnabled(topic.name) || previously_enabled_topics.count(topic.name) > 0) {
        persisted_enabled_topics_.insert(topic.name);
      }
    }
  }
}

void SensorTreePanel::setAllTopicsEnabled(bool enabled)
{
  for (const auto & topic : sensor_catalog_.allTopics()) {
    if (enabled) {
      persisted_enabled_topics_.insert(topic.name);
    } else {
      persisted_enabled_topics_.erase(topic.name);
    }

    if (display_registry_.hasDisplay(topic.name)) {
      display_registry_.setEnabled(topic.name, enabled);
    }
  }

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

bool SensorTreePanel::shouldCategoryStartExpanded(SensorCategory category) const
{
  const auto category_name = toString(category);
  const auto iter = persisted_category_expansion_.find(category_name);
  if (iter == persisted_category_expansion_.end()) {
    return true;
  }

  return iter->second;
}

void SensorTreePanel::rememberCategoryExpansionStates()
{
  if (!tree_widget_) {
    return;
  }

  for (int index = 0; index < tree_widget_->topLevelItemCount(); ++index) {
    auto * category_item = tree_widget_->topLevelItem(index);
    if (!category_item) {
      continue;
    }

    persisted_category_expansion_[category_item->text(kNameColumn).toStdString()] =
      category_item->isExpanded();
  }
}

}  // namespace rviz_auto_sensor_panel

PLUGINLIB_EXPORT_CLASS(rviz_auto_sensor_panel::SensorTreePanel, rviz_common::Panel)
