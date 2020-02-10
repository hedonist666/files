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

TEST_CASE("testing executables") {

  ifstream info { (path{"data"} / path{"info.test"}).c_str()};
  stringstream buf;
  buf << info.rdbuf();
  ELF64 e { path{"data"} / path{"a.out"} };

  SECTION("show") {
    REQUIRE( e.show() == buf.str());    
  }

}
