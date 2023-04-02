//
// Created by Vanka on 03.08.2022.
//
#ifndef MY_IDEAS_NDArray_H
#define MY_IDEAS_NDArray_H

#include <typeinfo>
#include <functional>
#include <iostream>
#include <vector>
#include <cstring>
using std::function;


struct InvalidIndexException : public std::exception {
    const char *what() const throw() {
        return "Invalid index";
    }
};

struct ArrayIndexException : public std::exception {
    const char *what() const throw() {
        return "Index out of bounds exception";
    }
};

template<typename T>
class NDArray {
private:

    int _length;//make it constant
    bool sliced = false;
    T *_data;
    std::vector<unsigned int> _shape;
    int step=1; //iteration step

    NDArray createSlice(T *array, std::vector<unsigned int> shape){
        NDArray<T> arr;
        arr.sliced = true;
        arr._data = array;
        arr.setShape(shape);
        return arr;
    }

    int shapeToLen(const std::vector<unsigned int>& shape){
        int len = 1;
        for (auto x: shape)len*=x;
        return len;
    }

    void setShape(const std::vector<unsigned int>& shape){
        _shape = shape;
        _length = shape.size()? 1 : 0;
        for (auto x: shape)_length*=x;
    }

    template<typename... Args>
    int _getNIndexToIndex(Args... indexes){
        std::vector<unsigned int> temp = std::vector<unsigned int>({indexes...});
        int index = temp.begin()[temp.size()-1];
        int block_size = 1;
        for (int i = temp.size()-2; i>=0; --i){
            block_size*=_shape.begin()[i+1];
            index+= block_size * temp.begin()[i];
        }
        return index;
    }

public:

    inline int totalLength(){
        return _length;
    }

    inline int length(){
        return _shape[0];
    }

    inline std::vector<unsigned int> shape(){
        return _shape;
    }

    NDArray& reshape(std::vector<unsigned int> shape){
        if (shapeToLen(shape)!=_length)throw InvalidIndexException();
        _shape = shape;
        return *this;
    }

    NDArray(){ _data = nullptr; setShape({0});}

    NDArray(const unsigned int length): NDArray(std::vector<unsigned int>({length})){}

    NDArray(const std::vector<unsigned int> shape) {
        setShape(shape);
        _data = new T[_length];
    }

    NDArray(const T *array, std::vector<unsigned int> shape) : NDArray(shape) {
        memcpy(_data, array, sizeof(T) * _length);
    }

    NDArray(const std::vector<unsigned int> shape, const T &value): NDArray(shape){
        fill(value);
    }

    //copy constructor
    NDArray(const NDArray<T> &arr): NDArray(arr._data, arr._shape){/*std::cout<<"copy!\n";*/}

    ~NDArray() {
        if (!sliced) delete[] _data;
    }

    template<typename U> //cout print
    friend std::ostream& operator << (std::ostream &out, const NDArray<U> &arr);

    template<typename U> //cin
    friend std::istream& operator >> (std::istream &in, NDArray<U> &arr);

    NDArray operator[](int index) {
        if (index < 0) index+=_shape[0];
        if (index >= _shape[0] || index<0)throw ArrayIndexException();
        std::vector<unsigned int> v = _shape;
        v.erase(v.begin(),v.begin()+1);
        NDArray<T> arr;
        arr.sliced = true;
        arr.setShape(v);
        arr._data = _data+arr._length*index;
        return arr;
    }

    T& get(int index) {
        if (_shape.empty())throw ArrayIndexException();
        if (index < 0) index += _shape[0];
        if (index >= _shape[0])throw ArrayIndexException();
        return _data[index];
    }

    NDArray &operator +=(const T &value){
        for (int i = 0; i < _length; i++) _data[i]+=value;
        return *this;
    }

    NDArray &operator -=(const T &value){
        for (int i = 0; i < _length; i++) _data[i]-=value;
        return *this;
    }

    NDArray &operator *=(const T &value){
        for (int i = 0; i < _length; i++) _data[i]*=value;
        return *this;
    }

    NDArray &operator /=(const T &value){
        for (int i = 0; i < _length; i++) _data[i]/=value;
        return *this;
    }

    NDArray operator +(const T &value){
        return NDArray(*this)+=value;
    }

    NDArray operator -(const T &value){
        return NDArray(*this)-=value;
    }

    NDArray operator *(const T &value){
        return NDArray(*this)*=value;
    }

    NDArray operator /(const T &value){
        return NDArray(*this)/=value;
    }

    NDArray &operator =(const T &value){
        return fill(value);
    }

    NDArray &operator =(const NDArray& arr){
        if (&arr != this) {
            _length = arr._length;
            delete[] _data;
            _data = new T[_length];
            memcpy(_data, arr._data, sizeof(T) * _length);
        }
        return *this;
    }

    bool operator ==(NDArray<T>& arr){
        return equals(arr);
    }

    T& first(){
        return operator[](0);
    }

    T& last(){
        return operator[](-1);
    }

    NDArray operator ()(int start, int end){
        return slice(start, end);
    }

    NDArray operator ()(int end){
        return slice(0, end);
    }

    bool equals(NDArray<T>& arr){
        if (&arr==this)return true;
        if (_length != arr._length || _shape != arr._shape)return false;
        for (int i = 0; i < _length; i++) {
            if (_data[i]!=arr._data[i])return false;
        }
        return true;
    }

    NDArray &fill(const T &value){
        for (int i = 0; i < _length; i++)_data[i] = value;
        return *this;
    }

    int indexOf(const T &value, int startIndex = 0){
        if (startIndex<0|| startIndex >= _length) throw InvalidIndexException();
        for (; startIndex < _length; startIndex++)
            if (_data[startIndex] == value)return startIndex;
        return -1;
    }

    int lastIndexOf(const T &value, int startIndex){
        if (startIndex<0 || startIndex >= _length) throw InvalidIndexException();
        for (;startIndex >= 0; startIndex--)
            if (_data[startIndex] == value)return startIndex;
        return -1;
    }
    int lastIndexOf(const T &value){
        return lastIndexOf(value, _length - 1);
    }

    bool contains(const T &value){
        for (int i = 0; i < _length; i++) {
            if (_data[i]==value)return true;
        }
        return false;
    }

    T min(){
        T m = get(0);
        for (int i = 1; i < _length; i++)
            if (_data[i]<m)m=_data[i];
        return m;
    }

    T max(){
        T m = get(0);
        for (int i = 1; i < _length; i++)
            if (_data[i]>m)m=_data[i];
        return m;
    }


    void forEach(const function<void(T &value)> &func){
        for (int i = 0; i < _length; i++)
            func(_data[i]);
    }

    void forEach(const function<void(T &value, int index)> &func){
        int index = 0;
        for (int i = 0; i < _length; i++)
            func(_data[i],index++);
    }

    template<typename U>
    void forEach(const function<U(T &value)> &func){
        for (int i = 0; i < _length; i++)
            func(_data[i]);
    }

    template<typename U>
    void forEach(const function<U(T &value, int index)> &func){
        int index = 0;
        for (int i = 0; i < _length; i++)
            func(_data[i],index++);
    }

    NDArray &map(const function<T(T &value)> &func){
        for (int i = 0; i < _length; i++)
            _data[i] = func(_data[i]);
        return *this;
    }

    NDArray &map(const function<T(T &value, int index)> &func){
        int index = 0;
        for (int i = 0; i < _length; i++)
            _data[i] = func(_data[i],index++);
        return *this;
    }

    int count(const function<bool(T &value)> &func){
        int k = 0;
        for (int i = 0; i < _length; i++)
            if (func(_data[i]))++k;
        return k;
    }

    int count(const function<bool(T &value, int index)> &func){
        int k = 0;
        int index = 0;
        for (int i = 0; i < _length; i++)
            if (func(_data[i], index++))++k;
        return k;
    }

    int count(const T &value){
        int k = 0;
        for (int i = 0; i < _length; i++)
            if (_data[i]==value)++k;
        return k;
    }

    bool any(const function<bool(T &value)> &func){
        for (size_t i = 0; i < _length; i++) {
            if (func(_data[i]))return true;
        }
        return false;
    }

    bool all(const function<bool(T &value)> &func){
        for (size_t i = 0; i < _length; i++) {
            if (!func(_data[i]))return false;
        }
        return true;
    }

    T reduce(const function<T (T &val1, T &val2)> &func){
        T value = operator[](0);
        for (int i = 1; i < _length; i++) {
            value = func(value, _data[i]);
        }
        return value;
    }

    T reduce(const function<T (T &val1, T &val2)> &func, const T &startValue){
        T value = startValue;
        for (int i = 0; i < _length; i++) {
            value = func(value, _data[i]);
        }
        return value;
    }

    NDArray filter(const function<bool(T &value)> &func){
        T* temp = new T[_length];
        int new_len = 0;
        for (int i = 0; i < _length; i++) {
            if (func(_data[i]))temp[new_len++] = _data[i];
        }
        NDArray arr(temp,new_len);
        delete[] temp;
        return arr;
    }

    NDArray filter(const function<bool (T &value, int index)> &func){
        T* temp = new T[_length];
        int new_len = 0;
        int index = 0;
        for (int i = 0; i < _length; i++) {
            if (func(_data[i],index++))temp[new_len++] = _data[i];
        }
        NDArray arr(temp,new_len);
        delete[] temp;
        return arr;
    }

    NDArray slice(int start, int end){
        if (start < 0) start+=_shape[0];
        if (end < 0) end+=_shape[0];
        if (start >= _shape[0] || start<0 ||
                end > _shape[0] || end<0 ||
                start>end)throw ArrayIndexException();
        std::vector<unsigned int> v = _shape;
        v[0] = end-start;
        NDArray<T> arr;
        arr.sliced = true;
        arr.setShape(v);
        arr._data = _data+_length/_shape[0]*start;
        return arr;
    }

    NDArray slice(int start){
        return slice(start, _shape[0]);
    }


    T* toArray(){
        if (_length == 0)return nullptr;
        T* arr = new T[_length];
        memcpy(arr,_data, sizeof(T) * _length);
        return arr;
    }

    T* getData(){
        return _data;
    }

    /*NDArray &reverse(){
        T temp;
        int index, end = _length >> 1u;
        for (int i = 0; i < end; ++i) {
            index = _length - i - 1;
            temp = _data[i];
            _data[i] = _data[index];
            _data[index] = temp;
        }
        return *this;
    }

    NDArray &sort(bool descending = false){
        sorts::heapSort(_data, _length);
		if (descending)reverse();
        return *this;
    }
    */
    T sum(){
        T sum = get(0);
        for (int i = 1; i < _length; i++)
            sum+=_data[i];
        return sum;
    }

    T mean(){
        return sum()/length();
    }

    //iterator
    template<typename ValueType>
    class MyIterator: public std::iterator<std::input_iterator_tag, ValueType>{
        friend class NDArray<ValueType>;//for private constructor
    private:
        ValueType* p;
        MyIterator(ValueType* p):p(p) {}

    public:
        MyIterator(const MyIterator &it):p(it.p){}

        bool operator!=(MyIterator const& other) const {
            return p != other.p;;
        }

        bool operator==(MyIterator const& other) const{
            return p == other.p;
        }


        MyIterator& operator++() {
            ++p;
            return *this;
        }
        MyIterator& operator--() {
            --p;
            return *this;
        }

        MyIterator& operator+=(int num) {
            p+=num;
            return *this;
        }

        MyIterator& operator-=(int num) {
            p-=num;
            return *this;
        }

        typename MyIterator<ValueType>::reference operator*() const{
            return *p;
        };
    };

    typedef MyIterator<T> iterator;
    typedef MyIterator<const T> const_iterator;

    iterator begin(){
        return iterator(_data);
    }

    iterator end(){
        return iterator(_data + _length);
    }

    const_iterator begin() const{
        return const_iterator(_data);
    }

    const_iterator end() const{
        return const_iterator(_data + _length);
    }

};

template <typename U>
std::ostream& operator << (std::ostream &out, const NDArray<U> &arr){
    std::vector<unsigned int> v(arr._shape.size());
    out<< "[";
    int i = 0;
    int j = 0;
    while (v[0] < arr._shape[0]) {
        if (v[i] == arr._shape[i]){
            v[i]=0;
            --i;
            ++v[i];
            out << "]";
            if (v[i] != arr._shape[i])out << ", ";
        }else if (i<v.size()-1){
            ++i;
            out << "[";
        }else{
            ++v[i];
            out << arr._data[j++];
            if (v[i] != arr._shape[i])out << ", ";
        }
    }
    return out << "]";
}

template<typename U> //cin
std::istream& operator >> (std::istream &in, NDArray<U> &arr){
    for (int i = 0; i < arr._length; i+=arr.step) {
        in>>arr[i];
    }
    return in;
}

#endif //MY_IDEAS_NDArray_H

//fill
//indexOf, lastIndexOf
//contains
//sum
//min
//max

//extend constructor
//copy constructor

//forEach
//map
//count(bool func)

//sort
//reverse

//equals
//compare---
//binary search
//hash ---

//slice

//dimensions---

//any
//all