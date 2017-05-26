#include "Player.h"

std::ostream &operator<<(std::ostream &s, const Player &p) {
  return p.comment(s);
}