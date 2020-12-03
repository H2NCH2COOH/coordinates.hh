#if 0
g++ -Wall -std=c++17 -o /dev/null $0
exit
#endif

#include "coordinates.hh"

struct Left;
struct Right;
struct Top;
struct Bottom;

struct X;
struct Y;

using C1 = struct Coordinate<int, CoordinateAttr::Direction<Left, Right>>;
using C2 = struct Coordinate<int, CoordinateAttr::Direction<Top, Bottom>>;

using V1 = struct Vec<C1, C2>;

int main() {
   C1 c1;
   C1 c2;
   V1 v1;
}
