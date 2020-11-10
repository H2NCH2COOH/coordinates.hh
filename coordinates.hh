#include <type_traits>
#include <algorithm>

namespace {
   template<typename... Ts> struct List {
      static const size_t size = sizeof...(Ts);
   };

   template<typename L, size_t idx> struct get;
   template<typename L, size_t idx> using get_t = typename get<L, idx>::type;
   template<typename T, typename... Ts> struct get<List<T, Ts...>, 0> {
      using type = T;
   };
   template<typename T, typename... Ts, size_t idx> struct get<List<T, Ts...>, idx> {
      using type = get_t<List<Ts...>, idx - 1>;
   };

   template<typename T, typename L> struct append;
   template<typename T, typename L> using append_t = typename append<T, L>::type;
   template<typename T, typename... Ts> struct append<T, List<Ts...>> {
      using type = List<T, Ts...>;
   };

   template<typename Slot> struct Attr {};

   template<typename Slot, typename... As> struct get_attr;
   template<typename Slot, typename... As> using get_attr_t = typename get_attr<Slot, As...>::type;
   template<typename Slot> struct get_attr<Slot> {
      using type = List<>;
   };
   template<typename Slot, typename A, typename... As> struct get_attr<Slot, A, As...> {
      using rest = get_attr_t<Slot, As...>;
      using type = std::conditional_t<std::is_base_of_v<Attr<Slot>, A>, append_t<A, rest>, rest>;
   };

   template<typename Slot, typename... As> struct one_or_none_attr {
      static_assert(get_attr_t<Slot, As...>::size == 0 || get_attr_t<Slot, As...>::size == 1, "Can't have more than one");
      using type = get_t<get_attr_t<Slot, As...>, 0>;
   };
   template<typename Slot, typename... As> using one_or_none_attr_t = typename one_or_none_attr<Slot, As...>::type;

   [[maybe_unused]] struct {
      static_assert(List<>::size == 0);
      static_assert(List<int>::size == 1);
      static_assert(List<int, int>::size == 2);
      static_assert(std::is_same_v<get_t<List<int>, 0>, int>);
      static_assert(std::is_same_v<get_t<List<int, float>, 1>, float>);

      static_assert(std::is_same_v<get_attr_t<int, Attr<void>>, List<>>);
      static_assert(std::is_same_v<get_attr_t<int, Attr<int>>, List<Attr<int>>>);
      static_assert(std::is_same_v<get_attr_t<int, Attr<int>, Attr<void>, Attr<int>>, List<Attr<int>, Attr<int>>>);
      static_assert(get_attr_t<int, Attr<int>, Attr<void>, Attr<int>>::size == 2);
   } StaticTests;

   // Attr Slots
   namespace AttrSlot {
      struct Direction;
      struct Name;
   }
}

namespace CoordinateAttr {
   template<typename From, typename To> struct Direction : Attr<AttrSlot::Direction> {
      static_assert(!std::is_same_v<From, To>, "The two directions must be different");
      using from = From;
      using to = To;
   };

   template<const char* N> struct Name : Attr<AttrSlot::Name> {
      static constexpr char* name = N;
   };
}

template<typename V, typename... Attrs> struct Coordinate {
   static_assert(std::is_integral_v<V> && std::is_signed_v<V>,
                 "Coordinate must be based on a signed integral type");
   using ValueType = V;

   using Direction = one_or_none_attr_t<AttrSlot::Direction, Attrs...>;
   using Name = one_or_none_attr_t<AttrSlot::Name, Attrs...>;

   V value;

   constexpr Coordinate() : value(0) {};
   constexpr Coordinate(const V v) : value(v) {};

   constexpr Coordinate operator-() const {
      return Coordinate(-value);
   }

   constexpr Coordinate operator+(const V v) const {
      return Coordinate(value + v);
   }

   constexpr Coordinate operator-(const V v) const {
      return Coordinate(value - v);
   }

   constexpr Coordinate operator*(const double factor) const {
      return Coordinate(static_cast<V>(value * factor));
   }

   constexpr Coordinate operator/(const double factor) const {
      return Coordinate(static_cast<V>(value / factor));
   }

   Coordinate& operator+=(const V v) {
      value += v;
      return *this;
   }

   Coordinate& operator-=(const V v) {
      value -= v;
      return *this;
   }

   Coordinate& operator*=(const double factor) {
      value = static_cast<int>(value * factor);
      return *this;
   }

   Coordinate& operator/=(const double factor) {
      value = static_cast<int>(value / factor);
      return *this;
   }

   constexpr bool operator==(const Coordinate& that) const {
      return value == that.value;
   }

   constexpr bool operator!=(const Coordinate& that) const {
      return value != that.value;
   }
};


#if 0

   constexpr Coordinate left(int v) {
      if constexpr (GoesLeft) {
         return Coordinate(value + v);
      } else {
         return Coordinate(value - v);
      }
   }
};

   static const bool YGoesUp = std::is_base_of_v<CoordinateAttrYGoesUp, T>;
   static const bool YGoesDown = !YGoesUp;

   static const int TopMost = (YGoesUp)? INT_MAX : INT_MIN;
   static const int BottomMost = (YGoesUp)? INT_MIN : INT_MAX;

   static constexpr int TopOf(const int a, const int b) {
      if constexpr (YGoesUp) {
         return (a > b)? a : b;
      } else {
         return (a < b)? a : b;
      }
   }

   static constexpr int BottomOf(const int a, const int b) {
      if constexpr (YGoesUp) {
         return (a < b)? a : b;
      } else {
         return (a > b)? a : b;
      }
   }
};

template<typename C>  struct Vec {
   int x;
   int y;

   static constexpr Vec<C> Left(const int x) {
      if constexpr (C::XGoesLeft) {
         return Vec<C>(x, 0);
      } else {
         return Vec<C>(-x, 0);
      }
   }

   static constexpr Vec<C> Right(const int x) {
      if constexpr (C::XGoesLeft) {
         return Vec<C>(-x, 0);
      } else {
         return Vec<C>(x, 0);
      }
   }

   static constexpr Vec<C> Up(const int y) {
      if constexpr (C::YGoesUp) {
         return Vec<C>(0, y);
      } else {
         return Vec<C>(0, -y);
      }
   }

   static constexpr Vec<C> Down(const int y) {
      if constexpr (C::YGoesUp) {
         return Vec<C>(0, -y);
      } else {
         return Vec<C>(0, y);
      }
   }

   static constexpr Vec<C> LeftUp(const int x, const int y) {
      return Left(x) + Up(y);
   }

   static constexpr Vec<C> RightUp(const int x, const int y) {
      return Right(x) + Up(y);
   }

   static constexpr Vec<C> LeftDown(const int x, const int y) {
      return Left(x) + Down(y);
   }

   static constexpr Vec<C> RightDown(const int x, const int y) {
      return Right(x) + Down(y);
   }

   constexpr Vec<C>() : x(0), y(0) {};

   constexpr Vec<C>(const int x, const int y) : x(x), y(y) {};

#ifdef CUI
   Vec<C>(const cui::Point& p) : x(p.x), y(p.y) {};

   cui::Point toCUIPoint() const {
      return cui::Point(x, y);
   }
#endif

   constexpr Vec<C> operator+(const Vec<C>& that) const {
      return Vec<C>(x + that.x, y + that.y);
   }

   constexpr Vec<C> operator-(const Vec<C>& that) const {
      return Vec<C>(x - that.x, y - that.y);
   }

   constexpr Vec<C> operator-() const {
      return Vec<C>(-x, -y);
   }

   constexpr Vec<C> operator*(const double factor) const {
      return Vec<C>(static_cast<int>(x * factor),
                    static_cast<int>(y * factor));
   }

   constexpr Vec<C> operator/(const double factor) const {
      return Vec<C>(static_cast<int>(x / factor),
                    static_cast<int>(y / factor));
   }

   constexpr bool operator==(const Vec<C>& that) const {
      return x == that.x && y == that.y;
   }

   constexpr bool operator!=(const Vec<C>& that) const {
      return !(*this == that);
   }

   constexpr bool goingUp() const {
      if constexpr (C::YGoesUp) {
         return y > 0;
      } else {
         return y < 0;
      }
   }

   constexpr bool goingDown() const {
      if constexpr (C::YGoesUp) {
         return y < 0;
      } else {
         return y > 0;
      }
   }

   constexpr bool goingLeft() const {
      if constexpr (C::XGoesLeft) {
         return x > 0;
      } else {
         return x < 0;
      }
   }

   constexpr bool goingRight() const {
      if constexpr (C::XGoesLeft) {
         return x < 0;
      } else {
         return x > 0;
      }
   }
};

template<typename C> struct Rect {
   Vec<C> origin; // Origin is always the corner with the lowest coordinates.
   int width;
   int height;

   constexpr Rect<C>() : origin(0, 0), width(0), height(0) {};

   constexpr Rect<C>(Vec<C> origin, int width, int height) :
      origin(origin), width(width), height(height) {};

   constexpr Rect<C>(int x, int y, int width, int height) :
      origin(x, y), width(width), height(height) {};

#ifdef CUI
   Rect<C>(const cui::Rect& rect) :
      origin(rect.GetOrigin()), width(rect.Width()), height(rect.Height()) {};

   cui::Rect toCUIRect() const {
      return cui::Rect::FromXYWH(origin.x, origin.y, width, height);
   }
#endif

   constexpr Vec<C> farEnd() const {
      return origin + Vec<C>((width > 0)? width - 1 : 0,
                             (height > 0)? height - 1 : 0);
   }

   constexpr bool operator==(const Rect<C>& that) const {
      return origin == that.origin &&
             width == that.width &&
             height == that.height;
   }

   constexpr bool operator!=(const Rect<C>& that) const {
      return !(*this == that);
   }

   constexpr Rect<C> move(const Vec<C>& vec) const {
      return Rect<C>(origin + vec, width, height);
   }

   constexpr Rect<C> operator*(double factor) const {
      return Rect<C>(origin, static_cast<int>(width * factor),
                             static_cast<int>(height * factor));
   }

   constexpr Rect<C> operator/(double factor) const {
      return Rect<C>(origin, static_cast<int>(width / factor),
                             static_cast<int>(height / factor));
   }

   constexpr bool contains(const Vec<C>& vec) const {
      return vec.x >= origin.x &&
             vec.x <= farEnd().x &&
             vec.y >= origin.y &&
             vec.y <= farEnd().y;
   }

   constexpr Rect<C> operator&(const Rect<C>& that) const {
      Vec<C> o(std::max(origin.x, that.origin.x),
               std::max(origin.y, that.origin.y));
      Vec<C> f(std::min(farEnd().x, that.farEnd().x),
               std::min(farEnd().y, that.farEnd().y));
      if (o.x > f.x || o.y > f.y) {
         return Rect<C>();
      } else {
         return Rect<C>(o, (f - o).x + 1, (f - o).y + 1);
      }
   }

   constexpr Rect<C> operator|(const Rect<C>& that) const {
      Vec<C> o(std::min(origin.x, that.origin.x),
               std::min(origin.y, that.origin.y));
      Vec<C> f(std::max(farEnd().x, that.farEnd().x),
               std::max(farEnd().y, that.farEnd().y));
      return Rect<C>(o, (f - o).x + 1, (f - o).y + 1);
   }

   constexpr int size() const {
      return width * height;
   }

   constexpr bool empty() const {
      return width == 0 || height == 0;
   }

   constexpr int top() const {
      return C::TopOf(origin.y, farEnd().y);
   }

   constexpr int bottom() const {
      return C::BottomOf(origin.y, farEnd().y);
   }

   constexpr int left() const {
      return C::LeftOf(origin.x, farEnd().x);
   }

   constexpr int right() const {
      return C::RightOf(origin.x, farEnd().x);
   }

   constexpr Vec<C> topLeft() const {
      return Vec<C>(left(), top());
   }

   constexpr Vec<C> topRight() const {
      return Vec<C>(right(), top());
   }

   constexpr Vec<C> bottomLeft() const {
      return Vec<C>(left(), bottom());
   }

   constexpr Vec<C> bottomRight() const {
      return Vec<C>(right(), bottom());
   }
};
#endif
