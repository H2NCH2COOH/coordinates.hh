#if 0
c++ -Wall -O2 -std=c++17 -S $0
exit
#endif

#include <stdlib.h>
#include "coordinates.hh"

struct Left;
struct Right;
struct Top;
struct Bottom;

struct {} X;
struct {} Y;

using C1 = struct Coordinate<int, CoordinateAttr::Direction<Left, Right>, CoordinateAttr::Name<decltype(X)>>;
using C2 = struct Coordinate<int, CoordinateAttr::Direction<Top, Bottom>, CoordinateAttr::Name<decltype(Y)>>;

using V1 = struct Vec<C1, C2>;

int main(int argc, char** argv) {
   V1 v1(atoi(argv[1]), atoi(argv[2]));

   v1 = v1 + V1(2, 1);

   return v1[X].value;
}
