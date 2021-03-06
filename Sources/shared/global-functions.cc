// This file is a part of FireScript.
// Copyright (c) 2021, Ink. All rights reserved.

#include "global-functions.hpp"
#include "fire-error.hpp"

#include "../../JSON/json.hpp"

void global::Log(std::ostream& s, const std::string& content, int level, bool endl, bool UI) {
  if (DEBUGGING || UI) {
    if (level == 0) SetColor(8);
    else if (level == 1) SetColor(YELLOW);
    else SetColor(RED);

    s << content;
    if (endl)
      s << std::endl;

    SetColor(7);
  } else return;
}

void global::ProcessParams(int argc, char** argv) {
  std::vector commands = {"-d", "-v", "-?", "-h"};
  std::vector command_full_name = {"--debug", "--version", "--help"};

  auto LinearFind = [commands, command_full_name](const char* key, int type) -> bool {
    if (type == 1)
      for (auto command : commands) {
        if (!strcmp(command, key))
          return true;
      }

    else
      for (auto i : command_full_name)
        if (!strcmp(i, key))
          return true;

    return false;
  };

  const char* usage = "Usage: fire [OPTIONS] INPUT\n"
  "\n"
  "Options:\n"
  "   -d, --debug        Use debug module (not debugger)\n"
  "   -v, --version      Look up the version of fire\n"
  "   -? | -h, --help    Display this message\n";
  std::string version = "FireLang Core 0.0.1 alpha [build " + std::string(BUILD_VER) + "]";

  using std::cout;

  if (argc == 1) {
    cout << usage;
    exit(0);
  } else {
    for (int i = 1; i < argc; ++i) {
      if (argv[i][0] == '-' && argv[i][1] != '-') {
        bool flag = LinearFind(argv[i], 1);

        if (!flag) {
          Log(cout, "Incorrect option: \'" + std::string(argv[i]) +"\'\n\n"
              ,2, true);
          cout << usage; exit(-1);
        }

        std::string t = argv[i];

        if (t == "-h" || t == "-?") {
          cout << usage; exit(0);
        }
        else if (t == "-d") DEBUGGING = true;
        else {
          cout << version; exit(0);
        }
      } else if (argv[i][0] == '-' && argv[i][1] == '-') {
        bool flag = LinearFind(argv[i], 2);

        if (!flag) {
          Log(cout, "Incorrect option: \'" + std::string(argv[i]) +"\'\n\n"
              , 2, true);
          cout << usage; exit(-1);
        }

        std::string t = argv[i];

        if (t == "--help") {
          cout << usage; exit(0);
        }
        else if (t == "--debug") DEBUGGING = true;
        else {
          cout << version; exit(0);
        }
      } else {

        using flexer::Lexer;
        using flexer::Token;

        Lexer lexer(argv[i]);

        if (DEBUGGING) {
          Token tok;
          std::vector<Token> group;

          try {
            while (tok.property != flexer::EOF_) {
              tok = lexer.Automata();
              group.push_back(tok);
            }
          } catch (FireError& e) {
            Log(std::cerr, e.what(), 2, true, true);
            exit(-1);
          }

          nlohmann::json json;

          for (const Token& t : group) {
            json.push_back(t.ToJSON());
          }

          Log(std::cout, json.dump(2));
        }
      }
    }
  }
}