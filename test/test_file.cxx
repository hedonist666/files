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

auto datadir {path {"data"}};

const auto readfile = [] (auto p) {
  ifstream info { ( datadir / p ).c_str()};
  stringstream buf;
  buf << info.rdbuf();
  return buf.str();
};


TEST_CASE("testing files") {

  auto tmp { datadir / path{"0.test"} };

  if (exists(tmp)) {
    remove(tmp);
  }

  copy( datadir / path{"1.test"}, tmp);
  File to{tmp};
  auto str { readfile(path{"2.test"}) };
  auto content { readfile(path{"1.test"}) };


  size_t n = find(content.begin(), content.end(), '\n') - content.begin() + 1;
  content.insert(n, str);

  SECTION("gets") {
    vector<char> res_v;
    res_v.resize(to.size());
    to.seek(0);
    to.gets(res_v);
    string res(res_v.begin(), res_v.end());

    to.seek(0);
    string std_res {readfile(path{"0.test"})};
    REQUIRE(equal(res.begin(), res.end(), std_res.begin()));
  }

  SECTION("insertion") {
    to.insert(str.c_str(), str.size(), n);
    to.seek(0);
    string res {readfile(path{"0.test"})};
    REQUIRE(equal(res.begin(), res.end(), content.begin()));
  }

}
