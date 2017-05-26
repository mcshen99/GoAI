#include "Move.h"
#include <ostream>

using std::ostream;

typedef std::pair<int, int> pos;

Move::Move(pos coordinate, int col, MoveType move) : coor_(coordinate), color_(col), move_(move) {}

pos Move::getCoor() const {
  return coor_;
}
int Move::getColor() const {
  return color_;
}

bool Move::isPass() const {
  return move_ == MoveType::PASS;
}
bool Move::isResign() const {
  return move_ == MoveType::RESIGN;
}

Move Move::move(pos coor, int color) {
  return {coor, color, MoveType::MOVE};
}
Move Move::pass(int color) {
  return {{-1, -1}, color, MoveType::PASS};
}
Move Move::resign(int color) {
  return {{-1, -1}, color, MoveType::RESIGN};
}

ostream &operator<<(ostream &s, const Move &m) {
  if (m.isResign()) {
    return s << "R";
  }

  if (m.isPass()) {
    return s << "P";
  }

  return s << m.getColor() << ": " << m.getCoor().first << ", " << m.getCoor().second;
}

bool Move::operator==(const Move &m) const {
  return move_ == m.move_ && color_ == m.getColor() && coor_ == m.getCoor();
}