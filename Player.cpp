#include "Player.h"

std::ostream& operator<<(std::ostream& s, const Player& p) {
  return p.comment(s);
}

std::vector<std::pair<Move, std::string>> Player::moveProbabilities() const {
  return {};
}

std::vector<std::pair<Move, std::string>> Player::moveVisits() const {
  return {};
}

std::vector<std::pair<Move, std::string>> Player::movePriorProbabilities() const {
  return {};
}


std::vector<std::pair<Move, std::string>> Player::raveMoveProbabilities() const {
  return {};
}

std::vector<std::pair<Move, std::string>> Player::raveMoveVisits() const {
  return {};
}

std::vector<pos> Player::primaryVariation() const {
  return {};
}
