#include <iostream>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch_test_macros.hpp"

#include "lib.h"
#include "crc32.h"

unsigned int Factorial( unsigned int number ) {
  return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
  REQUIRE( Factorial(1) == 1 );
  REQUIRE( Factorial(2) == 2 );
  REQUIRE( Factorial(3) == 6 );
  REQUIRE( Factorial(10) == 3628800 );
}

TEST_CASE("crc32 working", "[crc32]") {
  REQUIRE( crc32buf({}) == 0 );
  REQUIRE( crc32buf({'1','2','3','4','5','6','7','8','9'}) == 0xcbf43926 );
  auto res = crc32file("imgui.ini");
  if (res) {
    std::cout << std::hex << res.value().first << std::endl;
  }
}

TEST_CASE("dedupe", "[crc32]") {
  dedupe("cels", "output");
}
