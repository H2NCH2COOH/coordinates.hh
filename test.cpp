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

struct OriginA;
struct OriginB;

using PA = struct Point<OriginA, C1, C2>;
using PB = struct Point<OriginB, C1, C2>;

using RectA = struct ContinuousSet<OriginA, C1, C2>;
using RectB = struct ContinuousSet<OriginB, C1, C2>;

int main() {
   constexpr C1 c1(1);
   constexpr C2 c2;
   constexpr V1 v1(1, 2);
   constexpr PA pa(1, 2);
   constexpr RectA ra0;

   static_assert(-c1 == -1);
   static_assert(c2 != 1);
   static_assert(C1::to<Left>(1) == -1);
   static_assert(C2::to<Bottom>(1) == 1);
   static_assert(C1(1).of<Left>(2));
   static_assert(C1(2).of<Right>(1));
   static_assert(C2(0).go<Top>(1) == -1);
   static_assert(C2(0).go<Bottom>(1) == 1);
   static_assert(C1(1) * 2 == 2);
   static_assert(C1(2) / 2 == 1);
   static_assert(2 * C1(1) == 2);

   static_assert(v1[X] == 1);
   static_assert(v1[Y] == 2);
   static_assert(-v1 == V1(-1, -2));
   static_assert(v1.get<0>() == 1);
   static_assert(v1.get<1>() == 2);
   static_assert(v1[X].goes<Right>());
   static_assert(v1[Y].goes<Bottom>());
   static_assert(v1 + V1(2, 1) == V1(3, 3));
   static_assert(v1 - V1(1, 2) == V1());
   static_assert(V1(1, 2) * 2 == V1(2, 4));
   static_assert(V1(2, 2) / 2 == V1(1, 1));
   static_assert(2 * V1(1, 2) == V1(2, 4));

   static_assert(pa[X] == 1);
   static_assert(pa[Y] == 2);
   static_assert(pa.get<0>() == 1);
   static_assert(pa.get<1>() == 2);

   static_assert(pa.go(v1)[X] == 2);
   static_assert(pa.to(PA(1, 1)) == V1(0, 1));
   static_assert(PA(1, 1) - V1(0, 1) == PA(1, 0));
   static_assert(PA(1, 1) + V1(1, 2) == PA(2, 3));
   static_assert(PA(2, 2) - PA(1, 1) == V1(1, 1));
   static_assert(pa - PA(1, 1) + PA(1, 1) == pa);

   return 0;
}
