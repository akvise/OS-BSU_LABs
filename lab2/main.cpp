#include "Args.h"

void inputArray(Args a, int size) {
    for(int i = 0; i < size; i++) {
        std::cout << "Input " << i << " element of array: ";
        std::cin >> a.arr[i];
    }
}

void outputArray(Args a, int size) {
    std::cout << "All elements in array: ";
    for(int i = 0; i < size; i++) {
        std::cout << a.arr[i] << ' ';
    }
}

UINT WINAPI min_max(void* p){
    Args* args = static_cast<Args*>(p);
    int n = args->size;
    int* arr = args->arr;
    int max_, min_;

    max_ = 0;
    min_ = 0;

    for(int i = 1; i < n; i++) {
        if(arr[max_] < arr[i]){
            max_ = i;
        }
        if(arr[min_] > arr[i]){
            min_ = i;
        }
        Sleep(7);
    }
    args->maxEl = max_;
    args->minEl = min_;

    cout << "Max element: " << arr[max_] << endl;
    cout << "Min element: " << arr[min_] << endl;
    return 0;
}

UINT WINAPI average(void* p){
    Args* args = static_cast<Args*>(p);
    int n = args->size;
    int* arr = args->arr;
    int sum = 0;
    for(int i = 0; i < n; i++){
        sum += arr[i];
        Sleep(12);
    }
    args->avg = sum/n;
    printf("Average: %d \n", args->avg);
    return 0;
}

int main() {
    std::cout << "Input size of array: ";
    int n;
    std::cin >> n;

    Args* array = new Args();
    array->arr = new int[n];
    array->size = n;
    inputArray(*array, n);

    HANDLE hmin_max;
    hmin_max = (HANDLE)
            _beginthreadex(NULL, 0, min_max, array, 0, NULL);
    if(hmin_max == NULL) {
        return GetLastError();
    }

    HANDLE havg;
    havg = (HANDLE)
            _beginthreadex(NULL, 0, average, array, 0, NULL);
    if(havg == NULL){
        return GetLastError();
    }

    WaitForSingleObject(hmin_max, INFINITE);
    WaitForSingleObject(havg, INFINITE);

    array->arr[array->minEl] = array->avg;
    array->arr[array->maxEl] = array->avg;

    cout << "\nChanged array: " << endl;
    outputArray(*array, n);
    return 0;
}