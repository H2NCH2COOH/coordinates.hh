#include <type_traits>
#include <algorithm>
#include <tuple>

// General stuff
namespace {
   template<bool cond> struct Require {
      static_assert(cond);
   };

   template<auto t> struct Value {
      static const auto value = t;
   };

   template<typename T> struct Type {
      using type = T;
   };

   struct Option {
      struct None : Value<false> {};
      template<typename T> struct Some : Value<true>, Type<T> {};
   };

   template<typename... Ts> struct List {
      static const size_t size = sizeof...(Ts);
   };

   template<typename L, size_t idx> struct get;
   template<typename L, size_t idx> using get_t = typename get<L, idx>::type;
   template<size_t idx> struct get<List<>, idx> : Type<Option::None> {};
   template<typename T, typename... Ts> struct get<List<T, Ts...>, 0> : Type<Option::Some<T>> {};
   template<typename T, typename... Ts, size_t idx> struct get<List<T, Ts...>, idx> : Type<get_t<List<Ts...>, idx - 1>> {};

   template<typename T, typename L> struct append;
   template<typename T, typename L> using append_t = typename append<T, L>::type;
   template<typename T, typename... Ts> struct append<T, List<Ts...>> : Type<List<T, Ts...>> {};

   template<typename... Ts> struct same;
   template<typename... Ts> inline constexpr bool same_v = same<Ts...>::value;
   template<typename T1, typename T2> struct same<T1, T2> :
      std::is_same<typename T1::type, typename T2::type> {};
   template<typename T1, typename T2, typename... Ts> struct same<T1, T2, Ts...> :
      std::conjunction<same<T1, T2>, same<T2, Ts...>> {};

   template<typename... Ts> struct different;
   template<typename... Ts> inline constexpr bool different_v = different<Ts...>::value;
   template<typename T1, typename T2> struct different<T1, T2> :
      std::negation<std::is_same<typename T1::type, typename T2::type>> {};
   template<typename T1, typename T2, typename... Ts> struct different<T1, T2, Ts...> :
      std::conjunction<different<T1, T2>, different<T1, Ts...>, different<T2, Ts...>> {};

   template<template<typename> typename F, typename... Ts> struct first;
   template<template<typename> typename F, typename... Ts> using first_t = typename first<F, Ts...>::type;
   template<template<typename> typename F> struct first<F> : Type<Option::None> {};
   template<template<typename> typename F, typename T, typename... Ts> struct first<F, T, Ts...> :
      std::conditional<F<T>::value, Option::Some<T>, first_t<F, Ts...>> {};

   // Tests
   static_assert(Option::Some<int>::value);
   static_assert(!Option::None::value);
   static_assert(List<>::size == 0);
   static_assert(List<int>::size == 1);
   static_assert(List<int, int>::size == 2);
   static_assert(get_t<List<int>, 0>::value);
   static_assert(!get_t<List<int>, 1>::value);
   static_assert(std::is_same_v<get_t<List<int, float>, 1>::type, float>);
   static_assert(same_v<Type<int>, Type<int>>);
   static_assert(!same_v<Type<int>, Type<float>>);
   static_assert(same_v<Type<int>, Type<int>, Type<int>>);
   static_assert(!same_v<Type<int>, Type<float>, Type<int>>);
   static_assert(same_v<Type<int>, Type<int>, Type<int>, Type<int>>);
   static_assert(!same_v<Type<int>, Type<int>, Type<int>, Type<float>>);
   static_assert(different_v<Type<int>, Type<float>>);
   static_assert(!different_v<Type<int>, Type<int>>);
   static_assert(different_v<Type<int>, Type<float>, Type<bool>>);
   static_assert(!different_v<Type<int>, Type<int>, Type<bool>>);
   static_assert(different_v<Type<int>, Type<float>, Type<bool>, Type<void>>);
   static_assert(!different_v<Type<int>, Type<float>, Type<bool>, Type<int>>);
   static_assert(different_v<Type<int>, Type<float>, Type<bool>, Type<void>, Type<long>>);
   static_assert(!different_v<Type<int>, Type<float>, Type<bool>, Type<int>, Type<float>>);
   static_assert(same_v<first_t<std::is_integral, void, double, int, float, long>, Type<int>>);
   static_assert(!first_t<std::is_integral, void, float, char*>::value);
};

// Tuple Ext
namespace {
   template<typename... Ts, std::size_t... Is> constexpr std::tuple<Ts...> add_tuple(const std::tuple<Ts...>& left, const std::tuple<Ts...>& right, std::index_sequence<Is...>) {
      return std::tuple{ std::get<Is>(left) + std::get<Is>(right)... };
   }

   template<typename... Ts> constexpr std::tuple<Ts...> operator+(const std::tuple<Ts...>& left, const std::tuple<Ts...>& right) noexcept {
      return add_tuple(left, right, std::index_sequence_for<Ts...>{});
   }

   template<typename... Ts, std::size_t... Is> constexpr std::tuple<Ts...> minus_tuple(const std::tuple<Ts...>& left, const std::tuple<Ts...>& right, std::index_sequence<Is...>) {
      return std::tuple{ std::get<Is>(left) - std::get<Is>(right)... };
   }

   template<typename... Ts> constexpr std::tuple<Ts...> operator-(const std::tuple<Ts...>& left, const std::tuple<Ts...>& right) noexcept {
      return minus_tuple(left, right, std::index_sequence_for<Ts...>{});
   }

   template<typename T, typename... Ts, std::size_t... Is> constexpr std::tuple<Ts...> mult_tuple_by(const std::tuple<Ts...>& left, const T& fac, std::index_sequence<Is...>) {
      return std::tuple{ std::get<Is>(left) * fac... };
   }

   template<typename T, typename... Ts> constexpr std::tuple<Ts...> operator*(const std::tuple<Ts...>& t, const T& fac) noexcept {
      return mult_tuple_by(t, fac, std::index_sequence_for<Ts...>{});
   }

   template<typename T, typename... Ts, std::size_t... Is> constexpr std::tuple<Ts...> div_tuple_by(const std::tuple<Ts...>& left, const T& fac, std::index_sequence<Is...>) {
      return std::tuple{ std::get<Is>(left) / fac... };
   }

   template<typename T, typename... Ts> constexpr std::tuple<Ts...> operator/(const std::tuple<Ts...>& t, const T& fac) noexcept {
      return div_tuple_by(t, fac, std::index_sequence_for<Ts...>{});
   }
}

// Attribute
namespace {
   template<typename Slot> struct Attr {};

   template<typename Slot, typename... As> struct get_attr;
   template<typename Slot, typename... As> using get_attr_t = typename get_attr<Slot, As...>::type;
   template<typename Slot> struct get_attr<Slot> : Type<List<>> {};
   template<typename Slot, typename A, typename... As> struct get_attr<Slot, A, As...> :
      Type<
         std::conditional_t<
            std::is_base_of_v<Attr<Slot>, A>,
            append_t<A, get_attr_t<Slot, As...>>,
            get_attr_t<Slot, As...>>> {};

   template<typename Slot, typename... As> struct optional_attr :
      Type<get_t<get_attr_t<Slot, As...>, 0>>,
      Value<get_t<get_attr_t<Slot, As...>, 0>::value>,
      Require<get_attr_t<Slot, As...>::size <= 1> {};
   template<typename Slot, typename... As> using optional_attr_t = typename optional_attr<Slot, As...>::type;

   // Tests
   static_assert(std::is_same_v<get_attr_t<int, Attr<void>>, List<>>);
   static_assert(std::is_same_v<get_attr_t<int, Attr<int>>, List<Attr<int>>>);
   static_assert(std::is_same_v<get_attr_t<int, Attr<int>, Attr<void>, Attr<int>>, List<Attr<int>, Attr<int>>>);
   static_assert(get_attr_t<int, Attr<int>, Attr<void>, Attr<int>>::size == 2);
}

namespace {
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

   template<typename N> struct Name : Attr<AttrSlot::Name> {
      using name = N;
   };
}

template<typename V, typename... Attrs> struct Coordinate {
   static_assert(std::is_integral_v<V> && std::is_signed_v<V>,
                 "Coordinate must be based on a signed integral type");
   using ValueType = V;

   using Direction = optional_attr_t<AttrSlot::Direction, Attrs...>;
   using Name = optional_attr_t<AttrSlot::Name, Attrs...>;

   V value;

   template<typename D> struct _PH_test_dir : Value<std::is_same_v<D, typename Direction::type::from> || std::is_same_v<D, typename Direction::type::to>> {};

   constexpr Coordinate() noexcept : value(0) {}
   constexpr Coordinate(const V v) noexcept : value(v) {}

   template<typename D> static constexpr std::enable_if_t<
      std::conjunction_v<
         Direction,
         _PH_test_dir<D>>,
   Coordinate> to(const V v) noexcept {
      return Coordinate(std::is_same_v<D, typename Direction::type::to>? v : -v);
   }

   constexpr Coordinate operator-() const noexcept {
      return Coordinate(-value);
   }

   constexpr Coordinate operator+(const V v) const noexcept {
      return Coordinate(value + v);
   }

   constexpr Coordinate operator-(const V v) const noexcept {
      return Coordinate(value - v);
   }

   constexpr Coordinate operator*(const double factor) const noexcept {
      return Coordinate(static_cast<V>(value * factor));
   }

   constexpr Coordinate operator/(const double factor) const noexcept {
      return Coordinate(static_cast<V>(value / factor));
   }

   constexpr Coordinate operator+(const Coordinate& that) const noexcept {
      return Coordinate(value + that.value);
   }

   constexpr Coordinate operator-(const Coordinate& that) const noexcept {
      return Coordinate(value - that.value);
   }

   constexpr bool operator==(const Coordinate& that) const noexcept {
      return value == that.value;
   }

   constexpr bool operator!=(const Coordinate& that) const noexcept {
      return value != that.value;
   }

   template<typename D> constexpr std::enable_if_t<
      std::conjunction_v<
         Direction,
         _PH_test_dir<D>>,
   bool> of(const Coordinate& that) const noexcept {
      if constexpr (std::is_same_v<D, typename Direction::type::to>) {
         return value > that.value;
      } else {
         return value < that.value;
      }
   }

   template<typename D> constexpr std::enable_if_t<
      std::conjunction_v<
         Direction,
         _PH_test_dir<D>>,
   Coordinate> go(const V v) const noexcept {
      if constexpr (std::is_same_v<D, typename Direction::type::to>) {
         return value + v;
      } else {
         return value - v;
      }
   }
};

template<typename V, typename... Attrs> constexpr Coordinate<V, Attrs...> operator*(const double factor, const Coordinate<V, Attrs...>& coor) noexcept {
   return coor * factor;
}

template<typename... Cs> struct Vec {
   std::tuple<Cs...> s;

   template<typename T> struct _PH_access_name : Type<typename T::type::name> {};

   static_assert(std::disjunction_v<
      std::conjunction<
         std::conjunction<typename Cs::Name...>,
         different<_PH_access_name<typename Cs::Name>...>>,
      std::conjunction<std::negation<typename Cs::Name>...>>,
      "Coordinates must all have different names or all have no name");

   constexpr Vec() noexcept : s() {}
   constexpr Vec(const std::tuple<Cs...> t) noexcept : s(t) {}
   constexpr Vec(Cs... args) noexcept : s(std::make_tuple(args...)) {}

   template<typename Name> struct _PH_filter_name {
      template<typename T> struct then : Value<std::is_same_v<Name, typename T::Name::type::name>> {};
   };

   template<typename Name> constexpr auto& operator[](const Name&) const noexcept {
      return std::get<typename first_t<_PH_filter_name<Name>::template then, Cs...>::type>(s);
   }

   template<size_t idx> constexpr auto& get() const noexcept {
      return std::get<idx>(s);
   }

   constexpr Vec operator-() const noexcept {
      return Vec() - *this;
   }

   constexpr bool operator==(const Vec& that) const noexcept {
      return s == that.s;
   }

   constexpr bool operator!=(const Vec& that) const noexcept {
      return s != that.s;
   }

   constexpr Vec operator+(const Vec& that) const noexcept {
      return Vec(s + that.s);
   }

   constexpr Vec operator-(const Vec& that) const noexcept {
      return Vec(s - that.s);
   }

   constexpr Vec operator*(const double factor) const noexcept {
      return Vec(s * factor);
   }

   constexpr Vec operator/(const double factor) const noexcept {
      return Vec(s / factor);
   }
};

template<typename... Cs> constexpr Vec<Cs...> operator*(const double factor, const Vec<Cs...>& vec) noexcept {
   return vec * factor;
}

template<typename O, typename... Cs> struct Point {
   Vec<Cs...> vec;

   using Origin = O;

   constexpr Point() noexcept : vec() {}
   constexpr Point(Vec<Cs...> vec) noexcept : vec(vec) {}
   constexpr Point(Cs... args) noexcept : vec(args...) {}

   template<typename Name> constexpr auto& operator[](const Name& name) const noexcept {
      return vec[name];
   }

   template<size_t idx> constexpr auto& get() const noexcept {
      return vec.Vec<Cs...>::template get<idx>();
   }

   constexpr Point operator+(const Vec<Cs...>& v) const noexcept {
      return Point(vec + v);
   }

   constexpr Point operator-(const Vec<Cs...>& v) const noexcept {
      return Point(vec - v);
   }

   constexpr Point go(const Vec<Cs...>& v) const noexcept {
      return *this + v;
   }

   constexpr Vec<Cs...> operator-(const Point& other) const noexcept {
      return vec - other.vec;
   }

   constexpr Vec<Cs...> to(const Point& other) const noexcept {
      return *this - other;
   }

   constexpr bool operator==(const Point& other) const noexcept {
      return vec == other.vec;
   }

   constexpr bool operator!=(const Point& other) const noexcept {
      return vec != other.vec;
   }
};

template<typename O, typename... Cs> constexpr Point<O, Cs...> operator+(const Vec<Cs...>& vec, const Point<O, Cs...>& point) noexcept {
   return point + vec;
}

template<typename O, typename... Cs> struct ContinuousSet {
   Point<O, Cs...> base;
   std::tuple<typename Cs::ValueType...> cnts;

   static constexpr bool _PH_tuple_any_zero(const std::tuple<typename Cs::ValueType...>& t) noexcept {
      return std::apply([](const typename Cs::ValueType&... is) -> bool {
         return ((is == 0) || ...);
      }, t);
   }

   struct NegativeCountError {};
   static constexpr std::tuple<typename Cs::ValueType...> _PH_tuple_no_neg(const std::tuple<typename Cs::ValueType...>& t) {
      std::apply([](const typename Cs::ValueType&... is) {
         if (((is < 0) || ...)) {
            throw NegativeCountError();
         }
      }, t);
      return t;
   }

   constexpr ContinuousSet() noexcept {}
   constexpr ContinuousSet(const Point<O, Cs...>& b, const std::tuple<typename Cs::ValueType...>& cnts) :
      base(_PH_tuple_any_zero(cnts)? Point<O, Cs...>() : b),
      cnts(_PH_tuple_any_zero(_PH_tuple_no_neg(cnts))? std::tuple<typename Cs::ValueType...>() : cnts) {}
   constexpr ContinuousSet(const Point<O, Cs...>& b, const typename Cs::ValueType&... cnts) : ContinuousSet(b, std::make_tuple(cnts...)) {}
   constexpr ContinuousSet(const Cs&... baseCs, const typename Cs::ValueType&... cnts) : ContinuousSet(Point<O, Cs...>(baseCs...), cnts...) {}

   constexpr bool operator==(const ContinuousSet& other) const noexcept {
      return base == other.base && cnts == other.cnts;
   }

   constexpr bool operator!=(const ContinuousSet& other) const noexcept {
      return base != other.base || cnts == other.cnts;
   }

   constexpr bool empty() const noexcept {
      return std::get<0>(cnts) == 0;
   }

   constexpr ContinuousSet operator+(const Vec<Cs...>& vec) const noexcept {
      if (empty()) {
         return ContinuousSet();
      } else {
         return ContinuousSet(base + vec, cnts);
      }
   }

   constexpr ContinuousSet operator-(const Vec<Cs...>& vec) const noexcept {
      if (empty()) {
         return ContinuousSet();
      } else {
         return ContinuousSet(base - vec, cnts);
      }
   }
};
