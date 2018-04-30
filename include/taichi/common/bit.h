/*******************************************************************************
    Copyright (c) The Taichi Authors (2016- ). All Rights Reserved.
    The use of this software is governed by the LICENSE file.
*******************************************************************************/
#include <taichi/util.h>

TC_NAMESPACE_BEGIN

namespace bit {

TC_FORCE_INLINE constexpr bool is_power_of_two(int32 x) {
  return x != 0 && (x & (x - 1)) == 0;
}

TC_FORCE_INLINE constexpr bool is_power_of_two(uint32 x) {
  return x != 0 && (x & (x - 1)) == 0;
}

TC_FORCE_INLINE constexpr bool is_power_of_two(int64 x) {
  return x != 0 && (x & (x - 1)) == 0;
}

TC_FORCE_INLINE constexpr bool is_power_of_two(uint64 x) {
  return x != 0 && (x & (x - 1)) == 0;
}

template <int length>
struct Bits {
  static_assert(is_power_of_two(length), "length must be a power of two");
  static_assert(length == 32 || length == 64, "length must be 32/64");

  using T = std::conditional_t<length == 32, uint32, uint64>;

  T data;

  Bits() : data(0) {
  }

  // Uninitialized
  Bits(void *) {
  }

  template <int start, int bits = 1>
  static constexpr T mask() {
    return (((T)1 << bits) - 1) << start;
  }

  template <int start, int bits = 1>
  TC_FORCE_INLINE T get() const {
    return (data >> start) & (((T)1 << bits) - 1);
  }

  template <int start, int bits = 1>
  TC_FORCE_INLINE void set(T val) {
    data =
        (data & ~mask<start, bits>()) | ((val << start) & mask<start, bits>());
  }

  TC_FORCE_INLINE T operator()(T) const {
    return data;
  }

  TC_FORCE_INLINE T get() const {
    return data;
  }

  TC_FORCE_INLINE void set(const T &data) {
    this->data = data;
  }
};

template <typename T>
constexpr int bit_length() {
  return std::is_same<T, bool>() ? 1 : sizeof(T) * 8;
}

#define TC_BIT_FIELD(T, name, start)                    \
  T get_##name() const {                                \
    return (T)Base::get<start, bit::bit_length<T>()>(); \
  }                                                     \
  void set_##name(const T &val) {                       \
    Base::set<start, bit::bit_length<T>()>(val);        \
  }

template <typename T, int N>
TC_FORCE_INLINE constexpr T product(const std::array<T, N> arr) {
  T ret(1);
  for (int i = 0; i < N; i++) {
    ret *= arr[i];
  }
  return ret;
}

constexpr std::size_t least_pot_bound(std::size_t v) {
  std::size_t ret = 1;
  while (ret < v) {
    ret *= 2;
  }
  return ret;
}

TC_FORCE_INLINE uint32 pdep(uint32 value, uint32 mask) {
  return _pdep_u32(value, mask);
}

TC_FORCE_INLINE uint32 pdep(int32 value, int32 mask) {
  return pdep((uint32)value, (uint32)mask);
}

TC_FORCE_INLINE uint64 pdep(uint64 value, uint64 mask) {
  return _pdep_u64(value, mask);
}

TC_FORCE_INLINE constexpr uint32 pot_mask(int x) {
  return (1u << x) - 1;
}

TC_FORCE_INLINE constexpr uint32 log2int(uint64 value) {
  int ret = 0;
  value >>= 1;
  while (value) {
    value >>= 1;
    ret += 1;
  }
  return ret;
}

template <typename G, typename T>
struct Reinterpretor {
  union {
    G g;
    T t;
  };

  Reinterpretor(T t) : t(t) {
  }
  static_assert(sizeof(T) == sizeof(G));
};

template <typename G, typename T>
TC_FORCE_INLINE constexpr G reinterpret_bits(T t) {
  return Reinterpretor<G, T>(t).g;
};

TC_FORCE_INLINE constexpr float64 compress(float32 h, float32 l) {
  uint64 data =
      ((uint64)reinterpret_bits<uint32>(h) << 32) + reinterpret_bits<uint32>(l);
  return reinterpret_bits<float64>(data);
}

TC_FORCE_INLINE constexpr std::tuple<float32, float32> extract(float64 x) {
  auto data = reinterpret_bits<uint64>(x);
  return std::make_tuple(reinterpret_bits<float32>((uint32)(data >> 32)),
                         reinterpret_bits<float32>((uint32)(data & (-1))));
}

}  // namespace bit

TC_NAMESPACE_END
