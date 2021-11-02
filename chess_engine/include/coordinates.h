#ifndef COORDINATES_H
#define COORDINATES_H

#include "chess_types.h"
#include "my_assert.h"

class Coordinates
{
public:
  static std::optional<Coordinates> from_str(std::string_view str);

  constexpr Coordinates(int32_t x, int32_t y)
  {
    MY_ASSERT(0 <= x && x < 8, "A board has coordinates 0-7");
    MY_ASSERT(0 <= y && y < 8, "A board has coordinates 0-7");
    static_assert(sizeof(Coordinates) == 1);

    m_coords = static_cast<uint8_t>(x);
    m_coords |= static_cast<uint8_t>(y) << 4;
  }

  constexpr Coordinates(int32_t square_index)
  {
    MY_ASSERT(0 < square_index && square_index < c_board_dimension * c_board_dimension, "A board has 64 squares");

    auto x = static_cast<uint8_t>(square_index % 8);
    auto y = static_cast<uint8_t>(square_index / 8);

    m_coords = static_cast<uint8_t>(x);
    m_coords |= static_cast<uint8_t>(y) << 4;
  }

  constexpr Coordinates(Coordinates const& other) = default;

  constexpr Coordinates& operator=(Coordinates const& other) = default;

  constexpr int32_t x() const
  {
    return static_cast<int32_t>(m_coords & 0x0f);
  }

  constexpr int32_t y() const
  {
    return static_cast<int32_t>((m_coords & 0xf0) >> 4);
  }

  constexpr int32_t square_index() const
  {
    return 8 * y() + x();
  }

  constexpr auto operator<=>(Coordinates const& other) const = default;

private:
  uint8_t m_coords{0};
};

std::ostream& operator<<(std::ostream& os, Coordinates const& self);

#endif
