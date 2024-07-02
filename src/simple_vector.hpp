#include <cstdint>
#include <new>
#include <utility>
#include <memory>

namespace utils {

template <typename T>
struct RawArray {
    unsigned char* data = nullptr; size_t size = 0;
    RawArray() {}
    RawArray(size_t n) : size(n) {
        if (n) { data = new alignas(T) unsigned char[(n * sizeof(T))]; }
    }
    ~RawArray() { if (data) { delete data; } }
    RawArray(const RawArray&) = delete;
    RawArray& operator=(const RawArray&) = delete;
    RawArray(RawArray&& other):
        data(std::exchange(other.data, nullptr)), size(std::exchange(other.size, 0)) {}
    RawArray& operator=(RawArray&& other)
        { if (data) { delete data; } data = std::exchange(other.data, nullptr); size = std::exchange(other.size, 0); return *this; }
    
    T& operator[](size_t idx) { return *std::launder(ptr(idx)); }
    const T& operator[](size_t idx) const { return *std::launder(ptr(idx)); }
    
    template <typename... ArgTs>
    T& construct_at(size_t idx, ArgTs... args) { return *std::construct_at(ptr(idx), std::forward<ArgTs>(args)...); }
    void destroy_at(size_t idx) { std::destroy_at(std::launder(ptr(idx))); }

private:
    T* ptr() const { return reinterpret_cast<T*>(data); }
    T* ptr(size_t idx) const { return ptr() + idx; }
};

template <typename T>
class Vector {
    static constexpr size_t cap_block = 256;
    RawArray<T> _data; size_t _size;
public:
    using value_type = T;
    Vector() : _data(), _size(0) {}
    ~Vector() { clear(); }
    size_t size() { return _size; }
    T& back() { return _data[_size - 1]; }
    T& operator[](size_t idx) { return _data[idx]; }
    const T& operator[](size_t idx) const { return _data[idx]; }
    
    void preserve(size_t new_size) {
        if (new_size <= _data.size) { return; }
        size_t n_blocks = new_size ? ((new_size - 1) / cap_block + 1) : 1;
        RawArray<T> new_data(n_blocks * cap_block);
        for (size_t i = 0; i < _size; ++i) {
            new_data.construct_at(i, std::move(_data[i]));
            _data.destroy_at(i);
        }
        _data = std::move(new_data);
    }
    template <typename... ArgTs>
    T& push_back(ArgTs... args) {
        ++_size; preserve(_size);
        return _data.construct_at(_size - 1, std::forward<ArgTs>(args)...);
    }
    void pop_back() {
        _data.destroy_at(_size - 1);
        --_size; preserve(_size);
    }
    void clear() {
        for (size_t i = 0; i < _size; ++i) { std::destroy_at(&_data[i]); }
        _size = 0;
    }
};

} // namespace utils
