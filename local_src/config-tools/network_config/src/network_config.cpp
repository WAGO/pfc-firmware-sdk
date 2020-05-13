// SPDX-License-Identifier: GPL-2.0-or-later

#include "HandlerFactory.hpp"
#include <cstdio>
#include <exception>

#include "OptionParser.hpp"
#include "BridgeConfig.hpp"

namespace po = boost::program_options;

using network_config::OptionParser;

enum error_codes
{
  OK,
  SYNTAX_ERROR,
  OPERATION_ERROR
};

int main(int argc,
         const char *argv[])
{

  OptionParser parser;
  int status = OK;

  try
  {
    parser.Parse(argc, argv);
    auto handler = network_config::HandlerFactory::CreateHandler(parser);

    if(handler)
    {
      handler->Execute();
    }
  }
  catch (po::error &e)
  {
    printf("Failed to execute operation: %s\n", e.what());
    status = SYNTAX_ERROR;
  }
  catch (::std::runtime_error &e)
  {
    // TODO(Team PND): Expectation: Config-Tools do not print out error messages.
    printf("Failed to execute operation: %s\n", e.what());
    status = OPERATION_ERROR;
  }
  catch (...)
  {
    // TODO(Team PND): Expectation: Config-Tools do not print out error messages.
    printf("Failed to execute operation");
    status = OPERATION_ERROR;
  }

  return status;
}
