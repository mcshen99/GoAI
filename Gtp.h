#pragma once

#include <utility>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <iostream>

#include "Player.h"
#include "Game.h"
#include "Board.h"

// TODO merge with Game.cpp
class GtpGame {
 private:
  std::vector<std::shared_ptr<Player>> players_;
  Board board_;
  std::unordered_set<std::pair<int, Board>> states_;
  std::vector<Move> history_;
  std::vector<bool> resigned_;
  int numPasses_;
  int numOut_;
  std::vector<double> komi_;
  bool debug_;

 public:
  GtpGame(const std::vector<std::shared_ptr<Player>>& players, const std::vector<double>& komi, bool debug) :
      players_(players),
      board_(),
      states_(),
      history_(),
      resigned_(players.size()),
      numPasses_(0),
      numOut_(0),
      komi_(komi),
      debug_(debug) {}

  GtpGame(const GtpGame& g) = default;
  GtpGame& operator=(const GtpGame& g) = default;
  ~GtpGame() = default;

  void setKomi(int player, double komi) {
    komi_.at(player) = komi;
  }

  const Board& getBoard() const {
    return board_;
  }

  const std::vector<std::shared_ptr<Player>>& getPlayers() const {
    return players_;
  }

  const std::vector<double>& getKomi() const {
    return komi_;
  }

  bool isDebug() const {
    return debug_;
  }

  int nextPlayer(int player) {
    for (int i = (player + 1) % 2; i != player; i = (i + 1) % players_.size()) {
      if (!resigned_[i]) {
        return i;
      }
    }

    return player;
  }

  int getLastPlayer() {
    if (!history_.empty()) {
      return history_.back().getColor() - 1;
    }

    return 0;
  }

  Move move(int player) {
    Move m = players_.at(player)->move(board_, history_);
    if (debug_) {
      players_.at(player)->comment(std::cerr);
    }
    return m;
  }

  bool makeMove(int player, Move m) {
    states_.insert({player, board_});

    if (m.isPass()) {
      board_.move(m);
      numPasses_++;
      history_.push_back(m);
      return true;
    }

    if (m.isResign()) {
      numPasses_ = 0;
      resigned_[player] = true;
      numOut_++;
      history_.push_back(m);
      return true;
    }

    if (board_.canMove(m)) {
      Board next = board_;
      next.move(m);
      if (states_.find({nextPlayer(player), next}) == states_.end()) {
        numPasses_ = 0;
        board_.move(m);
        history_.push_back(m);
        return true;
      }
    }

    return false;
  }
};

class GtpException : public std::runtime_error {
 public:
  GtpException(const std::string& msg) : runtime_error(msg), msg_(msg) {}

  GtpException(const std::string& msg, const std::string& what) : runtime_error(what), msg_(msg) {}

  const std::string& getMsg() const {
    return msg_;
  }

 private:
  const std::string msg_;
};

class Gtp {
 public:
  Gtp(std::vector<std::shared_ptr<Player>>& players, bool debug = false);

  bool process(const std::string& input, std::string& output);
  void run();

 private:
  GtpGame game_;

  enum class color {
    BLACK = 1,
    WHITE = 2
  };

  typedef pos vertex;
  typedef std::vector<std::pair<Move, std::string>> pspairs;

  const static vertex pass;

  std::vector<std::string> split(const std::string& s);

  void parse(const std::string& s, int& i);

  void parse(const std::string& s, bool& b);

  void parse(const std::string& s, float& f);

  void parse(const std::string& s, vertex& v);

  void parse(const std::string& s, color& c);

  void parse(const std::string& s, Move& m);

  int protocol_version() const;

  std::string name() const;

  std::string version() const;

  bool known_command(const std::string& command_name) const;

  void boardsize(int size);

  void clear_board();

  void komi(float new_komi);

  void play(const Move& m);

  Move genmove(int color);

  std::string showboard();

  pspairs move_probabilities();

  pspairs move_visits();

  pspairs move_prior_probabilities();

  pspairs move_prior_visits();

  std::vector<pos> primary_variation();

  std::set<std::string> list_commands() const {
    return {
        "protocol_version",
        "name",
        "version",
        "known_command",
        "list_commands",
        "quit",
        "boardsize",
        "clear_board",
        "komi",
        "play",
        "genmove",
        "showboard",
        "gogui-analyze_commands",
        "move_probabilities",
        "move_visits",
        "move_prior_probabilities",
        "move_prior_visits",
        "primary_variation"
    };
  };

  std::set<std::string> gogui_analyze_commands() const {
    return {
        "pspairs/Move Probabilities/move_probabilities",
        "pspairs/Move Visits/move_visits",
        "pspairs/Move Prior Visits/move_prior_visits",
        "pspairs/Move Prior Probabilities/move_prior_probabilities",
        "var/Primary Variation/primary_variation",
    };
  }

  static std::string response();

  static std::string response(const std::set<std::string>& strings);

  static std::string response(bool b);

  static std::string response(int i);

  static std::string response(const std::string& s);

  static std::string response(const color& c);

  static std::string response(const Move& m);

  static std::string wrap(const std::string& id, const std::string& resp);

  static std::string response(const vertex& v);

  static std::string response(const pspairs& p);

  static std::string response(const std::vector<pos>& p);
};
