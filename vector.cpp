#include <iostream>
#include <stdexcept>
#include <initializer_list>
#include <algorithm>
#include <utility>
#include <cassert>
#include <type_traits>
#include<vector>
#include <iostream>
#include <cstdlib>
#include <compare>

template<typename T>
class MyVector{
public:
    MyVector():size_(0),capacity_(0),arr_(nullptr){}
    MyVector(size_t size, const T& value=T()):size_(0){
        Resize((capacity_==0)?1:2*capacity_);
        for(size_t i=0;i<size;i++){
            push_back(value);
        }
    }
    MyVector(MyVector&& other) noexcept{
        size_=other.size_;
        capacity_=other.capacity_;
        arr_=other.arr_;
        other.size_=0;
        other.capacity_=0;
        other.arr_=nullptr;
    }
    MyVector(const MyVector&& other) noexcept{
        size_=other.size_;
        capacity_=other.capacity_;
        arr_=other.arr_;
        other.size_=0;
        other.capacity_=0;
        other.arr_=nullptr;
    }
    MyVector(const MyVector& other):arr_(nullptr),size_(other.size_),capacity_(other.capacity_){
        if(capacity_){
            arr_=static_cast<T*>(malloc(capacity_*sizeof(T)));
            if(!arr_){
                throw std::bad_alloc();
            }
            for(size_t i=0;i<size_;i++){
                if constexpr(std::is_copy_constructible_v<T>){
                    new (&arr_[i]) T(other.arr_[i]);
                }
            }
        }
    }
    ~MyVector(){
        free(arr_);
    }
    MyVector& operator=(MyVector&& other) noexcept{
        if(this!=&other){
            size_=other.size_;
            capacity_=other.capacity_;
            arr_=other.arr_;
            other.size_=0;
            other.capacity_=0;
            other.arr_=nullptr;
        }
        return *this;
    }
    const MyVector& operator=(const MyVector&& other){
        if(this!=&other){
            size_=other.size_;
            capacity_=other.capacity_;
            arr_=other.arr_;
            other.size_=0;
            other.capacity_=0;
            other.arr_=nullptr;
        }
        return *this;
    }
    MyVector& operator=(const MyVector& other){
        if(this!=&other){
            arr_=nullptr;
            capacity_=other.capacity_;
            size_=other.size_;
            if(capacity_){
                arr_=static_cast<T*>(malloc(capacity_*sizeof(T)));
                if(!arr_){
                    throw std::bad_alloc();
                }
                for(size_t i=0;i<size_;i++){
                    if constexpr(std::is_copy_constructible_v<T>){
                        new (&arr_[i]) T(other.arr_[i]);
                    }
                }
            }
        }
        return *this;
    }
    void push_back(T&& val) {
        if(size_==capacity_){
            Resize((capacity_==0)?1:2*capacity_);
        }
        if constexpr(std::is_move_constructible_v<T>){
            new (&arr_[size_++]) T(std::move(val));
        }else{
            new (&arr_[size_++]) T(val);
        }
    }
    void push_back(const T& val) {
        if(size_==capacity_){
            Resize((capacity_==0)?1:2*capacity_);
        }
        if constexpr(std::is_copy_constructible_v<T>){
            new (&arr_[size_++]) T(val);
        }
    }
    template <typename U>
    void push_back(U&& value) {
        if (size_ == capacity_) {
            Resize((capacity_ == 0) ? 1 : 2 * capacity_);
        }
        new (&arr_[size_++]) T(std::forward<U>(value));
    }
    template<typename ...Args>
    void emplace_back(Args&& ...args){
        if (size_ == capacity_) {
            Resize((capacity_ == 0) ? 1 : 2 * capacity_);
        }
        new (&arr_[size_++]) T(std::forward<Args>(args)...);
    }
    void pop_back(){
        if(size_==0){
            throw std::out_of_range("Cannot pop empty vector");
        }
        size_--;
    }
    T& operator[](size_t ind){
        if(ind>=size_){
            throw std::out_of_range("Element is inaccessible");
        }
        return arr_[ind];
    }
    const T& operator[](size_t ind) const{
        if(ind>=size_){
            throw std::out_of_range("Element is inaccessible");
        }
        return arr_[ind];
    }
    size_t size(){
        return size_;
    }
    size_t capacity(){
        return capacity_;
    }
    bool empty(){
        return size_==0;
    }
    void clear(){
        size_=0;
    }
    void swap(MyVector&& other ){
        std::swap(size_,other.size_);
        std::swap(capacity_,other.capacity_);
        std::swap(arr_,other.arr_);
    }
    void swap(MyVector& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(arr_, other.arr_);
    }
    void resize(size_t new_size){
        if(new_size<=size_){
            size_=new_size;
            return;
        }
        for(size_t i=size_;i<new_size;++i){
            push_back(T());
        }
    }
    void reserve(size_t new_capacity){
        if(new_capacity>=capacity_){
            Resize(new_capacity);
        }
    }
    void shrink_to_fit() {
        Resize(size_);
    }
    void erase(size_t ind){
        if(ind>=size_){
            throw std::out_of_range("Index out of range");
        }
        arr_[ind].~T();
        for(size_t i=ind;i<size_-1;i++){
            new (&arr_[i]) T(std::move(arr_[i+1]));
            arr_[i+1].~T();
        }
        --size_;
    }
    T& front(){
        if(size_==0){
            throw std::out_of_range("Vector is empty");
        }
        return  arr_[0];
    }
    T& back(){
        if(size_==0){
            throw std::out_of_range("Vector is empty");
        }
        return  arr_[size_-1];
    }
    void insert(size_t ind,const T& value){
        if(ind>=size_){
            throw std::out_of_range("Index out of vector");
        }
        if(size_==capacity_){
            Resize((capacity_ == 0) ? 1 : 2 * capacity_);
        }
        for(size_t i=ind;i<size_;i++){
            new (&arr_[i+1]) T(std::move(arr_[i]));
            arr_[i].~T();
        }
        new (&arr_[ind]) T(value);
        ++size_;
    }
    void assign(size_t count,const T& val){
        clear();
        for(size_t i=0;i<count;i++){
            push_back(val);
        }
    }
    T& at(size_t ind){
        if(ind>=size_){
            throw std::out_of_range("Index out of bounds");
        }
        return arr_[ind];
    }
    class Iterator{
    public:
        using iterator_category=std::random_access_iterator_tag;
        using value_type=T;
        using difference_type=std::ptrdiff_t;
        using pointer=T*;
        using reference=T&;
        Iterator(T* ptr):ptr_(ptr){}
        ~Iterator(){
            ptr_=nullptr;
        }
        reference operator*(){
            return *ptr_;
        }
        pointer operator->() const{
            return ptr_;
        }
        Iterator& operator++(){
            ++ptr_;
            return *this;
        }
        Iterator& operator++(int){
            Iterator temp=*this;
            ++ptr_;
            return temp;
        }
        Iterator& operator--(){
            --ptr_;
            return *this;
        }
        Iterator& operator--(int){
            Iterator temp=*this;
            --ptr_;
            return temp;
        }
        std::strong_ordering operator<=>(const Iterator& other) const{
            return ptr_<=>other.ptr_;
        }
        bool operator==(const Iterator& other) const{
            return ptr_==other.ptr_;
        }

        Iterator operator+(difference_type n) const{
            return Iterator(ptr_+n);
        }
        Iterator& operator+=(difference_type n) const{
            ptr_+=n;
            return *this;
        }
        difference_type operator-(const Iterator& other) const{
            return ptr_-other.ptr_;
        }
        reference operator[](difference_type n) const{
            return *(ptr_+n);
        }

        private:
        T* ptr_;
    };
    Iterator begin(){
        return Iterator(arr_);
    }
    Iterator end(){
        return Iterator(arr_+size_);
    }
    class ConstIterator{
    public:
        using iterator_category=std::random_access_iterator_tag;
        using value_type=const T;
        using difference_type=std::ptrdiff_t;
        using pointer=T*;
        using reference=const T&;
        ConstIterator(T* ptr):ptr_(ptr){}
        ~ConstIterator(){
            ptr_=nullptr;
        }
        reference operator*(){
            return *ptr_;
        }
        pointer operator->() const{
            return ptr_;
        }
        ConstIterator& operator++(){
            ++ptr_;
            return *this;
        }
        ConstIterator& operator++(int){
            Iterator temp=*this;
            ++ptr_;
            return temp;
        }
        ConstIterator& operator--(){
            --ptr_;
            return *this;
        }
        ConstIterator& operator--(int){
            Iterator temp=*this;
            --ptr_;
            return temp;
        }
        std::strong_ordering operator<=>(const ConstIterator& other) const{
            return ptr_<=>other.ptr_;
        }
        bool operator==(const ConstIterator& other) const{
            return ptr_==other.ptr_;
        }

        ConstIterator operator+(difference_type n) const{
            return Iterator(ptr_+n);
        }
        ConstIterator& operator+=(difference_type n) const{
            ptr_+=n;
            return *this;
        }
        difference_type operator-(const ConstIterator& other) const{
            return ptr_-other.ptr_;
        }
        reference operator[](difference_type n) const{
            return *(ptr_+n);
        }

        private:
        const T* ptr_;
    };
    ConstIterator begin() const{
        return ConstIterator(arr_);
    }
    ConstIterator end() const{
        return ConstIterator(arr_+size_);
    }
    std::strong_ordering operator<=>(const MyVector& other) const{
        for(size_t i=0;i<((size_<other.size())? size_:other.size());++i){
            if(*this[i]!=other[i]){
                return *this[i]<=>other[i];
            }
        }
        return size_<=>other.size();
    }
private:
    void Resize(size_t new_capacity){
        T* new_arr=static_cast<T*>(realloc(arr_, new_capacity * sizeof(T)));
        if(!new_arr){
            throw std::bad_alloc();
        }
        arr_=new_arr;
        capacity_=new_capacity;
    }
    size_t size_;
    size_t capacity_;
    T*arr_;
};