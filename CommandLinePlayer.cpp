#include "CommandLinePlayer.h"

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::string;
using std::getline;
using std::cin;
using std::ostream;
using std::map;
using std::unordered_set;

CommandLinePlayer::CommandLinePlayer(int color) : color_(color) {}

Move CommandLinePlayer::move(const Board &board, const std::vector<Move> &history) {
  cout << "Please enter your move: " << endl;

  while (true) {
    string s;
    getline(cin, s);
    if (s == "P" || s == "p") {
      return Move::pass(color_);
    } else if (s == "R" || s == "r") {
      return Move::resign(color_);
    } else {
      try {
        std::stringstream stream(s);
        int x, y;
        stream >> x >> y;
        return Move::move({x, y}, color_);
      } catch (std::exception e) {

      }
    }

    cout << s << " is not a valid move (P/R/[x y]):" << endl;
  }
}

ostream &CommandLinePlayer::comment(ostream &s) const {
  return s;
}