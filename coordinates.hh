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

   struct Optional {
      struct None : Value<false> {};
      template<typename T> struct Some : Value<true>, Type<T> {};
   };

   template<typename... Ts> struct List {
      static const size_t size = sizeof...(Ts);
   };

   template<typename L, size_t idx> struct get;
   template<typename L, size_t idx> using get_t = typename get<L, idx>::type;
   template<size_t idx> struct get<List<>, idx> : Type<Optional::None> {};
   template<typename T, typename... Ts> struct get<List<T, Ts...>, 0> : Type<Optional::Some<T>> {};
   template<typename T, typename... Ts, size_t idx> struct get<List<T, Ts...>, idx> : Type<get_t<List<Ts...>, idx - 1>> {};

   template<typename T, typename L> struct append;
   template<typename T, typename L> using append_t = typename append<T, L>::type;
   template<typename T, typename... Ts> struct append<T, List<Ts...>> : Type<List<T, Ts...>> {};

   // Tests
   static_assert(List<>::size == 0);
   static_assert(List<int>::size == 1);
   static_assert(List<int, int>::size == 2);
   static_assert(get_t<List<int>, 0>::value == true);
   static_assert(get_t<List<int>, 1>::value == false);
   static_assert(std::is_same_v<get_t<List<int, float>, 1>::type, float>);
};

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

   template<template<typename> typename F, typename... Ts> struct all : Value<std::conjunction_v<F<Ts>...>> {};
   template<template<typename> typename F, typename... Ts> inline constexpr bool all_v = all<F, Ts...>::value;

   template<template<typename> typename F, typename... Ts> struct any : Value<std::disjunction_v<F<Ts>...>> {};
   template<template<typename> typename F, typename... Ts> inline constexpr bool any_v = any<F, Ts...>::value;

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

   constexpr Coordinate() noexcept : value(0) {};
   constexpr Coordinate(const V v) noexcept : value(v) {};

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

   Coordinate& operator+=(const V v) noexcept {
      value += v;
      return *this;
   }

   Coordinate& operator-=(const V v) noexcept {
      value -= v;
      return *this;
   }

   Coordinate& operator*=(const double factor) noexcept {
      value = static_cast<int>(value * factor);
      return *this;
   }

   Coordinate& operator/=(const double factor) noexcept {
      value = static_cast<int>(value / factor);
      return *this;
   }

   constexpr bool operator==(const Coordinate& that) const noexcept {
      return value == that.value;
   }

   constexpr bool operator!=(const Coordinate& that) const noexcept {
      return value != that.value;
   }
};

template<typename... Cs> struct Vec {
   std::tuple<Cs...> s;

   static_assert(std::conjunction_v<typename Cs::Name...> || std::conjunction_v<std::negation<typename Cs::Name>...>, "All or none of the coordinates must have name");
};
