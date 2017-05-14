#include "Board.h"
#include <queue>
#include <ostream>
#include <iostream>

using std::queue;
using std::ostream;
using std::endl;
using std::array;
using std::vector;
using std::cout;

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

Board::Board() : board_() {}

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
		//need to do stuff here
		return;
	}

	int a = m.getCoor().first;
	int b = m.getCoor().second;
	board_[a][b] = m.getColor();

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
			board_[p.first][p.second] = 0;
		}
	}
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