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

#endif // UTILS_H
