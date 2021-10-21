#ifndef COORDINATES_H
#define COORDINATES_H

#include <my_assert.h>

class Coordinates
{
public:
  Coordinates(uint8_t x, uint8_t y)
    : m_x(x),
      m_y(y)
  {
    if (m_x >= 8)
    {
      std::cerr << "x = " << m_x << std::endl;
    }
    if (m_y >= 8)
    {
      std::cerr << "y = " << m_y << std::endl;
    }
    MY_ASSERT(m_x < 8, "A board has coordinates 0-7");
    MY_ASSERT(m_y < 8, "A board has coordinates 0-7");
  }

  Coordinates(Coordinates const& other) = default;

  Coordinates& operator=(Coordinates const& other) = default;

  uint8_t x()
  {
    return m_x;
  }

  uint8_t y()
  {
    return m_y;
  }

private:
  uint8_t m_x;
  uint8_t m_y;
};

#endif
