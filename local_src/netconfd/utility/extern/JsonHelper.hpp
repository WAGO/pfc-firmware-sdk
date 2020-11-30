// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

void printTree(boost::property_tree::ptree &pt, int level = 100);

static std::string indent(int level) {

  std::string s;
  for (int i = 0; i < level; i++)
    s += "  ";
  return s;
}

void printTree(boost::property_tree::ptree &pt, int level) {

  if (pt.empty()) {
    std::cout << "\"" << pt.data() << "\"";
  }

  else {
    if (level)
      std::cout << std::endl;

    std::cout << indent(level) << "{" << std::endl;

    for (boost::property_tree::ptree::iterator pos = pt.begin();
        pos != pt.end();) {
      std::cout << indent(level + 1) << "\"" << pos->first << "\": ";

      printTree(pos->second, level + 1);
      ++pos;
      if (pos != pt.end()) {
        std::cout << ",";
      }
      std::cout << std::endl;
    }

    std::cout << indent(level) << " }";
  }
  std::cout << std::endl;
  return;
}

