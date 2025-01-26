#ifndef VECTOR_HPP_
#define VECTOR_HPP_

#include <memory>

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
            for (std::size_t i = 0; i != size_; ++i) {
                std::destroy_at(&data_[i]);
            }
            alloc_.deallocate(data_,capacity_);
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
            for (std::size_t i = 0; i != size_; ++i) {
                std::destroy_at(&data_[i]);
            }
            alloc_.deallocate(data_,capacity_);
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

    T& operator[](const std::size_t& pos) {
        return data_[pos];
    }

    const T& operator[](const std::size_t& pos) const {
        return data_[pos];
    }

    std::size_t size() {
        return size_;
    }

    std::size_t capacity() {
        return capacity_;
    }

    iterator begin() {
        return &data_[0];
    }

    iterator end() {
        return &data_[size_];
    }


    //测试输出
    void test_print() {
        for (int i = 0; i < size_; i++) {
            std::cout << data_[i] << " ";
        }
        std::cout << std::endl;
    }
    
private:
    T* data_;
    std::size_t size_;
    std::size_t capacity_;
    Allocator alloc_;
};

}

#endif