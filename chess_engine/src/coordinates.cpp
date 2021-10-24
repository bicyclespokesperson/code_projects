#include <coordinates.h>


std::optional<Coordinates> Coordinates::from_str(std::string_view str)
{
  std::cout << "Converting string to coord: " << str << std::endl;
  if (str.size() < 2)
  {
    return {};
  }

  auto x = static_cast<int8_t>(toupper(str[0]) - 'A');
  auto y = static_cast<int8_t>(str[1] - '1');

  std::cout << "x: " << std::to_string(x) << ", y: " << std::to_string(y) << std::endl;
  
  if (x < 0 || x > 7 || y < 0 || y > 7)
  {
    return {};
  }

  return Coordinates{x, y};
}

std::ostream& operator<<(std::ostream& os, Coordinates const& self)
{
  os << "{" << std::to_string(self.x()) << ", " << std::to_string(self.y()) << "}";
  return os;
}
