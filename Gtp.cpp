#include "Gtp.h"

#include <sstream>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;

const Gtp::vertex Gtp::pass = {-1, -1};


Gtp::Gtp(std::vector<std::shared_ptr<Player>>& players, bool debug) : game_(players, vector<double>(players.size()),
                                                                            debug) {}

void Gtp::run() {
    while (true) {
        string input;
        getline(cin, input);
		input.erase(remove_if(input.begin(), input.end(), [](char c) {
			return c == '\r';
		}), input.end());

        try {
            string output;
            bool done = process(input, output);
            cout << output << flush;
            if (done) {
                break;
            }
        } catch (const GtpException& e) {
            cout << e.getMsg();
        }
    }
}

bool Gtp::process(const string& input, string& output) {
    if (input.empty()) {
        return false;
    }

    if (input.at(0) == '#') {
        return false;
    }

    string s = input;
    std::replace(s.begin(), s.end(), '\t', ' ');
    if (s.find_first_not_of(' ') == s.npos) {
        return false;
    }

    string id = "";
    try {
        auto v = split(s);
        auto commands = list_commands();
        if (commands.find(v.at(0)) == commands.end()) {
            try {
                id = to_string(stoi(v.at(0)));
                v.erase(v.begin());
            } catch (const std::exception& e) {
                throw GtpException("unknown command");
            }
        }

        // runtime is unimportant
        const string& cmd = v.at(0);
        string args;
		if (v.size() >= 1) {
			for (size_t i = 1; i < v.size(); i++) {
				args += v[i];
				args += ' ';
			}
			if (!args.empty()) {
				args.pop_back();
			}
        }

        if (cmd == "play") {
            Move m = Move::pass(1);
            parse(args, m);
            play(m);
            output = wrap(id, response());
        } else if (cmd == "genmove") {
            color c;
            parse(args, c);
            Move m = genmove(static_cast<int>(c));
            game_.makeMove(m.getColor() - 1, m);
            output = wrap(id, response(m));
        } else if (cmd == "showboard") {
            output = wrap(id, response(showboard()));
        } else if (cmd == "protocol_version") {
            output = wrap(id, response(protocol_version()));
        } else if (cmd == "name") {
            output = wrap(id, response(name()));
        } else if (cmd == "version") {
            output = wrap(id, response(version()));
        } else if (cmd == "known_command") {
            output = wrap(id, response(known_command(args)));
        } else if (cmd == "list_commands") {
            output = wrap(id, response(list_commands()));
        } else if (cmd == "quit") {
            output = wrap(id, response());
            return true;
        } else if (cmd == "boardsize") {
            int i;
            parse(args, i);
            boardsize(i);
            output = wrap(id, response());
        } else if (cmd == "clear_board") {
            clear_board();
            output = wrap(id, response());
        } else if (cmd == "komi") {
            float f;
            parse(args, f);
            komi(f);
            output = wrap(id, response());
        } else {
            throw GtpException("unknown command");
        }
    } catch (const GtpException& e) {
        throw GtpException("?" + id + " " + e.getMsg() + "\n\n");
    } catch (const std::exception& e) {
        throw GtpException("?" + id + " " + "syntax error" + "\n\n");
    }

    return false;
}

vector<string> Gtp::split(const string& s) {
    vector<string> t;
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, ' ')) {
        if (!item.empty()) {
            t.push_back(item);
        }
    }

    return t;
}

void Gtp::parse(const string& s, int& i) {
    i = stoi(s);
}

void Gtp::parse(const string& s, bool& b) {
    b = (s == "true");
}

void Gtp::parse(const string& s, float& f) {
    f = stof(s);
}

void Gtp::parse(const string& s, Gtp::vertex& v) {
    string t;
    std::transform(s.begin(), s.end(), back_inserter(t), ::tolower);
    if (t == "pass") {
        v = pass;
        return;
    }

    v.first = tolower(t.at(0)) - 'a';
    if (v.first >= 'i' - 'a') {
        v.first--;
    }

    v.second = stoi(t.substr(1)) - 1;
}

void Gtp::parse(const string& s, Gtp::color& c) {
    char t = (char) tolower(s.at(0));
    if (t == 'b') {
        c = color::BLACK;
    } else if (t == 'w') {
        c = color::WHITE;
    }
}

void Gtp::parse(const string& s, Move& m) {
    vector<string> t = split(s);
    color c = color::BLACK;
    parse(t.at(0), c);
    vertex v;
    parse(t.at(1), v);
    if (v == pass) {
        m = Move::pass(static_cast<int>(c));
    } else {
        m = Move::move(v, static_cast<int>(c));
    }
}

int Gtp::protocol_version() const {
    return 2;
}

string Gtp::name() const {
    return "GoAI";
}

string Gtp::version() const {
    return "1.0";
}

bool Gtp::known_command(const std::string& command_name) const {
    auto known = list_commands();
    return known.find(command_name) != known.end();
}

void Gtp::boardsize(int size) {
    if (size != SIZE) {
        throw GtpException("unacceptable size");
    }
}

void Gtp::clear_board() {
    game_ = GtpGame(game_.getPlayers(), game_.getKomi(), game_.isDebug());
}

void Gtp::komi(float new_komi) {
    game_.setKomi(1, new_komi);
}

void Gtp::play(const Move& m) {
    bool legal = game_.makeMove(m.getColor() - 1, m);
    if (!legal) {
        throw GtpException("illegal move");
    }
}

Move Gtp::genmove(int color) {
    return game_.move(color - 1);
}

string Gtp::showboard() {
    stringstream ss;
    ss << '\n';
    ss << game_.getBoard();
    string s = ss.str();
    s.pop_back();
    return s;
}

string Gtp::response() {
    return "";
}

string Gtp::wrap(const string& id, const string& resp) {
    if (resp.empty()) {
        return "=" + id + "\n\n";
    } else {
        return "=" + id + " " + resp + "\n\n";
    }
}

string Gtp::response(const set<string>& strings) {
    string resp;
    for (auto& s : strings) {
        resp += s;
        resp += "\n";
    }
	if (!resp.empty()) {
		resp.pop_back();
	}
    return resp;
}

std::string Gtp::response(bool b) {
    return b ? "true" : "false";
}

string Gtp::response(int i) {
    return to_string(i);
}

std::string Gtp::response(const std::string& s) {
    return s;
}

std::string Gtp::response(const color& c) {
    if (c == color::BLACK) {
        return "b";
    } else {
        return "w";
    }
}

std::string Gtp::response(const vertex& v) {
    char c = (char) v.first + 'a';
    if (c >= 'i') {
        c++;
    }

    return string(1, c) + to_string(v.second + 1);
}

std::string Gtp::response(const Move& m) {
    if (m.isPass()) {
        return "pass";
    } else if (m.isResign()) {
        return "resign";
    } else {
        return response(m.getCoor());
    }
}


