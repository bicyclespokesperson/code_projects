#ifndef UTILS_H
#define UTILS_H

template <class T>
std::string format_with_commas(T value)
{
  std::stringstream ss;
  ss.imbue(std::locale(""));
  ss << std::fixed << std::setprecision(2) << value;
  return ss.str();
}

template <typename T>
void print_vector(std::ostream& os, std::vector<T> const& v)
{
  std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
  os << "\n";
}

/**
 * Returns true if a feature is enabled
 *
 * Features can be enabled/disables in a feature toggle file (feature_set.txt)
 * This file should be filled with lines of the form toggle_name=true or 
 * toggle_name = false. 
 */
bool is_feature_enabled(std::string const& feature_name);

#endif // UTILS_H
