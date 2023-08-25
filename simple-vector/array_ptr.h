#pragma once 

#include <cassert> 
#include <cstdlib> 
#include <algorithm> 
#include <iostream> 
#include <array> 
#include <utility> 

using namespace std;

template <typename Type>
class ArrayPtr {
public:
    ArrayPtr() = default;
    explicit ArrayPtr(size_t size) {
        if (size != 0) {
            Type* temp = new Type[size]{};
            raw_ptr_ = temp;
        }
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    ArrayPtr(const ArrayPtr&) = delete;

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }
    ArrayPtr& operator=(ArrayPtr&& rhs) {
        if (this == &rhs) {
            return *this;
        }
        raw_ptr_ = rhs.raw_ptr_;
        rhs.raw_ptr_ = nullptr;
        return *this;
    }

    Type& operator=(Type&& a) {
        return std::move(a);
    }

    [[nodiscard]] Type* Release() noexcept {
        Type* result = raw_ptr_;
        raw_ptr_ = nullptr;
        return result;
    }

    Type& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        const Type& result = raw_ptr_[index];
        return result;
    }

    explicit operator bool() const {
        return raw_ptr_ == nullptr ? false : true;
    }

    Type* Get() const noexcept {
        return &raw_ptr_[0];
    }

    void swap(ArrayPtr& other) noexcept {
        std::swap(raw_ptr_, other.raw_ptr_);
    }

    template <typename HZ>
    void change(HZ a) {
        this->raw_ptr_ = a;
    }

private:
    Type* raw_ptr_ = nullptr;
};

