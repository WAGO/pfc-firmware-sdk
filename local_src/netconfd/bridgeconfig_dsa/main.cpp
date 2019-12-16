// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     main.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include <string>
#include <vector>
#include <cstdint>
#include <utility>
#include <sstream>
#include <iostream>
#include <exception>
#include <algorithm>

#include <gsl.hpp>

#include "DSAJsonConverter.hpp"
#include "TypesHelper.hpp"
#include "Types.hpp"


int main(int argc, char **argv) {

  netconfd::DSAJsonConverter dsa_json_converter;

  try {
    auto args = gsl::make_span(argv, static_cast<uint>(argc));

    if (argc != 3) {
      std::cerr << "Wrong number of arguments! (";
      for(auto arg: args) {
        ::std::cerr << arg << " ";
      }
      std::cerr << ")" << std::endl;
      exit(1);
    }

    auto arg_operation = std::string { args[1] };
    auto arg_value = std::string { args[2] };

    if (arg_operation == "-f") {

      std::uint32_t dsa_mode;
      auto iss = std::istringstream { arg_value };
      iss >> dsa_mode;

      if (!iss.good()) {

        ::std::string json;
        auto status = dsa_json_converter.DSAToJson(dsa_mode, json);
        if (status.Ok()) {
          std::cout << json;
        } else {
          std::cerr << "Wrong value! " << status.GetMessage() << std::endl;
          exit(3);
        }

      }
    } else if (arg_operation == "-t") {

      uint32_t dsa_mode;
      auto status = dsa_json_converter.JsonToDSA(arg_value, dsa_mode);

      if (status.Ok()) {
        std::cout << ::std::to_string(dsa_mode);
      } else {
        std::cerr << "Wrong value! " << status.GetMessage() << std::endl;
      }

    } else {
      std::cerr << "Wrong operation!" << std::endl;
      exit(2);
    }
  } catch (std::exception& e) {
    std::cerr << "Wrong!" << e.what() << std::endl;
    exit(42);
  }
  exit(EXIT_SUCCESS);

}

//---- End of source file ------------------------------------------------------
