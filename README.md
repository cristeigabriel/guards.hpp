# guards.hpp
Modern C++ mutex/shared_mutex guard for T, similar to Rust.

# Mutex
Simple exclusive-lock mutex.

# Shared Mutex
Read/Write (Shared/Exclusive) mutex.

# Example
```cpp
int main() {
  guards::shared_mutex guarded_data(std::vector<int>{});

  std::thread([&] {
    while (true) {
      try {
        auto r = guarded_data.read();
        if (!r->empty())
          std::cout << r->back() << ' ' << r->size() << std::endl;
      } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        break;
      }

      usleep(500);
    }
  }).detach();

  std::thread([&] {
    while (true) {
      try {
        auto w = guarded_data.write();
        if (w->empty())
          w->push_back(1);
        else w->push_back(w->back() + 1);
      } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        break;
      }

      usleep(1000);
    }
  }).detach();

  // spin as threads are running
  while (true) {
    usleep(100000);
  }
}
```

# Tested on
- macOS 13.3.1 22E261 x86_64

# License
[The Unlicense](https://github.com/cristeigabriel/guards.hpp/blob/main/LICENSE)
