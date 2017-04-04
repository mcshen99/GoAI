#include "CommandLinePlayer.h"

#include <iostream>
#include <string>
#include <sstream>
#include <ostream>

using std::cout;
using std::endl;
using std::string;
using std::getline;
using std::cin;
using std::ostream;

CommandLinePlayer::CommandLinePlayer(int color) : color_(color) {}

Move CommandLinePlayer::move(const Board& board) {
	cout << "Please enter your move: " << endl;

	while (true) {
		string s;
		getline(cin, s);
		if (s == "P" || s == "p") {
			return Move::pass();
		} else if (s == "R" || s == "r") {
			return Move::resign();
		} else {
			try {
				std::stringstream stream(s);
				int x, y;
				stream >> x >> y;
				return Move::move({ x, y }, color_);
			} catch (std::exception e) {

			}
		}

		cout << s << " is not a valid move (P/R/[x y]):" << endl;
	}
}

ostream& CommandLinePlayer::comment(ostream& s) const {
	return s;
}