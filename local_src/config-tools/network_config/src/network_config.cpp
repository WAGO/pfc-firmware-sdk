// SPDX-License-Identifier: GPL-2.0-or-later

#include "HandlerFactory.hpp"
#include <cstdio>
#include <exception>
#include <cstdarg>
#include <cstdio>

#include "OptionParser.hpp"
#include "BridgeConfig.hpp"

namespace po = boost::program_options;

using network_config::OptionParser;

enum error_codes {
  OK,
  SYNTAX_ERROR,
  OPERATION_ERROR
};

template <typename... Params>
static void Print(bool beQuiet, const ::std::string& format, Params... params) {
  if (beQuiet) {
    return;
  }

  printf(format.c_str(), params...);
}

int main(int argc, const char *argv[]) {
  OptionParser parser;
  bool beQuiet = false;
  int status = OK;

  try {
    parser.Parse(argc, argv);

    beQuiet = parser.IsSet("quiet");
    auto needHelp = parser.IsSet("help");

    if (needHelp) {
      parser.PrintHelp();
      return status;
    }

    auto handler = network_config::HandlerFactory::CreateHandler(parser);

    if (handler) {
      handler->Execute();
    }
  } catch (po::error &e) {
    status = SYNTAX_ERROR;
    Print(beQuiet, "Your call was syntactically incorrect: %s\nMaybe pay attention to the order of your options and parameters\n", e.what());
  } catch (::std::runtime_error &e) {
    status = OPERATION_ERROR;
    Print(beQuiet, "%s\n", e.what());
  } catch (...) {
    status = OPERATION_ERROR;
    Print(beQuiet, "The operation failed because of an unknown error, sorry\n");
  }

  return status;
}
