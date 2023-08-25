#pragma once 

#include "array_ptr.h" 

#include <cassert> 
#include <initializer_list> 
#include <utility> 


class ReserveProxyObj {

public:
    explicit ReserveProxyObj(size_t capacity_to_reserve) : capacity_(capacity_to_reserve) {}
    size_t Reserve_capacity() {

        return capacity_;

    }

private:
    size_t capacity_;
};



ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}


template <typename Type>

class SimpleVector {

public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size) : size_(size), capacity_(size) {
        ArrayPtr <Type> massiv(size);
        items_.swap(massiv);
    }

    explicit  SimpleVector(ReserveProxyObj capacity_to_reserve) {
        Reserve(capacity_to_reserve.Reserve_capacity());
        size_ = 0;

    }

    SimpleVector(size_t size, const Type& value) : size_(size), capacity_(size) {
        ArrayPtr <Type> massiv(size);
        items_.swap(massiv);
        std::fill(begin(), end(), value);
    }

    SimpleVector(std::initializer_list<Type> init) : size_(init.size()) , capacity_(init.size()) {
        ArrayPtr <Type> massiv(init.size());
        size_t i = 0;
        for (auto val : init) {
            massiv[i] = val;
            ++i;
        }
        items_.swap(massiv);
    }

    SimpleVector(const SimpleVector& other): size_ (other.GetSize()), capacity_(other.GetCapacity()) {
        ArrayPtr <Type> massiv(other.GetCapacity());
        std::copy(other.begin(), other.end(), &massiv[0]);
        items_.swap(massiv);
    }

    SimpleVector(SimpleVector&& other)
    {
        items_ = std::move(other.items_);
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
    }

    SimpleVector& operator=(const SimpleVector& other) {
        SimpleVector<Type> massiv(other);
        std::copy(other.begin(), other.end(), &massiv[0]);
        items_.swap(massiv);
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& other) {
        ArrayPtr <Type> massiv(other.size_);
        std::move(other.begin(), other.end(), &massiv[0]);
        items_.swap(massiv);
        size_ = std::exchange(other.size_, 0);
        capacity_ = std::exchange(other.capacity_, 0);
        return *this;
    }

    void PushBack(const Type& item) {
        if (this->size_ < this->capacity_) {
            items_[size_] = move(item);
            ++size_;
        }
        else {
            size_t new_capacity = 0;
            if (!this->IsEmpty()) { new_capacity = capacity_ * 2; }
            else { new_capacity = 1; }
            ArrayPtr <Type> massiv(new_capacity);
            std::fill(massiv.Get(), massiv.Get() + new_capacity, 0);
            std::move(this->begin(), this->end(), &massiv[0]);
            items_.swap(massiv);
            std::exchange(items_[size_], item);
            ++size_;
            capacity_ = new_capacity;
        }
    }

    void PushBack(Type&& item) {
        if (this->size_ < this->capacity_) {
            items_[size_] = move(item);
            ++size_;
        }
        else {
            size_t new_capacity = 0;
            if (!this->IsEmpty()) { new_capacity = capacity_ * 2; }
            else { new_capacity = 1; }
            ArrayPtr <Type> massiv(new_capacity);
            std::fill(massiv.Get(), massiv.Get() + new_capacity, 0);
            std::move(this->begin(), this->end(), &massiv[0]);
            items_.swap(massiv);
            items_[size_] = move(item);
            ++size_;
            capacity_ = new_capacity;
        }
    }

    void PopBack() noexcept {
        assert(this->IsEmpty());
            --size_;
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return (GetSize() == 0);
    }

    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return items_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index <= size_);
        return items_[index];
    }
    Type& At(size_t index) {
        if (index >= size_) { throw std::out_of_range("out_of_range"); }
        else { return items_[index]; }
    }

    const Type& At(size_t index) const {
        if (index >= size_) { throw std::out_of_range("out_of_range"); }
        else { return items_[index]; }
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        size_ = new_size;
        if (size_ > new_size) {
            return;
        }
        else if (size_ <= new_size && capacity_ > new_size) {
            for (auto iter = begin() + new_size; iter != end(); --iter) {
                *iter = std::move(Type{});
            }
            return;
        }
        else {
            ArrayPtr<Type> helper(new_size);
            std::move(begin(), end(), &helper[0]);
            items_.swap(helper);
            capacity_ = new_size * 2;
        }
    }

    void swap(SimpleVector& other) noexcept {
        this->items_.swap(other.items_);
        std::swap(this->size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= cbegin() && pos <= cend());
        if (this->IsEmpty()) {
            this->PushBack(value);
            return this->begin();
        }
        const size_t temp = this->GetSize();
        const size_t size_begin_pos = std::distance(this->cbegin(), pos);
        ArrayPtr <Type> massiv(temp * 2);
        massiv[0] = value;
        std::copy_backward(pos, this->cend(), &massiv[temp + 1]);
        massiv[size_begin_pos] = value;
        std::copy(this->cbegin(), pos, &massiv[0]);
        items_.swap(massiv);
        ++(this->size_);
        this->capacity_ = temp * 2;
        return (this->begin() + size_begin_pos);
    }

    Iterator Insert(Iterator pos, Type&& value) {
        assert(pos >= cbegin() && pos <= cend());
        if (this->IsEmpty()) {
            this->PushBack(std::move(value));
            return this->begin();
        }
        const size_t temp = this->GetSize();
        const size_t size_begin_pos = std::distance(this->begin(), pos);
        ArrayPtr <Type> massiv(temp * 2);
        std::move_backward(pos, this->end(), &massiv[temp + 1]);
        massiv[size_begin_pos] = move(value);
        std::move(this->begin(), pos, &massiv[0]);
        items_.swap(massiv);
        ++(this->size_);
        this->capacity_ = temp * 2;
        return (this->begin() + size_begin_pos);
    }

    Iterator Erase(ConstIterator pos) {
        assert(pos >= cbegin() && pos < cend());
        size_t temp = this->GetSize();
        assert(temp == 0);
        size_t size_begin_pos = std::distance(this->cbegin(), pos);
        ArrayPtr <Type> massiv(temp - 1);
        std::copy(this->cbegin(), pos, &massiv[0]);
        std::copy_backward(pos + 1, this->cend(), &massiv[temp - 1]);
        items_.swap(massiv);
        --(this->size_);
        return (this->begin() + size_begin_pos);
    }

    Iterator Erase(Iterator pos) {
        size_t temp = this->GetSize();
        size_t size_begin_pos = std::distance(this->begin(), pos);
        ArrayPtr <Type> massiv(temp - 1);
        std::move(this->begin(), pos, &massiv[0]);
        std::move_backward(pos + 1, this->end(), &massiv[temp - 1]);
        items_.swap(massiv);
        --(this->size_);
        return (this->begin() + size_begin_pos);
    }

    Iterator begin() noexcept {
        return Iterator{ items_.Get() };
    }

    Iterator end() noexcept {
        return Iterator{ &items_[size_] };
    }

    ConstIterator begin() const noexcept {
        return ConstIterator{ items_.Get() };
    }

    ConstIterator end() const noexcept {
        ConstIterator{ &items_[size_] };
    }

    ConstIterator cbegin() const noexcept {
        return ConstIterator{ items_.Get() };
    }

    ConstIterator cend() const noexcept {
        if (this->IsEmpty()) { return nullptr; }
        const Type* const temp = &items_[size_];
        return temp;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr <Type> massiv(new_capacity);
            std::fill(massiv.Get(), massiv.Get() + new_capacity, 0);
            std::copy(this->begin(), this->end(), &massiv[0]);
            items_.swap(massiv);
            capacity_ = new_capacity;
        }
    }

private:
    ArrayPtr <Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) { return false; }
    return ( std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin()));
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs > lhs);
}

