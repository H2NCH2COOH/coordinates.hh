#if 0
g++ -Wall -std=c++17 -o /dev/null $0
exit
#endif

#include "coordinates.hh"

struct Left;
struct Right;
struct Top;
struct Bottom;

using C1 = struct Coordinate<int, CoordinateAttr::Direction<Left, Right>>;
using C2 = struct Coordinate<int, CoordinateAttr::Direction<Top, Bottom>>;

int main() {
}
