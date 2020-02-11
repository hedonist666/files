#define CATCH_CONFIG_MAIN
#include "libs/catch.hpp"
#include <iostream>
#include <vector>
#include <utility>
#include <array>
#include <functional>
#include <algorithm>
#include <string_view>
#include <fstream>

#define TEST

#include "../src/executable.cxx"

auto datadir {path {"data"}};

const auto readfile = [] (auto p) {
  ifstream info { ( datadir / p ).c_str()};
  stringstream buf;
  buf << info.rdbuf();
  return buf.str();
};


TEST_CASE("testing executables") {

  string info { readfile(path{"info.test"}) };

  ELF64 e { path{"data"} / path{"a.out"} };

  SECTION("elf show") {
    REQUIRE( e.show() == info );    
  }

}
