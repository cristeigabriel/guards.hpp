#ifndef _GUARDS_HPP
#define _GUARDS_HPP

// cristeigabriel

#include <mutex>        // std::mutex
#include <shared_mutex> // std::shared_mutex
#include <type_traits> // std::is_trivially_move_constructible_v, std::is_trivially_move_assignable_v, std::is_trivially_copy_constructible_v, std::is_trivially_move_assignable_v
#include <utility>     // std::move, std::forward

#ifndef _GUARD_DELETE_COPY_MOVE
#define _GUARD_DELETE_COPY_MOVE(x)                                             \
  explicit x(x &) = delete;                                                    \
  explicit x(const x &) = delete;                                              \
  explicit x(x &&) = delete;                                                   \
  x &operator=(x &) = delete;                                                  \
  x &operator=(const x &) = delete;                                            \
  x &operator=(x &&) = delete;
#endif

namespace guards {
namespace detail {
using mutex_t = std::mutex;
using shared_mutex_t = std::shared_mutex;
} // namespace detail

namespace impl {
template <class T> class mutex_lock_guard {
public:
  explicit mutex_lock_guard() = delete;

  explicit mutex_lock_guard(T &ref, detail::mutex_t &mutex_ref)
      : m_ref(ref), m_mutex_ref(mutex_ref) {
    m_mutex_ref.lock();
  }

  ~mutex_lock_guard() noexcept { m_mutex_ref.unlock(); }

  _GUARD_DELETE_COPY_MOVE(mutex_lock_guard)

  auto &get() noexcept { return m_ref; }

  auto *operator->() noexcept { return &m_ref; }

  auto &operator*() noexcept { return m_ref; }

private:
  T &m_ref;
  detail::mutex_t &m_mutex_ref;
};

template <class T> class shared_mutex_read_guard {
public:
  explicit shared_mutex_read_guard() = delete;

  explicit shared_mutex_read_guard(const T &ref,
                                   detail::shared_mutex_t &mutex_ref)
      : m_ref(ref), m_mutex_ref(mutex_ref) {
    m_mutex_ref.lock_shared();
  }

  ~shared_mutex_read_guard() { m_mutex_ref.unlock_shared(); }

  _GUARD_DELETE_COPY_MOVE(shared_mutex_read_guard)

  const auto &get() const noexcept { return m_ref; }

  const auto *operator->() const noexcept { return &m_ref; }

  const auto &operator*() const noexcept { return m_ref; }

private:
  const T &m_ref;
  detail::shared_mutex_t &m_mutex_ref;
};

template <class T> class shared_mutex_write_guard {
public:
  explicit shared_mutex_write_guard() = delete;

  explicit shared_mutex_write_guard(T &ref, detail::shared_mutex_t &mutex_ref)
      : m_ref(ref), m_mutex_ref(mutex_ref) {
    m_mutex_ref.lock();
  }

  ~shared_mutex_write_guard() { m_mutex_ref.unlock(); }

  _GUARD_DELETE_COPY_MOVE(shared_mutex_write_guard)

  auto &get() noexcept { return m_ref; }

  auto *operator->() noexcept { return &m_ref; }

  auto &operator*() noexcept { return m_ref; }

private:
  T &m_ref;
  detail::shared_mutex_t &m_mutex_ref;
};
} // namespace impl

template <class T> class mutex {
public:
  explicit mutex() = delete;

  template <class U = T>
  explicit mutex(
      T &&value,
      typename std::enable_if<std::is_move_constructible_v<U>>::type * = 0)
      : m_value(std::move(value)), m_mutex() {}

  template <class U = T>
  explicit mutex(
      T &&value,
      typename std::enable_if<!std::is_move_constructible_v<U> &&
                              std::is_move_assignable_v<U>>::type * = 0)
      : m_mutex() {
    m_value = std::move(value);
  }

  template <class U = T>
  explicit mutex(
      T value,
      typename std::enable_if<!std::is_move_constructible_v<U> &&
                              std::is_move_assignable_v<U>>::type * = 0)
      : m_mutex() {
    m_value = std::move(value);
  }

  template <class U = T>
  explicit mutex(const T &value,
                 typename std::enable_if<
                     !std::is_trivially_move_constructible_v<U> &&
                     !std::is_trivially_move_assignable_v<U> &&
                     std::is_trivially_copy_constructible_v<U>>::type * = 0)
      : m_value(value), m_mutex() {}

  template <class U = T>
  explicit mutex(
      const T &value,
      typename std::enable_if<!std::is_trivially_move_constructible_v<U> &&
                              !std::is_trivially_move_assignable_v<U> &&
                              !std::is_trivially_copy_constructible_v<U> &&
                              std::is_trivially_move_assignable_v<U>>::type * =
          0)
      : m_mutex() {
    m_value = value;
  }

  template <typename... Args>
  explicit mutex(Args &&...args)
      : m_value(std::forward<decltype(args)>(args)...), m_mutex() {}

  _GUARD_DELETE_COPY_MOVE(mutex)

  auto lock() { return impl::mutex_lock_guard(m_value, m_mutex); }

private:
  T m_value;
  detail::mutex_t m_mutex;
};

template <class T> class shared_mutex {
public:
  explicit shared_mutex() = delete;

  template <class U = T>
  explicit shared_mutex(
      T &&value,
      typename std::enable_if<std::is_move_constructible_v<U>>::type * = 0)
      : m_value(std::move(value)), m_mutex() {}

  template <class U = T>
  explicit shared_mutex(
      T &&value,
      typename std::enable_if<!std::is_move_constructible_v<U> &&
                              std::is_move_assignable_v<U>>::type * = 0)
      : m_mutex() {
    m_value = std::move(value);
  }

  template <class U = T>
  explicit shared_mutex(
      T value,
      typename std::enable_if<!std::is_move_constructible_v<U> &&
                              std::is_move_assignable_v<U>>::type * = 0)
      : m_mutex() {
    m_value = std::move(value);
  }

  template <class U = T>
  explicit shared_mutex(
      const T &value,
      typename std::enable_if<!std::is_trivially_move_constructible_v<U> &&
                              !std::is_trivially_move_assignable_v<U> &&
                              std::is_trivially_copy_constructible_v<U>>::type
          * = 0)
      : m_value(value), m_mutex() {}

  template <class U = T>
  explicit shared_mutex(
      const T &value,
      typename std::enable_if<!std::is_trivially_move_constructible_v<U> &&
                              !std::is_trivially_move_assignable_v<U> &&
                              !std::is_trivially_copy_constructible_v<U> &&
                              std::is_trivially_move_assignable_v<U>>::type * =
          0)
      : m_mutex() {
    m_value = value;
  }

  template <typename... Args>
  explicit shared_mutex(Args &&...args)
      : m_value(std::forward<decltype(args)>(args)...), m_mutex() {}

  _GUARD_DELETE_COPY_MOVE(shared_mutex)

  auto read() const { return impl::shared_mutex_read_guard(m_value, m_mutex); }

  auto write() { return impl::shared_mutex_write_guard(m_value, m_mutex); }

private:
  T m_value;
  mutable detail::shared_mutex_t m_mutex;
};
} // namespace guards

#endif // _GUARDS_HPP
