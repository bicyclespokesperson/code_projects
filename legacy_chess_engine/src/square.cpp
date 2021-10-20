#include "square.h"
#include "board.h"
#include "king.h"
#include "piece.h"

Square::Square(int x, int y) : _x(x), _y(y), _occupier(nullptr)
{
}

Square::Square(int x, int y, Piece* occupier) : _x(x), _y(y), _occupier(occupier)
{
}

Square::~Square() = default;

int Square::get_x() const
{
  return _x;
}

int Square::get_y() const
{
  return _y;
}

bool Square::occupied() const
{
  return (_occupier != nullptr);
}

Piece& Square::occupied_by() const
{
  return *_occupier;
}

void Square::set_occupier(Piece& piece)
{
  _occupier = &piece;
}

void Square::remove_occupier()
{
  _occupier = nullptr;
}

void Square::display(std::ostream& out) const
{
  if (occupied())
  {
    _occupier->display(out);
  }
  else
  {
    out << "___ ";
  }
}

// Print the Square's location to the console.
// Useful for debugging purposes.
std::ostream& operator<<(std::ostream& out, const Square s)
{
  // Add 65 to _x because it represents a letter but we store it as a
  // number so we can do array access with it. ('A' == 65)
  out << "Column: " << static_cast<char>(s._x + 65) << ", Row: " << s._y << std::endl;
  return out;
}
