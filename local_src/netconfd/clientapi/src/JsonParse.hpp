// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <nlohmann/json.hpp>

namespace netconf {

using nlohmann::json;

static json JsonToNJson(::std::string const &json_str) {

  json json_object;
  try {

    class throwing_sax : public nlohmann::detail::json_sax_dom_parser<nlohmann::json> {
     public:
      explicit throwing_sax(nlohmann::json &j)
          : nlohmann::detail::json_sax_dom_parser<nlohmann::json>(j) {
      }
      ;

      bool key(json::string_t &val) {
        if (ref_stack.back()->contains(val)) {
          throw std::invalid_argument("key " + val + " was already stored");
        }
        return nlohmann::detail::json_sax_dom_parser < nlohmann::json > ::key(val);
      }
    };
    throwing_sax sax_consumer(json_object);
    json::sax_parse(json_str, &sax_consumer);

  } catch (std::exception const &e) {
    throw ::std::system_error(errno, ::std::system_category(), "Json parse error " + std::string(e.what()));
  } catch (...) {
    throw ::std::system_error(errno, ::std::system_category(), "Json parse error unexpected exception");
  }

  return json_object;

}

}
