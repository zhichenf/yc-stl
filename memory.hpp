/**
 * 已实现类
 * default_delete
 * unique_ptr
 * 
 * @author YC奕晨 
 * */ 

#ifndef MEMORY_HPP_
#define MEMORY_HPP_

#include <type_traits>
#include <cstddef>
 
namespace ycstl {

//default_delete的实现
template<typename T> 
struct default_delete {
    constexpr default_delete() noexcept = default;

    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    default_delete(const default_delete<U>& d) noexcept {}

    void operator()(T* ptr) const noexcept {
        if (nullptr != ptr) {
            delete ptr;
        }
    }
};

template<typename T>
struct default_delete<T[]> {
    constexpr default_delete() noexcept = default;

    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U(*)[], T(*)[]>>>
    default_delete(const default_delete<U[]>& d) noexcept {}

    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U(*)[], T(*)[]>>>
    void operator()(U* ptr) const noexcept {
        if (nullptr != ptr) {
            delete[] ptr;
        }
    }
};

//unique_ptr的实现
template <typename T, typename Deleter = default_delete<T>>
class unique_ptr {
public:
    using pointer = T*;
    using element_type = T;
    using deleter_type = Deleter;

    constexpr unique_ptr() noexcept : data_(nullptr), del_() {}

    explicit unique_ptr(pointer p) noexcept : data_(p), del_() {}

    template <typename D = Deleter, typename = std::enable_if_t<
        std::is_constructible<Deleter, D&&>::value &&           //Deleter可以被d类型构造
        !std::is_same<std::decay_t<D>, unique_ptr>::value       //当别人传入错误的参数，会告知没有构造函数而编译失败，否则会在之后才报编译失败的错误
    >>
    unique_ptr(pointer p, D&& d) noexcept
        : data_(p), del_(std::forward<D>(d)) {}

    unique_ptr(unique_ptr&& u) noexcept : data_(u.data_), del_(std::move(u.del_)) {
        u.data_ = nullptr;
    }

    template<typename U, typename E, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    unique_ptr(unique_ptr<U,E>&& u) noexcept : data_(u.data_), del_(std::move(u.del_)) {
        u.data_ = nullptr;
    }

    //明确该构造函数接受nullptr类型
    constexpr unique_ptr(std::nullptr_t) noexcept : data_(nullptr), del_() {}

    ~unique_ptr() {
        del_(data_);
    }

    unique_ptr& operator=(unique_ptr&& u) noexcept {
        del_(data_);
        data_ = u.data_;
        del_ = std::move(u.del_);
        u.data_ = nullptr;
        return *this;
    }

    template<typename U, typename E, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    unique_ptr& operator=(unique_ptr<U,E>&& u) noexcept  {
        del_(data_);
        data_ = u.data_;
        del_ = std::move(u.del_);
        u.data_ = nullptr;
        return *this;
    }

    unique_ptr& operator=(std::nullptr_t) noexcept {
        del_(data_);
        del_ = Deleter();
        return *this;
    }

    std::add_lvalue_reference_t<T> operator*() const noexcept {
        return (*data_);
    }

    pointer operator->() const noexcept {
        return data_;
    }

    pointer get() const noexcept {
        return data_;
    }

    deleter_type& get_deleter() noexcept {
        return del_;
    }

    const deleter_type& get_deleter() const noexcept {
        return del_;
    }

    explicit operator bool() const noexcept {
        return nullptr != data_;
    }

    pointer release() noexcept {
        pointer p = data_;
        data_ = nullptr;
        return p;
    }

    void reset(pointer p = nullptr) noexcept {
        del_(data_);
        data_ = p;
    }

    void swap(unique_ptr& u) noexcept {
        pointer tmp = data_;
        data_ = u.data_;
        u.data_ = tmp;
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

private:
    T* data_;
    Deleter del_;

    template<typename, typename>
    friend class unique_ptr;
};

//数组类型特化
template<typename T, typename Deleter>
class unique_ptr<T[], Deleter> {
public:

    using pointer = T*;
    using element_type = T;
    using deleter_type = Deleter;

    constexpr unique_ptr() noexcept : data_(nullptr), del_() {
    }

    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, T*>>>
    explicit unique_ptr(U p) noexcept :data_(p), del_() {}

    template<typename U, typename D = Deleter, typename = std::enable_if_t<
        std::is_constructible<Deleter, D&&>::value &&           
        !std::is_same<std::decay_t<D>, unique_ptr>::value &&
        std::is_convertible_v<U(*)[], T(*)[]>      
    >>
    explicit unique_ptr(U* p, D&& d) noexcept : data_(p), del_(std::forward<D>(d)) {}

    unique_ptr(unique_ptr&& u) noexcept {
        data_ = u.data_;
        del_ = std::move(u.del_);
        u.data_ = nullptr;
    }

    template<typename U, typename E, typename = std::enable_if_t<std::is_convertible_v<std::remove_pointer_t<U>(*)[], T(*)[]>>>
    unique_ptr(unique_ptr<U,E>&& u) noexcept : data_(u.data_), del_(std::move(u.del_)) {
        u.data_ = nullptr;
    }

    constexpr unique_ptr(std::nullptr_t) noexcept : data_(nullptr), del_() {}

    ~unique_ptr() {
        del_(data_);
    }

    unique_ptr& operator=(unique_ptr&& u) noexcept {
        del_(data_);
        data_ = u.data_;
        del_ = std::move(u.del_);
        u.data_ = nullptr;
        return *this;
    }

    template<typename U, typename E, typename = std::enable_if_t<std::is_convertible_v<std::remove_pointer_t<U>(*)[], T(*)[]>>>
    unique_ptr& operator=(unique_ptr<U,E>&& u) noexcept {
        del_(data_);
        data_ = u.data_;
        del_ = std::move(u.del_);
        u.data_ = nullptr;
    }

    unique_ptr& operator=(std::nullptr_t) noexcept {
        del_(data_);
        data_ = nullptr;
        del_ = Deleter();
    }

    T& operator[](size_t offset) const {
        return *(data_ + offset);
    }

    pointer get() const noexcept {
        return data_;
    }

    deleter_type& get_deleter() noexcept {
        return del_;
    }

    const deleter_type& get_deleter() const noexcept {
        return del_;
    }    

    explicit operator bool() const noexcept {
        return nullptr != data_;
    }

    pointer release() noexcept {
        pointer p = data_;
        data_ = nullptr;
        return p;
    }

    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U(*)[], T(*)[]>>>
    void reset(U* p) {
        del_(data_);
        data_ = p;
    }

    void reset(std::nullptr_t = nullptr) noexcept {
        del_(data_);
        data_ = nullptr;
    }

    void swap(unique_ptr& u) noexcept {
        T* tmp = data_;
        data_ = u.data_;
        u.data_ = tmp;
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr operator=(const unique_ptr&) = delete;

private:
    T* data_;
    Deleter del_;

    template<typename, typename>
    friend class unique_ptr;
};

//unique相关的非成员函数
template<typename T, typename... Args, typename = std::enable_if_t<!std::is_array<T>::value>>
unique_ptr<T> make_unique( Args&&... args ) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T, typename = std::enable_if_t<std::is_array_v<T>>>
unique_ptr<T> make_unique(std::size_t size) {
    using ElementType = std::remove_extent_t<T>;
    return unique_ptr<T>(new ElementType[size]());
}

template<typename T, typename = std::enable_if_t<!std::is_array<T>::value>>
bool operator==(const unique_ptr<T>& u1, const unique_ptr<T>& u2) {
    return u1.get() == u2.get();
}

template<typename T, typename = std::enable_if_t<!std::is_array<T>::value>>
bool operator==(const unique_ptr<T>& u, std::nullptr_t) {
    return u.get() == nullptr;
}

template<typename T, typename = std::enable_if_t<!std::is_array<T>::value>>
bool operator==(std::nullptr_t, const unique_ptr<T>& u) {
    return u.get() == nullptr;
}

template<typename T, typename = std::enable_if_t<std::is_array<T>::value>>
bool operator==(const std::unique_ptr<T>& u1, const std::unique_ptr<T>& u2) {
    return u1.get() == u2.get();
}

template<typename T, typename = std::enable_if_t<std::is_array<T>::value>>
bool operator==(const std::unique_ptr<T>& u, std::nullptr_t) {
    return u.get() == nullptr;
}

template<typename T, typename = std::enable_if_t<std::is_array<T>::value>>
bool operator==(std::nullptr_t, const std::unique_ptr<T>& u) {
    return u.get() == nullptr;
}


}   //ycstl


#endif