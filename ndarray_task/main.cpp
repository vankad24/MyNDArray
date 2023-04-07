#include <iostream>
#include <ctime>
#include "NDArray.h"

#define println(x) std::cout << x << "\n"

int main() {
    //Создание пустого массива заданного размера
    NDArray<double> arr(5);
    NDArray<int> arr2({3,4});

    //Создание массива заполненного нулями
    NDArray<int> arr_zeros({5},0);

    //Создание массива заполненного единицами
    NDArray<int> arr_ones({5},1);

    //Создание массива заполненного случайными значениям
    srand(time(nullptr));
    NDArray<int> arr_rand = NDArray<int>(5).map([](int v){return rand();});

    //Операторы поэлементного сложения, вычитания, умножения, деления
    arr+=7;
    arr-=4;
    arr*=3;
    NDArray<double> arr3 = arr/2;

    //минимум, максимум, сумма, среднее
    arr.min();
    arr.max();
    arr.sum();
    arr.mean();

    //срезы
    NDArray<int> arr_i = NDArray<int>({2,3,2}).map([](int v,int i){return i;});
    println(arr_i);
    println(arr_i[-1].slice(1));
    arr_i[0].slice(-2)=4;
    println(arr_i);

    //присвоение и получение элемента
    arr_i[0][1][0] = 24;
    println(arr_i[0][1].get(0));

    //транспонирование и перемножение
    arr_i.reshape({3,4});
    NDArray<int> tr = arr_i.transpose();
    NDArray<int> product = arr_i.mul(tr);
    println(arr_i);
    println(tr);
    println(product);

    //сортировка и разворот
    NDArray<int> sorted = product.slice(1).sort();
    println(sorted);
    println(sorted.reverse());

    return 0;
}
