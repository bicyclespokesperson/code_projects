#ifndef COLOR_H
#define COLOR_H

#include <cstdint>

namespace gar {

class Color {
 public:
  Color() = default;
  Color(Color const& other) = default;
  Color(Color&& other) = default;

  Color& operator=(Color const& other) = default;
  Color& operator=(Color&& other) = default;

  uint8_t r{0};
  uint8_t g{0};
  uint8_t b{0};
  uint8_t a{0};
};

}  // namespace gar

#endif  // COLOR_H
