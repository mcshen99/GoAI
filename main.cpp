#include <memory>
#include <vector>

#include "Game.h"
#include "CommandLinePlayer.h"
#include "MonteCarloPlayer.h"
#include "Gtp.h"

using std::vector;
using std::shared_ptr;

int main(int argc, char* argv[]) {
  std::string mode = "cli";
  bool debug = false;
  for (int i = 1; i < argc; i++) {
    std::string arg(argv[i]);
    if (arg == "--mode") {
      if (i + 1 < argc) {
        mode = argv[i + 1];
      }
    }

    if (arg == "--debug") {
      debug = true;
    }
  }

  vector<double> komi{0, 7.5};

  if (mode == "cli") {
    auto p = std::make_shared<CommandLinePlayer>(1);
    auto q = std::make_shared<MonteCarloPlayer>(2, komi, 10000, 500);
    vector<shared_ptr<Player>> players{shared_ptr<Player>(p), shared_ptr<Player>(q)};
    Game g(players, komi);
    g.run();
  } else if (mode == "gtp") {
    auto r1 = std::make_shared<MonteCarloPlayer>(1, komi, 10000);
    auto r2 = std::make_shared<MonteCarloPlayer>(2, komi, 10000);
    vector<shared_ptr<Player>> gtpPlayers{shared_ptr<Player>(r1), shared_ptr<Player>(r2)};
    Gtp gtp(gtpPlayers, debug);
    gtp.run();
  } else if (mode == "prof") {
    auto p = std::make_shared<MonteCarloPlayer>(1, komi, 1000);
    Board b;
    vector<Move> history;
    p->move(b, history);
  }

  return 0;
}