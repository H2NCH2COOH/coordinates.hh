#if 0
c++ -Wall -std=c++17 -o /dev/null $0
exit
#endif

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

int main() {
   constexpr Coordinate<int> c0;
   constexpr Vec<Coordinate<int>, Coordinate<int>> v0;
   constexpr C1 c1(1);
   constexpr C2 c2(2);
   constexpr V1 v1(1, 2);

   static_assert(C1(1).of<Left>(2));
   static_assert(C1(2).of<Right>(1));
   static_assert(C2(0).go<Top>(1) == -1);
   static_assert(C2(0).go<Bottom>(1) == 1);
   static_assert(v1[X] == 1);
   static_assert(v1[Y] == 2);
   static_assert(v1.get<0>() == 1);
   static_assert(v1.get<1>() == 2);
   static_assert(v1[X].goes<Right>());
   static_assert(v1[Y].goes<Bottom>());
   static_assert(v1 + V1(2, 1) == V1(3, 3));
   static_assert(v1 - V1(1, 2) == V1());
   static_assert(V1(1, 2) * 2 == V1(2, 4));
   static_assert(V1(2, 2) / 2 == V1(1, 1));

   return 0;
}
