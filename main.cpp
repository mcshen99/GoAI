#include <iostream>
#include <array>
#include <queue>
#include <string>
#include <vector>
#include <unordered_set>
#include <memory>
#include <sstream>

using std::cout;
using std::endl;
using std::string;
using std::cin;
using std::array;
using std::vector;
using std::queue;
using std::shared_ptr;
using std::ostream;

typedef std::pair<int, int> pos;
const int size = 9;

enum class MoveType {
    MOVE,
    PASS,
    RESIGN
};

class Move {
private:
    pos coor_;
    int color_;
    MoveType move_;

    Move(pos coordinate, int col, MoveType move) : coor_(coordinate), color_(col), move_(move) {}

public:
    pos getCoor() const {
        return coor_;
    }

    int getColor() const {
        return color_;
    }

    bool isPass() const {
        return color_ == 0;
    }

    bool isResign() const {
        return color_ == -1;
    }

    static Move move(pos coor, int color) {
        return{ coor, color, MoveType::MOVE };
    }

    static Move pass() {
        return{ { -1, -1 } , 0, MoveType::PASS };
    }

    static Move resign() {
        return{ { -1, -1 } , -1, MoveType::RESIGN };
    }
};

ostream& operator<<(ostream &s, const Move &m) {
    if (m.isResign()) {
        return s << "R";
    }

    if (m.isPass()) {
        return s << "P";
    }

    return s << m.getColor() << ": " << m.getCoor().first << ", " << m.getCoor().second;
}

//make sure moves are valid
class Board {
private:
    const int dirs[2][4] = { { 1, -1, 0, 0 },{ 0, 0, 1, -1 } };
    array<array<int, size>, size> board_;

    bool inBounds(pos position) {
        return position.first >= 0 && position.first < size && position.second >= 0 && position.second < size;
    }

    vector<pos> getCaptured(pos position, int color) {
        vector<pos> v;
        queue<pos> q;

        if (color == 0) {
            return v;
        }

        q.push(position);

        array<array<bool, size>, size> visited = { false };
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

    bool isSuicide(const Move& move) {
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

public:
    Board() : board_() {}
    const array<array<int, size>, size>& getBoard() const {
        return board_;
    }

    bool canMove(const Move& m) {
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

    void move(const Move& m) {
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

    bool operator==(const Board& b) const {
        return board_ == b.board_;
    }

    // TODO implement score(list of locations)
    // locations are all the dead things.
    // *1. Create a new multidimensional array that is a copy -- dead board
    // *2. Any dead piece marks all connected/connected through empty intersection pieces of the same color
    // as dead. // use flood fill here.
    // *3. All dead pieces are removed from the board.
    // *4. In a loop through remaining pieces, if they are colored and not seen yet, then flood fill
    // *5. Any location they can path to that is neutral is marked as theirs on a different *territory board*.
    // *6. If the location is already marked as a different color, then it is marked as a -1 instead.
    // *7. Return the territory board. (May want to change -1s to 0's, but doesnt matter).

    // anything that is dead, makes all things of the same color that are reachable through
    // a path that does not cross another color also dead

    array<array<int, size>, size> score(vector<pos> locations) {
        cout << "test" << endl;
        array<array<int, size>, size> deadBoard = board_;

        queue<pos> q;
        array<array<bool, size>, size> visited = { false };
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

        /*cout << "deadBoard" << endl;
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                int c = deadBoard[i][j];
                if (c <= 0) {
                    cout << '.';
                } else if (c == 1) {
                    cout << 'X';
                } else if (c == 2) {
                    cout << 'O';
                } else {
                    cout << c;
                }
            }
            cout << endl;
        }*/
        array<array<int, size>, size> territoryBoard = deadBoard;

        visited = { false };
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (deadBoard[i][j] != 0) {
                    cout << "visiting: " << i << " " << j << endl;
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

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (territoryBoard[i][j] == -1) {
                    territoryBoard[i][j] = 0;
                }
            }
        }

        return territoryBoard;
    }
};

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

namespace std {
    template<>
    struct hash<Board> {
        size_t operator()(const Board &board) const {
            auto& b = board.getBoard();
            int h = 0;
            for (size_t i = 0; i < b.size(); i++) {
                for (size_t j = 0; j < b[i].size(); j++) {
                    h *= 31;
                    h += b[i][j];
                }
            }

            return h;
        }
    };
}

class Player {
public:
    virtual ostream& comment(ostream& s) const = 0;
    virtual Move move(const Board& board) = 0;
};

ostream& operator<<(ostream& s, const Player& p) {
    return p.comment(s);
}

class CommandLinePlayer : public Player {
private:
    int color_;

public:
    CommandLinePlayer(int color) : color_(color) {}

    Move move(const Board& board) {
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

    ostream& comment(ostream& s) const {
        return s;
    }
};
namespace std {
    template<>
    struct hash<std::pair<int, Board>> {
        size_t operator()(const std::pair<int, Board> &state) const {
            return state.first + std::hash<Board>{}(state.second) * 31;
        }
    };
}

//game keeps track of turn and all past board states.
//game vs played game
class Game {
private:
    vector<shared_ptr<Player>> players_;
    Board board_;
    std::unordered_set<std::pair<int, Board>> states_;
    int player_;
    vector<bool> resigned_;
    int numPasses_;
    int numOut_;
    vector<double> komi_;

public:
    Game(vector<shared_ptr<Player>> players, vector<double> komi) :
            players_(players), board_(), states_(), player_(0), resigned_(players.size()), numPasses_(0), numOut_(0), komi_(komi) {}

    // TODO
    // *Accept a list of komi (compensation) values for each player in the constructor.
    // *At the end of the game, go to each player, and ask for a list of locations or "accept"
    // *If the list of locations is provided, then run scoring, print out the territory board
    // Also print out the score - this is the score from the territory board + the komi for that player
    // If a new list of locations is provided, then it replaces the old one
    // When all players have accepted, scoring phase ends, and print out winner/score/w.e
    // A player can also "decline". In this case, all the passes are reversed, and the game continues.

    int run() {
        while (true) {
            if (resigned_[player_]) {
                player_ = player_ + 1 % players_.size();
                continue;
            }
            states_.insert({ player_, board_ });

            if (numPasses_ == players_.size() - numOut_) {
                int numAccept = 0;
                bool decline = false;
                int curPlayer = player_;
                vector<pos> locations;
                array<array<int, size>, size> territoryBoard;
                vector<double> scores;
                while (numAccept != players_.size() - numOut_ && !decline) {
                    if (resigned_[curPlayer]) {
                        continue;
                    }
                    cout << "Player " << curPlayer << " is marking dead..." << endl;
                    cout << "Please enter your mark: " << endl;

                    //cout << "before the loop" << endl;
                    while (true) {
                        //cout << "in the loop" << endl;
                        string s;
                        getline(cin, s);
                        if (s == "A" || s == "a") {
                            numAccept++;
                            //cout << "in a" << endl;
                            break;
                        } else if (s == "D" || s == "d") {
                            numPasses_ = 0;
                            decline = true;
                            //cout << "in d" << endl;
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
                                //cout << "in dead: " << curPlayer << " " << numLocs << endl;

                                territoryBoard = board_.score(locations);
                                scores = komi_;
                                cout << "Territory Board: " << endl;
                                for (int i = 0; i < size; ++i) {
                                    for (int j = 0; j < size; ++j) {
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

                    //cout << "out of the loop" << endl;
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
};

int main() {
    auto p = std::make_shared<CommandLinePlayer>(1);
    auto q = std::make_shared<CommandLinePlayer>(2);
    vector<shared_ptr<Player>> players{ shared_ptr<Player>(p), shared_ptr<Player>(q) };
    vector<double> komi{ 0, 7.5 };
    Game g(players, komi);

    g.run();
}