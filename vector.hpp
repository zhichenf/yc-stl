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
 
    vector() noexcept {
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }
    
    vector(std::size_t n, const Allocator& alloc = Allocator()) : alloc_(std::move(alloc)) {
        data_ = alloc_.allocate(n);
        size_ = capacity_ = n;
        for(size_t i = 0; i != n; i++) {
            std::construct_at(&data_[i]);   //c++20
        }
    }   

    vector(std::size_t n, const T& value, const Allocator& alloc = Allocator()) : alloc_(std::move(alloc)) {
        data_ = alloc_.allocate(n);
        size_ = capacity_ = n;
        for (size_t i = 0; i != n; i++) {
            //new(&data_[i]) T(value);
            std::construct_at(&data_[i],value); //c++20
        }
    }

    ~vector(){
        for (size_t i = 0; i != n; i++) {
            std::destroy_at(&data_[i]);         //c++20
        }
        alloc_.deallocate(data_, capacity_);
    }



    //测试输出
    void test_print() {
        for (int i = 0; i < size_; i++) {
            std::cout << data_[i] << " " << std::endl;
        }
    }
    
private:
    T* data_;
    std::size_t size_;
    std::size_t capacity_;
    Allocator alloc_;

};

}

#endif