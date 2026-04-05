#include "rviz_auto_sensor_panel/sensor_classifier.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace rviz_auto_sensor_panel
{

namespace
{

constexpr char kLaserScanMessageType[] = "sensor_msgs/msg/LaserScan";
constexpr char kImageMessageType[] = "sensor_msgs/msg/Image";
constexpr char kPointCloud2MessageType[] = "sensor_msgs/msg/PointCloud2";
constexpr char kOdometryMessageType[] = "nav_msgs/msg/Odometry";
constexpr char kPathMessageType[] = "nav_msgs/msg/Path";
constexpr char kPoseArrayMessageType[] = "geometry_msgs/msg/PoseArray";
constexpr char kMarkerMessageType[] = "visualization_msgs/msg/Marker";
constexpr char kMarkerArrayMessageType[] = "visualization_msgs/msg/MarkerArray";

constexpr char kLaserScanDisplayType[] = "rviz_default_plugins/LaserScan";
constexpr char kImageDisplayType[] = "rviz_default_plugins/Image";
constexpr char kPointCloud2DisplayType[] = "rviz_default_plugins/PointCloud2";
constexpr char kOdometryDisplayType[] = "rviz_default_plugins/Odometry";
constexpr char kPathDisplayType[] = "rviz_default_plugins/Path";
constexpr char kPoseArrayDisplayType[] = "rviz_default_plugins/PoseArray";
constexpr char kMarkerDisplayType[] = "rviz_default_plugins/Marker";
constexpr char kMarkerArrayDisplayType[] = "rviz_default_plugins/MarkerArray";
constexpr char kRulesEnvVar[] = "RVIZ_AUTO_SENSOR_PANEL_RULES_FILE";

std::string sourceTreeRulesPath()
{
#ifdef RVIZ_AUTO_SENSOR_PANEL_SOURCE_DIR
  return (std::filesystem::path(RVIZ_AUTO_SENSOR_PANEL_SOURCE_DIR) / "config" /
         "topic_grouping_rules.ini").string();
#else
  return "";
#endif
}

std::string toLowerCopy(const std::string & value)
{
  std::string lowered = value;
  std::transform(
    lowered.begin(), lowered.end(), lowered.begin(),
    [](unsigned char character) {return static_cast<char>(std::tolower(character));});
  return lowered;
}

std::vector<std::string> defaultDirectionPriority()
{
  return {
    "front", "left", "center", "centre", "right", "rear", "back", "top", "bottom"
  };
}

GroupingRules buildDefaultRules()
{
  GroupingRules rules;
  rules.ignored_leading_tokens = {"demo", "sim", "simulator", "example", "samples"};
  rules.token_aliases = {
    {"cam", "camera"},
    {"pc", "pointcloud"},
    {"pcl", "pointcloud"}
  };
  rules.direction_priority = defaultDirectionPriority();
  return rules;
}

std::string trimCopy(const std::string & value)
{
  const auto begin = std::find_if_not(
    value.begin(), value.end(),
    [](unsigned char character) {return std::isspace(character) != 0;});
  const auto end = std::find_if_not(
    value.rbegin(), value.rend(),
    [](unsigned char character) {return std::isspace(character) != 0;}).base();

  if (begin >= end) {
    return "";
  }

  return std::string(begin, end);
}

std::vector<std::string> splitCommaSeparated(const std::string & value)
{
  std::vector<std::string> parts;
  std::stringstream stream(value);
  std::string part;
  while (std::getline(stream, part, ',')) {
    const auto trimmed = trimCopy(part);
    if (!trimmed.empty()) {
      parts.push_back(toLowerCopy(trimmed));
    }
  }
  return parts;
}

void applyCommaSeparatedList(const std::string & value, std::set<std::string> * output)
{
  if (!output) {
    return;
  }

  output->clear();
  for (const auto & part : splitCommaSeparated(value)) {
    output->insert(part);
  }
}

void applyCommaSeparatedVector(const std::string & value, std::vector<std::string> * output)
{
  if (!output) {
    return;
  }

  output->clear();
  for (const auto & part : splitCommaSeparated(value)) {
    output->push_back(part);
  }
}

void loadRulesOverrides(const std::string & path, GroupingRules * rules)
{
  if (!rules) {
    return;
  }

  std::ifstream input(path);
  if (!input.good()) {
    return;
  }

  std::string current_section;
  std::string line;
  while (std::getline(input, line)) {
    const auto trimmed = trimCopy(line);
    if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == ';') {
      continue;
    }

    if (trimmed.front() == '[' && trimmed.back() == ']') {
      current_section = toLowerCopy(trimCopy(trimmed.substr(1, trimmed.size() - 2)));
      continue;
    }

    const auto separator = trimmed.find('=');
    if (separator == std::string::npos) {
      continue;
    }

    const auto key = toLowerCopy(trimCopy(trimmed.substr(0, separator)));
    const auto value = trimCopy(trimmed.substr(separator + 1));

    if (current_section == "grouping" && key == "ignored_leading_tokens") {
      applyCommaSeparatedList(value, &rules->ignored_leading_tokens);
      continue;
    }

    if (current_section == "grouping" && key == "direction_priority") {
      applyCommaSeparatedVector(value, &rules->direction_priority);
      continue;
    }

    if (current_section == "aliases" && !key.empty() && !value.empty()) {
      rules->token_aliases[key] = toLowerCopy(value);
    }
  }
}

GroupingRules loadGroupingRules()
{
  auto rules = buildDefaultRules();

  if (const char * rules_path = std::getenv(kRulesEnvVar)) {
    loadRulesOverrides(rules_path, &rules);
    return rules;
  }

  try {
    const auto package_share =
      ament_index_cpp::get_package_share_directory("rviz_auto_sensor_panel");
    loadRulesOverrides(package_share + "/config/topic_grouping_rules.ini", &rules);
  } catch (const std::exception &) {
    loadRulesOverrides(sourceTreeRulesPath(), &rules);
  }

  return rules;
}

std::vector<std::string> splitTopicTokens(
  const std::string & topic_name,
  const GroupingRules & rules)
{
  std::vector<std::string> tokens;
  std::stringstream stream(topic_name);
  std::string path_token;

  while (std::getline(stream, path_token, '/')) {
    if (path_token.empty()) {
      continue;
    }

    std::string current_word;
    auto flush_word = [&tokens, &current_word, &rules]() {
        if (!current_word.empty()) {
          auto lowered = toLowerCopy(current_word);
          const auto alias = rules.token_aliases.find(lowered);
          if (alias != rules.token_aliases.end()) {
            lowered = alias->second;
          }
          tokens.push_back(lowered);
          current_word.clear();
        }
      };

    for (const char character : path_token) {
      if (character == '_' || character == '-') {
        flush_word();
        continue;
      }

      current_word.push_back(character);
    }

    flush_word();
  }

  return tokens;
}

std::string topicLeafLabel(const std::string & topic_name)
{
  const auto last_separator = topic_name.find_last_of('/');
  if (last_separator == std::string::npos) {
    return topic_name;
  }

  return topic_name.substr(last_separator + 1);
}

std::string joinTitleCase(const std::vector<std::string> & tokens)
{
  std::string result;
  for (const auto & token : tokens) {
    if (token.empty()) {
      continue;
    }

    std::string titled = token;
    titled[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(titled[0])));
    for (std::size_t index = 1; index < titled.size(); ++index) {
      titled[index] = static_cast<char>(std::tolower(static_cast<unsigned char>(titled[index])));
    }

    if (!result.empty()) {
      result += " ";
    }
    result += titled;
  }

  return result;
}

std::string joinSnakeCase(const std::vector<std::string> & tokens)
{
  std::string result;
  for (const auto & token : tokens) {
    if (token.empty()) {
      continue;
    }

    if (!result.empty()) {
      result += "_";
    }
    result += token;
  }

  return result;
}

bool contains(const std::set<std::string> & values, const std::string & candidate)
{
  return values.find(candidate) != values.end();
}

std::string categoryLabel(SensorCategory category)
{
  switch (category) {
    case SensorCategory::Lidar:
      return "Lidar";
    case SensorCategory::Camera:
      return "Camera";
    case SensorCategory::PointCloud:
      return "Point Cloud";
    case SensorCategory::Navigation:
      return "Navigation";
    case SensorCategory::Visualization:
      return "Visualization";
    case SensorCategory::Unknown:
    default:
      return "Unknown";
  }
}

std::string categoryKey(SensorCategory category)
{
  switch (category) {
    case SensorCategory::Lidar:
      return "lidar";
    case SensorCategory::Camera:
      return "camera";
    case SensorCategory::PointCloud:
      return "point_cloud";
    case SensorCategory::Navigation:
      return "navigation";
    case SensorCategory::Visualization:
      return "visualization";
    case SensorCategory::Unknown:
    default:
      return "unknown";
  }
}

std::set<std::string> genericSuffixesFor(SensorCategory category)
{
  switch (category) {
    case SensorCategory::Lidar:
      return {"scan", "laserscan", "raw", "filtered"};
    case SensorCategory::Camera:
      return {
        "image", "images", "image_raw", "image_rect", "image_rect_color", "image_color",
        "compressed", "raw", "rect", "color", "rgb", "mono", "camera_info"
      };
    case SensorCategory::PointCloud:
      return {"points", "pointcloud", "point_cloud", "velodyne_points", "cloud"};
    case SensorCategory::Navigation:
      return {"odom", "odometry", "path", "posearray", "pose_array", "poses", "plan", "trajectory"};
    case SensorCategory::Visualization:
      return {"marker", "markers", "markerarray", "marker_array"};
    case SensorCategory::Unknown:
    default:
      return {};
  }
}

std::set<std::string> genericTypeWordsFor(SensorCategory category)
{
  switch (category) {
    case SensorCategory::Lidar:
      return {"lidar", "laser", "laserscan", "scan"};
    case SensorCategory::Camera:
      return {"camera", "cam", "image"};
    case SensorCategory::PointCloud:
      return {"pointcloud", "point_cloud", "points", "cloud", "velodyne"};
    case SensorCategory::Navigation:
      return {
        "odom", "odometry", "path", "poses",
        "posearray", "pose_array", "trajectory", "plan", "route"
      };
    case SensorCategory::Visualization:
      return {"visualization", "viz", "marker", "markers", "markerarray", "marker_array"};
    case SensorCategory::Unknown:
    default:
      return {};
  }
}

std::set<std::string> categorySelfWordsFor(SensorCategory category)
{
  switch (category) {
    case SensorCategory::Lidar:
      return {"lidar", "laser"};
    case SensorCategory::Camera:
      return {"camera", "cam"};
    case SensorCategory::PointCloud:
      return {"pointcloud", "point_cloud", "cloud", "velodyne"};
    case SensorCategory::Navigation:
      return {"navigation", "localization", "planning"};
    case SensorCategory::Visualization:
      return {"visualization", "viz"};
    case SensorCategory::Unknown:
    default:
      return {};
  }
}

std::vector<std::string> directionWords()
{
  return {
    "front", "rear", "left", "right", "top", "bottom", "center", "centre",
    "upper", "lower", "middle", "mid", "back", "forward", "aft"
  };
}

bool isDirectionWord(const std::string & token, const GroupingRules & rules)
{
  const auto directions = rules.direction_priority.empty() ?
    defaultDirectionPriority() : rules.direction_priority;
  return std::find(directions.begin(), directions.end(), token) != directions.end();
}

std::vector<std::string> deriveIdentityTokens(
  const std::vector<std::string> & topic_tokens,
  SensorCategory category,
  const GroupingRules & rules)
{
  auto identity_tokens = topic_tokens;
  const auto suffixes = genericSuffixesFor(category);
  const auto type_words = genericTypeWordsFor(category);

  while (!identity_tokens.empty() && contains(suffixes, identity_tokens.back())) {
    identity_tokens.pop_back();
  }

  std::vector<std::string> compact_tokens;
  compact_tokens.reserve(identity_tokens.size());
  for (const auto & token : identity_tokens) {
    if (contains(type_words, token)) {
      continue;
    }
    compact_tokens.push_back(token);
  }

  while (
    !compact_tokens.empty() &&
    contains(rules.ignored_leading_tokens, compact_tokens.front()))
  {
    compact_tokens.erase(compact_tokens.begin());
  }

  return compact_tokens;
}

TopicProfile buildTopicProfile(
  const std::string & topic_name,
  SensorCategory category,
  const GroupingRules & rules)
{
  TopicProfile profile;
  profile.category = category;
  profile.topic_label = topicLeafLabel(topic_name);

  if (category == SensorCategory::Unknown) {
    profile.group_key = "unknown";
    profile.group_label = "Unknown";
    return profile;
  }

  const auto topic_tokens = splitTopicTokens(topic_name, rules);
  const auto identity_tokens = deriveIdentityTokens(topic_tokens, category, rules);

  std::vector<std::string> label_tokens;
  std::vector<std::string> key_tokens;

  for (const auto & token : identity_tokens) {
    if (isDirectionWord(token, rules)) {
      label_tokens.push_back(token);
      key_tokens.push_back(token);
      continue;
    }

    label_tokens.push_back(token);
    key_tokens.push_back(token);
  }

  const auto category_name = categoryLabel(category);
  const auto category_key_name = categoryKey(category);
  const auto self_words = categorySelfWordsFor(category);

  if (label_tokens.empty()) {
    profile.group_label = category_name;
    profile.group_key = category_key_name;
    return profile;
  }

  const auto last_token = label_tokens.back();
  if (contains(self_words, last_token)) {
    profile.group_label = joinTitleCase(label_tokens);
  } else {
    profile.group_label = joinTitleCase(label_tokens) + " " + category_name;
  }
  profile.group_key = category_key_name + ":" + joinSnakeCase(key_tokens);
  return profile;
}

}  // namespace

SensorClassifier::SensorClassifier()
: rules_(loadGroupingRules())
{
}

SensorCategory SensorClassifier::classifyMessageType(const std::string & message_type) const
{
  if (message_type == kLaserScanMessageType) {
    return SensorCategory::Lidar;
  }

  if (message_type == kImageMessageType) {
    return SensorCategory::Camera;
  }

  if (message_type == kPointCloud2MessageType) {
    return SensorCategory::PointCloud;
  }

  if (
    message_type == kOdometryMessageType ||
    message_type == kPathMessageType ||
    message_type == kPoseArrayMessageType)
  {
    return SensorCategory::Navigation;
  }

  if (
    message_type == kMarkerMessageType ||
    message_type == kMarkerArrayMessageType)
  {
    return SensorCategory::Visualization;
  }

  return SensorCategory::Unknown;
}

TopicProfile SensorClassifier::classifyTopic(
  const std::string & topic_name,
  const std::string & message_type) const
{
  return buildTopicProfile(topic_name, classifyMessageType(message_type), rules_);
}

bool SensorClassifier::isSupportedMessageType(const std::string & message_type) const
{
  return classifyMessageType(message_type) != SensorCategory::Unknown;
}

std::string SensorClassifier::lookupDisplayType(const std::string & message_type) const
{
  if (message_type == kLaserScanMessageType) {
    return kLaserScanDisplayType;
  }

  if (message_type == kImageMessageType) {
    return kImageDisplayType;
  }

  if (message_type == kPointCloud2MessageType) {
    return kPointCloud2DisplayType;
  }

  if (message_type == kOdometryMessageType) {
    return kOdometryDisplayType;
  }

  if (message_type == kPathMessageType) {
    return kPathDisplayType;
  }

  if (message_type == kPoseArrayMessageType) {
    return kPoseArrayDisplayType;
  }

  if (message_type == kMarkerMessageType) {
    return kMarkerDisplayType;
  }

  if (message_type == kMarkerArrayMessageType) {
    return kMarkerArrayDisplayType;
  }

  return "";
}

const GroupingRules & SensorClassifier::rules() const
{
  return rules_;
}

std::string toString(SensorCategory category)
{
  return categoryLabel(category);
}

std::vector<SensorCategory> supportedCategories()
{
  return {
    SensorCategory::Lidar,
    SensorCategory::Camera,
    SensorCategory::PointCloud,
    SensorCategory::Navigation,
    SensorCategory::Visualization
  };
}

}  // namespace rviz_auto_sensor_panel
