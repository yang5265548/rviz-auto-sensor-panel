#include "rviz_auto_sensor_panel/sensor_tree_panel.hpp"

#include <utility>

#include <QHBoxLayout>
#include <QSignalBlocker>
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

}  // namespace

SensorTreePanel::SensorTreePanel(QWidget * parent)
: rviz_common::Panel(parent),
  status_label_(nullptr),
  refresh_button_(nullptr),
  tree_widget_(nullptr),
  refresh_timer_(nullptr),
  topic_scanner_(SensorClassifier())
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

void SensorTreePanel::refreshTopics()
{
  sensor_catalog_.update(topic_scanner_.scan(node_));
  rebuildTree();
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

  if (display_registry_.hasDisplay(topic_name)) {
    display_registry_.setEnabled(topic_name, should_enable);
    updateStatusLabel();
    return;
  }

  if (!should_enable) {
    updateStatusLabel();
    return;
  }

  auto * visualization_manager =
    dynamic_cast<rviz_common::VisualizationManager *>(getDisplayContext());
  if (!visualization_manager) {
    updateStatusLabel();
    return;
  }

  SensorClassifier classifier;
  const std::string display_type = classifier.lookupDisplayType(message_type);
  auto * display = auto_display_factory_.createDisplay(
    visualization_manager, display_type, topic_name, message_type);

  if (!display) {
    item->setCheckState(kNameColumn, Qt::Unchecked);
    updateStatusLabel();
    return;
  }

  display_registry_.registerDisplay(
    ManagedDisplay{topic_name, message_type, display_type, display->getName().toStdString(), true},
    display);
  updateStatusLabel();
}

void SensorTreePanel::buildUi()
{
  auto * layout = new QVBoxLayout(this);

  status_label_ = new QLabel("No supported topics discovered yet.");
  refresh_button_ = new QPushButton("Refresh");
  tree_widget_ = new QTreeWidget();
  refresh_timer_ = new QTimer(this);

  tree_widget_->setColumnCount(kTreeColumnCount);
  tree_widget_->setHeaderLabels(QStringList() << "Sensor / Topic" << "State");
  tree_widget_->setRootIsDecorated(true);
  tree_widget_->setAlternatingRowColors(true);

  auto * header_layout = new QHBoxLayout();
  header_layout->addWidget(status_label_);
  header_layout->addStretch();
  header_layout->addWidget(refresh_button_);

  layout->addLayout(header_layout);
  layout->addWidget(tree_widget_);

  QObject::connect(refresh_button_, &QPushButton::clicked, this, &SensorTreePanel::handleRefreshClicked);
  QObject::connect(refresh_timer_, &QTimer::timeout, this, &SensorTreePanel::refreshTopics);
  QObject::connect(
    tree_widget_, &QTreeWidget::itemChanged, this, &SensorTreePanel::handleTreeItemChanged);
}

void SensorTreePanel::rebuildTree()
{
  const QSignalBlocker blocker(tree_widget_);
  tree_widget_->clear();

  const auto groups = sensor_catalog_.groupedTopics();

  for (const auto category : supportedCategories()) {
    auto * category_item = new QTreeWidgetItem(tree_widget_);
    category_item->setText(kNameColumn, QString::fromStdString(toString(category)));
    category_item->setText(kStateColumn, "Group");
    category_item->setFlags(category_item->flags() & ~Qt::ItemIsUserCheckable);

    const auto group = groups.find(category);
    if (group == groups.end()) {
      continue;
    }

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
        display_registry_.isEnabled(topic.name) ? Qt::Checked : Qt::Unchecked);
    }

    category_item->setExpanded(true);
  }

  tree_widget_->resizeColumnToContents(kNameColumn);
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

void SensorTreePanel::setTreeSignalsBlocked(bool blocked)
{
  tree_widget_->blockSignals(blocked);
}

}  // namespace rviz_auto_sensor_panel

PLUGINLIB_EXPORT_CLASS(rviz_auto_sensor_panel::SensorTreePanel, rviz_common::Panel)
