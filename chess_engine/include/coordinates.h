#ifndef COORDINATES_H
#define COORDINATES_H

#include <my_assert.h>

class Coordinates
{
public:
  Coordinates(int8_t x, int8_t y)
  {
    MY_ASSERT(x < 8, "A board has coordinates 0-7");
    MY_ASSERT(y < 8, "A board has coordinates 0-7");
    static_assert(sizeof(Coordinates) == 1);

    m_coords = static_cast<uint8_t>(x);
    m_coords |= static_cast<uint8_t>(y) << 4;
  }

  Coordinates(Coordinates const& other) = default;

  Coordinates& operator=(Coordinates const& other) = default;

  int8_t x() const
  {
    return static_cast<int8_t>(m_coords & 0x0f);
  }

  int8_t y() const
  {
    return static_cast<int8_t>((m_coords & 0xf0) >> 4);
  }

  auto operator<=>(Coordinates const& other) const = default;

private:
  uint8_t m_coords{0};
};

#endif
