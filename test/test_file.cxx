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

#include "../src/file.cxx"


TEST_CASE("testing executables") {

  ifstream info { (path{"data"} / path{"info.test"}).c_str()};
  stringstream buf;
  buf << info.rdbuf();
  ELF64 e { path{"data"} / path{"a.out"} };

  SECTION("show") {
    REQUIRE( e.show() == buf.str());    
  }

}


TEST_CASE("testing insertion") {

  vector<char> chs;

  const auto read_file = [&chs](auto f) {
    chs.resize(f.size());
    f.gets(chs);
  };

  File res_file { path{"data"} / path{"res.test"} };
  read_file(res_file);
  decltype(chs) res;
  swap(res, chs);

  auto tmp { path{"data"} / path{"0.test"} };

  if (exists(tmp)) {
    remove(tmp);
  }

  copy(path{"data"} / path{"1.test"}, tmp);

  File to{tmp};
  File from{ path{"data"} / path{"2.test"} };

  read_file(from);

  SECTION("mirror_insert") {
    mirror_insert(to, chs, 41); //const
    read_file(to);
    REQUIRE(equal(chs.begin(), chs.end(), res.begin()));
  }

  SECTION("insert_byte_by_byte") {
    insert_byte_by_byte(to, chs, 41); //const
    read_file(to);
    REQUIRE(equal(chs.begin(), chs.end(), res.begin()));
  }

}
