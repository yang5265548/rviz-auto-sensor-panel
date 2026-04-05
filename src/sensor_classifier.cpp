#include "rviz_auto_sensor_panel/sensor_classifier.hpp"

#include <algorithm>
#include <cctype>
#include <functional>
#include <set>
#include <sstream>
#include <vector>

namespace rviz_auto_sensor_panel
{

namespace
{

constexpr char kLaserScanMessageType[] = "sensor_msgs/msg/LaserScan";
constexpr char kImageMessageType[] = "sensor_msgs/msg/Image";
constexpr char kPointCloud2MessageType[] = "sensor_msgs/msg/PointCloud2";

constexpr char kLaserScanDisplayType[] = "rviz_default_plugins/LaserScan";
constexpr char kImageDisplayType[] = "rviz_default_plugins/Image";
constexpr char kPointCloud2DisplayType[] = "rviz_default_plugins/PointCloud2";

std::string toLowerCopy(const std::string & value)
{
  std::string lowered = value;
  std::transform(
    lowered.begin(), lowered.end(), lowered.begin(),
    [](unsigned char character) {return static_cast<char>(std::tolower(character));});
  return lowered;
}

std::vector<std::string> splitTopicTokens(const std::string & topic_name)
{
  std::vector<std::string> tokens;
  std::stringstream stream(topic_name);
  std::string path_token;

  while (std::getline(stream, path_token, '/')) {
    if (path_token.empty()) {
      continue;
    }

    std::string current_word;
    auto flush_word = [&tokens, &current_word]() {
        if (!current_word.empty()) {
          tokens.push_back(toLowerCopy(current_word));
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

bool isDirectionWord(const std::string & token)
{
  const auto directions = directionWords();
  return std::find(directions.begin(), directions.end(), token) != directions.end();
}

std::set<std::string> leadingContextWords()
{
  return {"demo", "sim", "simulator", "example", "samples"};
}

std::vector<std::string> deriveIdentityTokens(
  const std::vector<std::string> & topic_tokens,
  SensorCategory category)
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

  const auto context_words = leadingContextWords();
  while (!compact_tokens.empty() && contains(context_words, compact_tokens.front())) {
    compact_tokens.erase(compact_tokens.begin());
  }

  return compact_tokens;
}

TopicProfile buildTopicProfile(const std::string & topic_name, SensorCategory category)
{
  TopicProfile profile;
  profile.category = category;
  profile.topic_label = topicLeafLabel(topic_name);

  if (category == SensorCategory::Unknown) {
    profile.group_key = "unknown";
    profile.group_label = "Unknown";
    return profile;
  }

  const auto topic_tokens = splitTopicTokens(topic_name);
  const auto identity_tokens = deriveIdentityTokens(topic_tokens, category);

  std::vector<std::string> label_tokens;
  std::vector<std::string> key_tokens;

  for (const auto & token : identity_tokens) {
    if (isDirectionWord(token)) {
      label_tokens.push_back(token);
      key_tokens.push_back(token);
      continue;
    }

    label_tokens.push_back(token);
    key_tokens.push_back(token);
  }

  const auto category_name = categoryLabel(category);
  const auto category_key_name = categoryKey(category);

  if (label_tokens.empty()) {
    profile.group_label = category_name;
    profile.group_key = category_key_name;
    return profile;
  }

  profile.group_label = joinTitleCase(label_tokens) + " " + category_name;
  profile.group_key = category_key_name + ":" + joinSnakeCase(key_tokens);
  return profile;
}

}  // namespace

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

  return SensorCategory::Unknown;
}

TopicProfile SensorClassifier::classifyTopic(
  const std::string & topic_name,
  const std::string & message_type) const
{
  return buildTopicProfile(topic_name, classifyMessageType(message_type));
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

  return "";
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
    SensorCategory::PointCloud
  };
}

}  // namespace rviz_auto_sensor_panel
