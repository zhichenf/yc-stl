/**
 * 实现vector
 * 
 * @author YC奕晨 
 * */ 

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

    template<class InputIt, typename = std::void_t<
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

    vector<T>& operator=(const vector<T>& v) {
        if (data_ == v.data_) {
            return *this;
        }
        if (this->data_) {
            this->~vector();
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

    vector<T>& operator=(vector<T>&& v) noexcept {
        if (data_ == v.data_) {
            return *this;
        }
        if (this->data_) {
            this->~vector();
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

    vector& operator=(std::initializer_list<value_type> ilist ){
        if (this->data_) {
            this->~vector();
        }
        size_ = capacity_ = ilist.size();
        data_ = alloc_.allocate(capacity_);
        std::size_t pos = 0;
        auto it = ilist.begin();
        for (std::size_t i = 0; i != size_; ++i, ++it) {
            std::construct_at(&data_[i],*it);
        }
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

    bool empty() const {
        return size_ == 0;
    }

    void reserve(std::size_t new_cap) {
        if (new_cap <= capacity_) {
            return;
        }
        new_cap = new_cap > capacity_ * 2 ? new_cap : capacity_ * 2;
        
        expand(new_cap);
    }

    void shrink_to_fit() {
        if (size_ == capacity_) {
            return;
        }
        T* old_data = data_;

        data_ = alloc_.allocate(size_);
        for (std::size_t i = 0; i != size_; ++i) {
            std::construct_at(&data_[i], old_data[i]);
            std::destroy_at(&old_data[i]);
        }
        alloc_.deallocate(old_data, capacity_);
        capacity_ = size_;

    }

    T* begin() noexcept {
        if (size_ == 0) {
            return nullptr;
        }
        return &data_[0];
    }

    T* end() noexcept {
        if (size_ == 0) {
            return nullptr;
        }
        return &data_[size_];
    }

    const T* begin() const noexcept {
        if (size_ == 0) {
            return nullptr;
        }
        return &data_[0];
    }

    const T* end() const noexcept {
        if (size_ == 0) {
            return nullptr;
        }
        return &data_[size_];
    }

    const T* cbegin() const noexcept {
        if (size_ == 0) {
            return nullptr;
        }
        return &data_[0];
    }

    const T* cend() const noexcept {
        if (size_ == 0) {
            return nullptr;
        }
        return std::reverse_iterator<iterator>(&data_[size_]);
    }

    reverse_iterator rbegin() noexcept {
        if (size_ == 0) {
            return std::reverse_iterator<iterator>(nullptr);
        }
        if (size_ == 0) {

        }
        return std::reverse_iterator<iterator>(&data_[size_]);
    }

    reverse_iterator rend() noexcept {
        if (size_ == 0) {
            return std::reverse_iterator<iterator>(nullptr);
        }
        return std::reverse_iterator<iterator>((&data_[0]));
    }

    const_reverse_iterator rbegin() const noexcept {
        if (size_ == 0) {
            return std::reverse_iterator<iterator>(nullptr);
        }
        if (size_ == 0) {

        }
        return std::reverse_iterator<iterator>(&data_[size_]);
    }

    const_reverse_iterator rend() const noexcept {
        if (size_ == 0) {
            return std::reverse_iterator<iterator>(nullptr);
        }
        return std::reverse_iterator<iterator>((&data_[0]));
    }

    const_reverse_iterator crbegin() const noexcept {
        if (size_ == 0) {
            return std::reverse_iterator<iterator>(nullptr);
        }
        return std::reverse_iterator<const_iterator>(&data_[size_]);
    }

    const_reverse_iterator crend() const noexcept {
        if (size_ == 0) {
            return std::reverse_iterator<iterator>(nullptr);
        }
        return std::reverse_iterator<const_iterator>((&data_[0]));
    }

    void clear() {
        for (size_t i = 0; i != size_; ++i) {
            std::destroy_at(&data_[i]);         //c++20
        }
        size_ = 0;
    }

    T* insert(const T* pos, const T& value) {
        std::size_t pos_i = pos - &data_[0];
        if (size_ == capacity_) {
            expand(2*capacity_);
        }
        if (pos_i == size_) {
            std::construct_at(&data_[size_],value);
        } else {
            std::construct_at(&data_[size_],data_[size_-1]);
            std::size_t index = size_-1;
            for( ; index != pos_i; --index) {
                data_[index] = data_[index-1];
            }
            data_[index] = value;
        }
        size_++;
        return &data_[pos_i];
    }

    T* insert(const T* pos, T&& value) {
        std::size_t pos_i = pos - &data_[0];
        if (size_ == capacity_) {
            expand(2*capacity_);
        }
        if (pos_i == size_) {
            std::construct_at(&data_[size_],value);
        } else {
            std::construct_at(&data_[size_],data_[size_-1]);
            std::size_t index = size_-1;
            for( ; index != pos_i; --index) {
                data_[index] = data_[index-1];
            }
            data_[index] = std::move(value);
        }
        size_++;
        return &data_[pos_i];
    }

    T* insert(const T* pos, std::size_t n, const T& value) {
        std::size_t pos_i = pos - &data_[0];
        if (size_ + n > capacity_) {
            std::size_t new_cap = size_ + n > 2*capacity_ ? size_ + n : 2*capacity_;
            expand(new_cap);
        }
        if (pos_i == size_) {
            for (std::size_t i = 0; i < n; i++) {
                std::construct_at(&data_[size_ + i],value);
            }
        } else {
            std::size_t index = size_ + n - 1;
            for ( ; index != pos_i + n - 1; --index) {
                if (index >= size_) {
                    std::construct_at(&data_[index],data_[index-n]);
                } else {
                    data_[index] = data_[index - n];
                }
            }
            for (std::size_t i = pos_i; i <= index; ++i) {
                if (i >= size_) {
                    std::construct_at(&data_[i],value);
                } else {
                    data_[i] = value;
                }
            }
        }
        size_ += n;
        return &data_[pos_i];
    }

    template< class InputIt, typename = std::void_t<
        decltype(*std::declval<InputIt>()),
        decltype(++std::declval<InputIt&>())
    >>
    T* insert(const T* pos, InputIt first, InputIt last) {
        std::size_t n = last - first;
        std::size_t pos_i = pos - &data_[0];
        if (size_ + n > capacity_) {
            std::size_t new_cap = size_ + n > 2*capacity_ ? size_ + n : 2*capacity_;
            expand(new_cap);
        }
        if (pos_i == size_) {
            InputIt it = first;
            for (std::size_t i = 0; i < n; ++i, ++it) {
                std::construct_at(&data_[size_ + i],*it);
            }
        } else {
            std::size_t index = size_ + n - 1;
            for ( ; index != pos_i + n - 1; --index) {
                if (index >= size_) {
                    std::construct_at(&data_[index],data_[index-n]);
                } else {
                    data_[index] = data_[index - n];
                }
            }
            InputIt it = first;
            for (std::size_t i = pos_i; i <= index; ++i, ++it) {
                if (i >= size_) {
                    std::construct_at(&data_[i],*it);
                } else {
                    data_[i] = *it;
                }
            }
        }
        size_ += n;
        return &data_[pos_i]; 
    }

    T* insert(const T* pos, std::initializer_list<T> ilist) {
        return insert(pos, ilist.begin(), ilist.end()); 
    }

    template< class... Args >
    T* emplace( const T* pos, Args&&... args) {
        std::size_t pos_i = pos - &data_[0];
        if (size_ == capacity_) {
            expand(2*capacity_);
        }
        if (pos_i == size_) {
            std::construct_at(&data_[size_],T(std::forward<Args>(args)...));
        } else {
            std::construct_at(&data_[size_],data_[size_-1]);
            std::size_t index = size_-1;
            for( ; index != pos_i; --index) {
                data_[index] = data_[index-1];
            }
            data_[index] = T(std::forward<Args>(args)...);
        }
        size_++;
        return &data_[pos_i];
    } 

    iterator erase(iterator pos ) {
        for (auto it = pos; it != end()-1; ++it) {
            *it = *(it + 1);
        }
        std::destroy_at(&data_[size_-1]);
        size_--;
        return pos;
    }

    iterator erase( const_iterator pos ) {
        for (auto it = pos; it != end()-1; ++it) {
            *it = *(it + 1);
        }
        std::destroy_at(&data_[size_-1]);
        size_--;
        return pos;
    }

    iterator erase( iterator first, iterator last ) {
        T* start = first;
        T* it = last;
        for (; it != end(); ++it, ++start) {
            *start = *it;
        }
        for(; start != end(); ++start) {
            std::destroy_at(start);
        }
        size_ -= (last - first);
        return first;
    }

    iterator erase( const_iterator first, const_iterator last ) {
        T* start = first;
        T* it = last;
        for (; it != end(); ++it, ++start) {
            *start = *it;
        }
        for(; start != end(); ++start) {
            std::destroy_at(start);
        }
        size_ -= (last - first);
        return first;
    }

    void push_back( const T& value ) {
        if (size_ == capacity_) {
            expand(1 > 2*capacity_ ? 1 : 2*capacity_);
        }
        std::construct_at(&data_[size_],value);
        size_++;
    }

    void push_back( T&& value ) {
        if (size_ == capacity_) {
            expand(1 > 2*capacity_ ? 1 : 2*capacity_);
        }
        std::construct_at(&data_[size_],std::move(value));
        size_++;
    }

    template< class... Args >
    void emplace_back( Args&&... args ) {
        if (size_ == capacity_) {
            expand(1 > 2*capacity_ ? 1 : 2*capacity_);
        }
        std::construct_at(&data_[size_],T(std::forward<Args>(args)...));
        size_++;
    }

    void pop_back() {
        std::destroy_at(&data_[size_ - 1]);
        size_--;
    }

    
    void resize(std::size_t count) {
        if (count == size_) {
            return;
        } else if (count > size_) {
            if (count > capacity_) {
                expand(count > 2*capacity_ ? count : 2*capacity_);
            }
            while (size_ != count) {
                std::construct_at(&data_[size_], T());
                size_++;
            }
            return;
        }
        while (size_ != count) {
            std::destroy_at(&data_[size_-1]);
            size_--;
        }
    }

    void resize(std::size_t count, const T& value) {
        if (count == size_) {
            return;
        } else if (count > size_) {
            if (count > capacity_) {
                expand(count > 2*capacity_ ? count : 2*capacity_);
            }
            while (size_ != count) {
                std::construct_at(&data_[size_], value);
                size_++;
            }
            return;
        }
        while (size_ != count) {
            std::destroy_at(&data_[size_-1]);
            size_--;
        }
    }

    void swap(vector<T>& other) {
        auto old_data = data_;
        auto old_size = size_;
        auto old_cap = capacity_;

        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        other.data_ = old_data;
        other.size_ = old_size;
        other.capacity_ = old_cap;
    } 


private:
    //辅助扩容函数，扩容到指定大小
    void expand(std::size_t new_cap) {
        if (new_cap <= capacity_) {
            return;
        }
        T* old_data = data_;
        std::size_t old_cap = capacity_;
        data_ = alloc_.allocate(new_cap);

        capacity_ = new_cap;
        for(std::size_t i = 0; i != size_; ++i) {
            std::construct_at(&data_[i], old_data[i]);
            std::destroy_at(&old_data[i]);
        }
        alloc_.deallocate(old_data,old_cap);
    }

    T* data_;
    std::size_t size_;
    std::size_t capacity_;
    Allocator alloc_;
};

//输出vector，方便测试
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