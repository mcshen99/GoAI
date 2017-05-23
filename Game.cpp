#include "Game.h"

#include <iostream>
#include <string>
#include <array>
#include <ostream>
#include <sstream>

using std::cout;
using std::endl;
using std::string;
using std::getline;
using std::cin;
using std::array;
using std::vector;
using std::shared_ptr;

Game::Game(vector<shared_ptr<Player>> players, vector<double> komi) :
	players_(players), board_(), states_(), player_(0), resigned_(players.size()), numPasses_(0), numOut_(0), komi_(komi) {}

int Game::run() {
	while (true) {
		if (resigned_[player_]) {
			player_ = (player_ + 1) % players_.size();
			continue;
		}
		states_.insert({ player_, board_ });

		if (numPasses_ == players_.size() - numOut_) {
			int numAccept = 0;
			bool decline = false;
			int curPlayer = player_;
			vector<pos> locations;
			array<array<int, SIZE>, SIZE> territoryBoard;
			vector<double> scores;
			while (numAccept != players_.size() - numOut_ && !decline) {
				if (resigned_[curPlayer]) {
					continue;
				}
				cout << "Player " << curPlayer << " is marking dead..." << endl;
				cout << "Please enter your mark: " << endl;

				while (true) {
					string s;
					getline(cin, s);
					if (s == "A" || s == "a") {
						numAccept++;
						break;
					} else if (s == "D" || s == "d") {
						numPasses_ = 0;
						decline = true;
						break;
					} else {
						try {
							std::stringstream stream(s);
							int numLocs;
							stream >> numLocs;
							locations.clear();
							for (int i = 0; i < numLocs; ++i) {
								int x, y;
								stream >> x >> y;
								locations.push_back({ x, y });
							}
							numAccept++;

							territoryBoard = board_.score(locations);
							scores = komi_;
							cout << "Territory Board: " << endl;
							for (int i = 0; i < SIZE; ++i) {
								for (int j = 0; j < SIZE; ++j) {
									int c = territoryBoard[i][j];
									if (c <= 0) {
										cout << '.';
									} else if (c == 1) {
										cout << 'X';
									} else if (c == 2) {
										cout << 'O';
									} else {
										cout << c;
									}
									if (c != 0) {
										scores[c - 1]++;
									}
								}

								cout << endl;
							}
							cout << "Scores: " << endl;
							for (int i = 0; i < players_.size(); ++i) {
								cout << i << ": " << scores[i] << endl;
							}
							break;
						} catch (std::exception e) {

						}
					}
					cout << "invalid" << endl;
				}

				curPlayer = (curPlayer + 1) % players_.size();
			}

			if (!decline) {
				cout << "Everyone accepted." << endl;
				double maxScore = -1;
				int winner = -1;
				for (int i = 0; i < scores.size(); ++i) {
					if (scores[i] > maxScore) {
						maxScore = scores[i];
						winner = i;
					}
				}

				cout << "Player " << winner << " won with " << maxScore << " points";
				return winner;
			}
		}

		cout << board_ << endl;
		cout << "Player " << player_ << " is moving..." << endl;
		Move m = players_[player_]->move(board_);

		if (m.isPass()) {
			board_.move(m);
			cout << "Player " << player_ << " passed" << endl;
			numPasses_++;
			player_ = (player_ + 1) % players_.size();
			continue;
		}
		if (m.isResign()) {
			numPasses_ = 0;
			cout << "Player " << player_ << " lost" << endl;
			resigned_[player_] = true;
			numOut_++;
			int p = -1;
			int k = 0;
			for (size_t i = 0; i < players_.size(); ++i) {
				if (!resigned_[i]) {
					if (k == 0) {
						++k;
						p = i;
					} else {
						++k;
						break;
					}
				}
			}
			if (k == 1) {
				cout << "Player " << p << " won" << endl;
				return p;
			} else {
				player_ = (player_ + 1) % players_.size();
				continue;
			}
		}

		if (board_.canMove(m)) {
			Board next = board_;
			next.move(m);
			if (states_.find({ (player_ + 1) % players_.size(), next }) == states_.end()) {
				numPasses_ = 0;
				board_.move(m);
				player_ = (player_ + 1) % players_.size();
			} else {
				cout << "invalid" << endl;
			}
		} else {
			cout << "invalid" << endl;
		}
	}
}