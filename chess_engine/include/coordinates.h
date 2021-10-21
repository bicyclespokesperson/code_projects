#ifndef COORDINATES_H
#define COORDINATES_H

#include <my_assert.h>

class Coordinates
{
public:
  Coordinates(uint8_t x, uint8_t y)
  {
    MY_ASSERT(x < 8, "A board has coordinates 0-7");
    MY_ASSERT(y < 8, "A board has coordinates 0-7");
    static_assert(sizeof(Coordinates) == 1);

    m_coords = x;
    m_coords |= y << 4;
  }

  Coordinates(Coordinates const& other) = default;

  Coordinates& operator=(Coordinates const& other) = default;

  uint8_t x() const
  {
    return m_coords & 0x0f;
  }

  uint8_t y() const
  {
    return (m_coords & 0xf0) >> 4;
  }

  auto operator<=>(Coordinates const& other) const = default;

private:
  uint8_t m_coords{0};
};

#endif
