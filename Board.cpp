#include "Board.h"
#include <queue>
#include <ostream>
#include <iostream>
#include <random>

using std::queue;
using std::ostream;
using std::endl;
using std::array;
using std::vector;
using std::cout;

const int Board::dirs[2][4] = { { 1, -1, 0, 0 },{ 0, 0, 1, -1 } };
const std::array<std::array<size_t, SIZE>, SIZE> Board::TABLE = Board::getTable();

bool Board::inBounds(pos position) const {
	return position.first >= 0 && position.first < SIZE && position.second >= 0 && position.second < SIZE;
}

vector<pos> Board::getCaptured(pos position, int color) const {
	vector<pos> v;
	queue<pos> q;

	if (color == 0) {
		return v;
	}

	q.push(position);

	array<array<bool, SIZE>, SIZE> visited = { false };
	visited[position.first][position.second] = true;

	while (!q.empty()) {
		pos& next = q.front();
		q.pop();

		int a = next.first;
		int b = next.second;
		visited[a][b] = true;
		v.push_back({ a, b });

		for (int i = 0; i < 4; ++i) {
			int x = a + dirs[0][i];
			int y = b + dirs[1][i];
			if (!inBounds({ x, y }) || visited[x][y]) {
				continue;
			}

			if (board_[x][y] == 0) {
				v.clear();
				return v;
			}

			if (board_[x][y] == color) {
				q.push({ x, y });
			}
		}
	}

	return v;
}

int Board::liberties(pos p) const {
	queue<pos> q;

	int color = board_[p.first][p.second];
	if (color == 0) {
		return true;
	}

	q.push(p);

	array<array<bool, SIZE>, SIZE> visited = { false };
	visited[p.first][p.second] = true;
	int count = 0;

	while (!q.empty()) {
		pos& next = q.front();
		q.pop();

		int a = next.first;
		int b = next.second;
		visited[a][b] = true;

		for (int i = 0; i < 4; ++i) {
			int x = a + dirs[0][i];
			int y = b + dirs[1][i];
			if (!inBounds({ x, y }) || visited[x][y]) {
				continue;
			}

			if (board_[x][y] == 0) {
				visited[x][y] = true;
				count++;
			}

			if (board_[x][y] == color) {
				q.push({ x, y });
			}
		}
	}

	return count;
}

vector<Move> Board::getValidMoves(int color, std::unordered_set<size_t>& playerHistory) const {
	vector<Move> moves;
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			Move m = Move::move({ i, j }, color);
			//use canMove and then move on a copy of the board (only if there are captures) to see if same boardState, for boardstates use board instead of hash, call boardstates history, consider passing
			//boardstate should only be for the player about to move
			if (!canMove(m)) {
				continue;
			}

			int a = m.getCoor().first;
			int b = m.getCoor().second;
			board_[a][b] = m.getColor();
			
			vector<pos> allCaptures;
			for (int i = 0; i < 4; ++i) {
				int x = a + dirs[0][i];
				int y = b + dirs[1][i];
				if (!inBounds({ x, y })) {
					continue;
				}
				if (board_[x][y] == m.getColor()) {
					continue;
				}
				auto captured = getCaptured({ x, y }, board_[x][y]);
				allCaptures.insert(allCaptures.end(), captured.begin(), captured.end());
			}

			if (!allCaptures.empty()) {
				Board newBoard(*this);
				for (pos p : allCaptures) {
					newBoard.board_[p.first][p.second] = 0;
					newBoard.hash_ ^= (newBoard.board_[p.first][p.second] * TABLE[p.first][p.second]);
				}
				if (playerHistory.find(newBoard.getHash()) == playerHistory.end()) {
					moves.push_back(m);
				}
			} else {
				if (playerHistory.find(this->getHash()) == playerHistory.end()) {
					moves.push_back(m);
				}
			}
		}
	}

	moves.push_back(Move::pass(color));

	return moves;
}

bool Board::isSuicide(const Move& move) const {
	const pos& p = move.getCoor();
	int c = move.getColor();
	board_[p.first][p.second] = c;

	auto captured = getCaptured(p, c);
	if (captured.empty()) {
		board_[p.first][p.second] = 0;
		return false;
	}

	for (int i = 0; i < 4; ++i) {
		int x = p.first + dirs[0][i];
		int y = p.second + dirs[1][i];
		if (!inBounds({ x, y })) {
			continue;
		}
		if (board_[x][y] == c) {
			continue;
		}
		auto enemyCaptured = getCaptured({ x, y }, board_[x][y]);
		if (!enemyCaptured.empty()) {
			board_[p.first][p.second] = 0;
			return false;
		}
	}

	board_[p.first][p.second] = 0;
	return true;
}

const array<array<int, SIZE>, SIZE>& Board::getBoard() const {
	return board_;
}

bool Board::canMove(const Move& m) const {
	if (!inBounds(m.getCoor())) {
		return false;
	}
	if (board_[m.getCoor().first][m.getCoor().second] != 0) {
		return false;
	}

	if (isSuicide(m)) {
		return false;
	}

	return true;
}

void Board::move(const Move& m) {
	if (m.isResign()) {
		return;
	}
	if (m.isPass()) {
		return;
	}

	int a = m.getCoor().first;
	int b = m.getCoor().second;
	board_[a][b] = m.getColor();
	hash_ ^= (m.getColor() * TABLE[a][b]);

	for (int i = 0; i < 4; ++i) {
		int x = a + dirs[0][i];
		int y = b + dirs[1][i];
		if (!inBounds({ x, y })) {
			continue;
		}
		if (board_[x][y] == m.getColor()) {
			continue;
		}
		auto captured = getCaptured({ x, y }, board_[x][y]);
		for (pos& p : captured) {
			hash_ ^= (board_[p.first][p.second] * TABLE[p.first][p.second]);
			board_[p.first][p.second] = 0;
		}
	}
}

size_t Board::getHash(const Move& m) const {
	if (m.isResign()) {
		return hash_;
	}
	if (m.isPass()) {
		return hash_;
	}

	size_t hash = hash_;

	int a = m.getCoor().first;
	int b = m.getCoor().second;
	board_[a][b] = m.getColor();
	hash ^= (m.getColor() * TABLE[a][b]);

	vector<std::pair<int, pos>> allCaptures;
	for (int i = 0; i < 4; ++i) {
		int x = a + dirs[0][i];
		int y = b + dirs[1][i];
		if (!inBounds({ x, y })) {
			continue;
		}
		if (board_[x][y] == m.getColor()) {
			continue;
		}
		auto captured = getCaptured({ x, y }, board_[x][y]);
		for (pos& p : captured) {
			hash ^= (board_[p.first][p.second] * TABLE[p.first][p.second]);
			allCaptures.push_back({ board_[p.first][p.second], p });
			board_[p.first][p.second] = 0;
		}
	}

	board_[a][b] = 0;
	for (auto& capture : allCaptures) {
		board_[capture.second.first][capture.second.second] = capture.first;
	}

	return hash;
}

bool Board::operator==(const Board& b) const {
	return board_ == b.board_;
}

array<array<int, SIZE>, SIZE> Board::score(vector<pos> locations) const {
	array<array<int, SIZE>, SIZE> deadBoard = board_;

	queue<pos> q;
	array<array<bool, SIZE>, SIZE> visited = { false };
	for (pos l : locations) {
		q.push(l);
		visited[l.first][l.second] = true;

		while (!q.empty()) {
			pos& next = q.front();
			q.pop();

			int a = next.first;
			int b = next.second;
			visited[a][b] = true;
			deadBoard[a][b] = 0;

			for (int i = 0; i < 4; ++i) {
				int x = a + dirs[0][i];
				int y = b + dirs[1][i];
				if (!inBounds({ x, y }) || visited[x][y]) {
					continue;
				}

				if (board_[x][y] == 0 || board_[x][y] == board_[l.first][l.second]) {
					q.push({ x, y });
				}
			}
		}
	}

	array<array<int, SIZE>, SIZE> territoryBoard = deadBoard;

	visited = { false };
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			if (deadBoard[i][j] != 0) {
				q.push({ i, j });

				while (!q.empty()) {
					pos& next = q.front();
					q.pop();

					int a = next.first;
					int b = next.second;
					visited[a][b] = true;
					territoryBoard[a][b] = deadBoard[i][j];

					for (int d = 0; d < 4; ++d) {
						int x = a + dirs[0][d];
						int y = b + dirs[1][d];
						if (!inBounds({ x, y }) || visited[x][y]) {
							continue;
						}

						if (deadBoard[x][y] == 0) {
							if (territoryBoard[x][y] == 0) {
								q.push({ x, y });
							} else {
								territoryBoard[x][y] = -1;
							}
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			if (territoryBoard[i][j] == -1) {
				territoryBoard[i][j] = 0;
			}
		}
	}

	return territoryBoard;
}

std::array<std::array<size_t, SIZE>, SIZE> Board::getTable() {
	std::array<std::array<size_t, SIZE>, SIZE> table;
	std::default_random_engine gen;
	std::uniform_int_distribution<size_t> dist;
	for (size_t i = 0; i < SIZE; i++) {
		for (size_t j = 0; j < SIZE; j++) {
			table[i][j] = dist(gen);
		}
	}

	return table;
}

size_t Board::getHash() const {
	return hash_;
}

std::array<std::array<int, 3>, 3> Board::getPattern(pos p) const {
	array<array<int, 3>, 3> pattern;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (!inBounds(p)) {
				pattern[i + 1][j + 1] = -1;
			} else {
				pattern[i + 1][j + 1] = board_[p.first + i][p.second + j];
			}
		}
	}

	return pattern;
}

ostream& operator<<(ostream& s, const Board& b) {
	for (auto& i : b.getBoard()) {
		for (auto j : i) {
			if (j == 0) {
				s << '.';
			} else if (j == 1) {
				s << 'X';
			} else if (j == 2) {
				s << 'O';
			} else {
				s << j;
			}
		}

		s << endl;
	}

	return s;
}