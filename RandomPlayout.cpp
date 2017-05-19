#include "RandomPlayout.h"
#include <random>
#include <queue>
#include <iostream>

using std::uniform_real_distribution;
using std::vector;
using std::map;
using std::string;
using std::default_random_engine;
using std::array;
using std::queue;
using std::ostream;

RandomPlayout::RandomPlayout(vector<double> komi) : gen_(), dist_(0.0, 1.0), komi_(komi) {}

std::map<pos, double> RandomPlayout::gen_playout(const Board& board, int player) {
	/*if (distribution(generator) < probs["capture"]) {
		unordered_set<pos> already_suggested;
		for (pos c : heuristic_set) {
			if (b.getBoard()[c.first][c.second] != 0) {

			}
		}
	}

	if (distribution(generator) < probs["pat3"]) {
		unordered_set<pos> already_suggested;
		for (pos c : heuristic_set) {
			if (b.getBoard()[c.first][c.second] == 0 && already_suggested.find(c) == already_suggested.end()) {

			}
		}
	}*/

	map<pos, double> gens;

	array<array<bool, SIZE>, SIZE> grouped = { 0 };

	std::array<std::array<int, SIZE>, SIZE> brd = board.getBoard();

	//put all the positions into their groups, actually maybe count liberties here -> if neighbor = 0, then add it to the list if it hasn't already been visited
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			if (brd[i][j] == 0) {
				gens[{i, j}] = 1;
			}
		}
	}

//	for (int i = 0; i < SIZE; ++i) {
//		for (int j = 0; j < SIZE; ++j) {
//			vector<pos> liberties;
//
//			if (!grouped[i][j] && brd[i][j] == player) {
//				queue<pos> q;
//				array<array<bool, SIZE>, SIZE> visited = { false };
//				q.push({ i, j });
//				visited[i][j] = true;
//
//				while (!q.empty()) {
//					pos& next = q.front();
//					q.pop();
//
//					int a = next.first;
//					int b = next.second;
//					visited[a][b] = true;
//					grouped[a][b] = true;
//
//					for (int i = 0; i < 4; ++i) {
//						int x = a + dirs[0][i];
//						int y = b + dirs[1][i];
//						if (!board.inBounds({ x, y }) || visited[x][y]) {
//							continue;
//						}
//
//						if (brd[x][y] == brd[i][j]) {
//							q.push({ x, y });
//						} else if (brd[x][y] == 0) {
//							visited[x][y] = true;
//							liberties.push_back({ x, y });
//						}
//					}
//				}
//			}
//
//			if (liberties.size() <= 2) {
//				for (const pos& p : liberties) {
//					gens.erase(p);
//				}
//			}
//		}
//	}

	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			if (gens.find({ i, j }) == gens.end()) {
				continue;
			}
			Move m = Move::move({ i, j }, player);
			if (board.isSuicide(m) || (isGroup(board, m) && isAtari(board, m) && !isCapture(board, m))) {
				gens.erase({ i, j });
			}
		}
	}

	if (gens.size() != 0) {
		Board print;
		for (auto& g : gens) {
			print.move(Move::move(g.first, 3));
		}
		std::cout << print << std::endl;
		double prob = 1.0 / (gens.size());

		for (const auto& it : gens) {
			gens[it.first] = prob;
		}
	}

	return gens;
}

//implement gen_playout (filter bad moves), then in move pick a random move (if none, then pass)
Move RandomPlayout::move(const Board& board, int player) {
	//to end the game, need to check for moves that you would not make in end state
	//ideal ending state: everything has 2 eyes or seki (neither can kill the other)
	//so there is no move you can make, so all moves are bad
	//list of bad moves: suicide (illegal), next move opponent can kill (self-atari)
	map<pos, double> possibleMoves = gen_playout(board, player);
	if (possibleMoves.size() == 0) {
		return Move::pass();
	}

	//update to fit with probabilities
	
	double m = dist_(gen_);
	double sum = 0;
	for (const auto& it : possibleMoves) {
		sum += it.second;
		if (sum >= m) {
			return Move::move(it.first, player);
		}
	}

	return Move::pass();
}

int RandomPlayout::simulate(Board& board, int player) {
	//if 2 players pass, game ends
	//need scoring phase, everything should be alive (don't put in locations), don't mark dead, add komi (constructor)
	bool lastPass = false;
	bool gameOver = false;
	while (!gameOver) {
		Move m = move(board, player);
		if (m.isPass()) {
			if (lastPass) {
				gameOver = true;
				break;
			} else {
				lastPass = true;
			}
		} else {
			lastPass = false;
			board.move(m);
		}
	}
	//do score here and return higher one
	vector<pos> locations;
	array<array<int, SIZE>, SIZE> territoryBoard = board.score(locations);
	vector<double> scores = komi_;
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			int c = territoryBoard[i][j];
			if (c != 0) {
				scores[c - 1]++;
			}
		}
	}
	double maxScore = -1;
	int winner = -1;
	for (int i = 0; i < scores.size(); ++i) {
		if (scores[i] > maxScore) {
			maxScore = scores[i];
			winner = i;
		}
	}

	return winner;
}

bool RandomPlayout::isGroup(const Board &board, const Move &m) {
	const int dirs[2][4] = { { 1, -1, 0, 0 },{ 0, 0, 1, -1 } };

	pos p = m.getCoor();
	int c = m.getColor();

	bool group = false;
	for (int i = 0; i < 4; ++i) {
		int x = p.first + dirs[0][i];
		int y = p.second + dirs[1][i];
		if (board.inBounds({x, y}) && board.getBoard()[x][y] == c) {
			group = true;
			break;
		}
	}

	return group;
}

bool RandomPlayout::isAtari(const Board &board, const Move &m) {
	return Board::placeAndTest(board, { m }, [&m](const Board& b) {
		return b.liberties(m.getCoor()) == 1;
	});
}

bool RandomPlayout::isCapture(const Board &board, const Move &m) {
	return Board::placeAndTest(board, { m }, [&m](const Board& b) {
		const int dirs[2][4] = { { 1, -1, 0, 0 },{ 0, 0, 1, -1 } };

		int c = m.getCoor().first;
		int d = m.getCoor().second;
		for (int i = 0; i < 4; ++i) {
			int x = c + dirs[0][i];
			int y = d + dirs[1][i];
			if (!b.inBounds({ x, y })) {
				continue;
			}
			if (b.getBoard()[x][y] == m.getColor()) {
				continue;
			}
			auto captured = b.getCaptured({ x, y }, b.getBoard()[x][y]);
			if (!captured.empty()) {
				return true;
			}
		}

		return false;
	});
}