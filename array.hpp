#ifndef ARRAY_HPP_
#define ARRAY_HPP_

#include <iostream>
#include <memory>
#include <stdexcept>


namespace ycstl {

template<class T, std::size_t N> 
class array {
public:
    //类型
    using value_type             = T;
    using pointer                = T*;
    using const_pointer          = const T*;
    using reference              = T&;
    using const_reference        = const T&;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using iterator               = T*;
    using const_iterator         = const T*;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    void fill(const T& u) {
        for (size_t i = 0; i < N; ++i) {
            data_[i] = u;
        }
    }

    void swap(array<T,N>& arr) noexcept {
        for (size_t i = 0; i < N; ++i) {
            T temp = arr[i];
            arr[i] = data_[i];
            data_[i] = temp;
        }
    }

    T& operator[](std::size_t pos) {
        return data_[pos];
    } 

    const T& operator[](std::size_t pos) const {
        return data_[pos];
    }

    T& at(std::size_t pos) {
        if (pos >= N) {
            throw std::out_of_range("out of range");
        }
        return data_[pos];
    }

    const T& at(std::size_t pos) const {
        if (pos >= N) {
            throw std::out_of_range("out of range");
        }
        return data_[pos];
    }

    T& front() {
        return data_[0];
    }

    const T& front() const {
        return data_[0];
    }

    T& back() {
        return data_[N-1];
    }

    const T& back() const {
        return data_[N-1];
    }

    T* begin() noexcept {
        return &data_[0];
    }

    T* end() noexcept {
        return &data_[N];
    }

    const T* begin() const noexcept {
        return &data_[0];
    }

    const T* end() const noexcept {
        return &data_[N];
    }

    reverse_iterator rbegin() noexcept {
        if (0 == N) {
            return std::reverse_iterator<iterator>(nullptr);
        } 
        return std::reverse_iterator<iterator>(&data_[N]);
    }

    reverse_iterator rend() noexcept {
        if (0 == N) {
            return std::reverse_iterator<iterator>(nullptr);
        } 
        return std::reverse_iterator<iterator>(&data_[0]);
    }

    const_reverse_iterator rbegin() const noexcept {
        if (0 == N) {
            return std::reverse_iterator<iterator>(nullptr);
        } 
        return std::reverse_iterator<iterator>(&data_[N]);
    }

    const_reverse_iterator rend() const noexcept {
        if (0 == N) {
            return std::reverse_iterator<iterator>(nullptr);
        } 
        return std::reverse_iterator<iterator>(&data_[0]);
    }

    const_iterator cbegin() const noexcept {
        return &data_[0];
    }

    const_iterator cend() const noexcept {
        return &data_[N];
    }

    const_reverse_iterator crbegin() const noexcept {
        if (0 == N) {
            return std::reverse_iterator<iterator>(nullptr);
        } 
        return std::reverse_iterator<iterator>(&data_[N]);
    }

    const_reverse_iterator crend() const noexcept {
        if (0 == N) {
            return std::reverse_iterator<iterator>(nullptr);
        } 
        return std::reverse_iterator<iterator>(&data_[0]);
    }

    std::size_t size() {
        return N;
    }

    bool empty() const noexcept {
        return N == 0;
    }

    T* data() noexcept {
        if (0 == N) {
            return nullptr;
        }
        return &data_[0];
    }

    const T* data() const noexcept {
        if (0 == N) {
            return nullptr;
        }
        return &data_[0];
    }

    T data_[N];
};

template<typename T, std::size_t N>
std::ostream& operator<<(std::ostream& os, const array<T,N>& arr) {
    os << "{";
    for (std::size_t i = 0; i != N; ++i) {
        if (i != N-1) {
            os << arr[i] << ", ";
        } else {
            os << arr[i] << "}";
        }
    }
    return os;
}

}

#endif