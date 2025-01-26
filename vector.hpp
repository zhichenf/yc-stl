#ifndef VECTOR_HPP_
#define VECTOR_HPP_

#include <memory>
#include <iostream>
#include <stdexcept>

namespace ycstl {

template<class T, class Allocator = std::allocator<T>>
class vector{
public:
    // 类型
    using value_type             = T;
    using allocator_type         = Allocator;
    using pointer                = T*;
    using const_pointer          = const T*;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using iterator               = T*;
    using const_iterator         = const T*;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
 
    //构造
    vector() noexcept : data_(nullptr), size_(0), capacity_(0 ) {
    }
    
    vector(std::size_t n, const Allocator& alloc = Allocator()) : 
    size_(n), capacity_(n), alloc_(std::move(alloc)) {
        data_ = alloc_.allocate(capacity_);
        for(size_t i = 0; i != n; i++) {
            std::construct_at(&data_[i]);   //c++20
        }
    }   

    vector(std::size_t n, const T& value, const Allocator& alloc = Allocator()) : 
    size_(n), capacity_(n), alloc_(std::move(alloc)) {
        data_ = alloc_.allocate(capacity_);
        for (size_t i = 0; i != n; i++) {
            //new(&data_[i]) T(value);
            std::construct_at(&data_[i],value); //c++20
        }
    }

    template< class InputIt, typename = std::void_t<
        decltype(*std::declval<InputIt>()),
        decltype(++std::declval<InputIt&>())
    >>
    vector(InputIt first, InputIt last, const Allocator& alloc = Allocator()) : 
    size_(last - first), capacity_(last - first), alloc_(std::move(alloc)) {
        data_ = alloc_.allocate(capacity_);
        std::size_t pos = 0;
        for (InputIt it = first; it != last; ++it, ++pos) {
            std::construct_at(&data_[pos], *it);
        }
    }

    vector( std::initializer_list<T> init, const Allocator& alloc = Allocator()) : 
    size_(init.size()), capacity_(init.size()), alloc_(std::move(alloc)) {
        data_ = alloc_.allocate(capacity_);
        std::size_t pos = 0;
        for (auto it = init.begin(); it != init.end(); ++it, ++pos) {
            std::construct_at(&data_[pos],*it);
        }
    }

    //拷贝构造
    vector(const ycstl::vector<T>& v, const Allocator& alloc = Allocator()) : 
    size_(v.size_), capacity_(v.capacity_), alloc_(std::move(alloc)) {
        data_ = alloc_.allocate(capacity_);
        for (std::size_t i = 0; i != size_; ++i) {
            std::construct_at(&data_[i],v[i]);
        }
    }

    //移动构造
    vector(ycstl::vector<T>&& v, const Allocator& alloc = Allocator()) noexcept :
    data_(v.data_), size_(v.size_), capacity_(v.capacity_), alloc_(std::move(v.alloc_)) {
        v.data_ = nullptr;
        v.size_ = 0;
        v.capacity_ = 0;
    }

    ~vector(){
        for (size_t i = 0; i != size_; ++i) {
            std::destroy_at(&data_[i]);         //c++20
        }
        alloc_.deallocate(data_, capacity_);
    }

    vector<int>& operator=(const vector<int>& v) {
        if (data_ == v.data_) {
            return *this;
        }
        if (v.data_) {
            v.~vector();
        }
        size_ = v.size_;
        capacity_ = v.capacity_;
        data_ = alloc_.allocate(capacity_);
        std::size_t pos = 0;
        for (std::size_t i = 0; i != size_; ++i) {
            std::construct_at(&data_[i],v[i]);
        }
        return *this;
    }

    vector<int>& operator=(vector<int>&& v) {
        if (data_ == v.data_) {
            return *this;
        }
        if (v.data_) {
            v.~vector();
        }
        size_ = v.size_;
        capacity_ = v.capacity_;
        data_ = alloc_.allocate(capacity_);
        std::size_t pos = 0;
        for (std::size_t i = 0; i != size_; ++i) {
            std::construct_at(&data_[i],v[i]);
        }
        v.size_ = v.capacity_ = 0;
        v.data_ = nullptr;
        return *this;
    }

    void assign(std::size_t n, const T& value) {
        if (data_) {
            this->~vector();
        }
        size_ = capacity_ = n;
        data_ = alloc_.allocate(capacity_);
        for (std::size_t i = 0; i != size_; ++i) {
            std::construct_at(&data_[i],value);
        }
    }

    void assign(std::initializer_list<T> ilist) {
        if (data_) {
            this->~vector();
        }
        size_ = capacity_ = ilist.size();
        data_ = alloc_.allocate(capacity_);
        std::size_t pos = 0;
        for (auto it = ilist.begin(); it != ilist.end(); ++it, ++pos) {
            std::construct_at(&data_[pos],*it);
        }
    }

    template< class InputIt, typename = std::void_t<
        decltype(*std::declval<InputIt>()),
        decltype(++std::declval<InputIt&>())
    >>
    void assign(InputIt first, InputIt last) {
        if (data_) {
            this->~vector();
        }
        size_ = capacity_ = last - first;
        data_ = alloc_.allocate(capacity_);
        std::size_t pos = 0;
        for (InputIt it = first; it != last; ++it, ++pos) {
            std::construct_at(&data_[pos], *it);
        }
    }

    T& operator[](const std::size_t& pos) {
        return data_[pos];
    }

    const T& operator[](const std::size_t& pos) const {
        return data_[pos];
    }

    T& at(const std::size_t& pos) {
        if (pos >= size_) {
            throw std::out_of_range("index out of range");
        }
        return data_[pos];
    }

    const T& at(const std::size_t& pos) const {
        if (pos >= size_) {
            throw std::out_of_range("index out of range");
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
        return data_[size_ - 1];
    }

    const T& back() const {
        return data_[size_ - 1];
    }

    T* data() {
        return data_;
    }

    const T* data() const {
        return data_;
    }

    std::size_t size() const {
        return size_;
    }

    std::size_t capacity() const {
        return capacity_;
    }

    iterator begin() const {
        return &data_[0];
    }

    iterator end() const {
        return &data_[size_];
    }
    
private:
    T* data_;
    std::size_t size_;
    std::size_t capacity_;
    Allocator alloc_;
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const ycstl::vector<T>& v) {
    os << "{";
    for (auto it = v.begin(); it != v.end(); ++it) {
        os << *it;
        if (it != v.end() - 1) {
            os << ", ";

        }
    }
    os << "}";
    return os;
}

}
#endif