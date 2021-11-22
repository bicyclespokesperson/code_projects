#include "utils.h"

namespace
{

std::string c_feature_toggle_file_path{"./feature_set.txt"};

std::unordered_map<std::string, bool> parse_features(std::string_view filename)
{
  std::unordered_map<std::string, bool> enabled_features;
  std::ifstream infile{filename};
  if (!infile.good())
  {
    std::cerr << "Could not open feature toggle file: " << c_feature_toggle_file_path << "\n";
    return enabled_features;
  }

  std::string line;
  line.reserve(256);
  while (std::getline(infile, line))
  {
    line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
    std::transform(line.begin(), line.end(), line.begin(), tolower);
    if (line.empty())
    {
      continue;
    }
    auto const eq = line.find('=');
    if (eq == std::string::npos)
    {
      std::cerr << "Unexpected line in toggle file: " << line << "\n";
      return enabled_features;
    }

    auto const toggle_name = line.substr(0, eq);
    auto const toggle_val = line.substr(eq + 1, std::string::npos) == "true";
    enabled_features[toggle_name] = toggle_val;
  }

  return enabled_features;
}

} // namespace

bool is_feature_enabled(std::string const& feature_name)
{
  static const auto features = parse_features(c_feature_toggle_file_path);

  auto const& search = features.find(feature_name);
  if (search != features.cend() && search->second)
  {
    return true;
  }
  return false;
}
