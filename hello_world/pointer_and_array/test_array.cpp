
#include <iostream>
using namespace std;

// =============================== Conclusion ==================
// 1. float * 实际上只保证了指针, 不保证可以作为数组正常使用。其空间分配只保证了一个 float的空间是被指向的；后续可以用数组的方式指向后续连续空间
// 但对于地址上内容的修改，是有可能造成意料外结果的 ---> 或者说，概念里要锁定 float *变量的 [] 取用是对连续地址空间的取用，但没有对应到任何预先分配、预留的空间上
// 2. [] 的下标方式在 float * 和 float *[5] 上会导向一样的结果：根据数组连续空间计算位置，取出对应位置上的值。不同的是，float *的方式不预留空间
// 3. int *arr[5] 代表 --5个-- 整型指针，int (*arr)[5] 代表指向 5个整数构成的数组的 --一个-- 指针
// 4. int *arr[5] 没有太好的赋值方式，只能数组里的一个一个元素赋值
// 5. vector的操作方式：分配pre_size, 在达到上限后分配 2* pre_size的连续空间，并进行一次拷贝操作（这是其标准实现）
// 6. 从这段程序的输出结果看，float b 和 int arr[] 的空间在赋值后进行了连续空间上的分配，arr[0] 的输出结果是被 a[1] 赋值覆盖以后的结果，
// 且由于 *a 是float类型，虽然赋值为3，但其实际存储在强制转为整型的时候就是一个奇怪的数字了
// 这可能跟编译器本身对堆栈上的空间怎么分配的行为也有关系

bool outsth(){
    cout << "ok!" << endl;
	return true;
}

int main()
{
	// bool outsth = outsth();
    outsth();

	// int* arr[10];
	float *a, *bb;
	// float b=1.0;
	
	// int *a, *bb;
	// int b = 1.0;
	int arr[5] = {9,11,12,13,14};
	cout << arr[0] << std::endl;
	cout << "int arr start from by arr[0]: " << &arr[0] << "," << &arr[1] << std::endl;
		
	float b=1.0;
	a = &b;
	a[1] = 3;
	cout << "int arr start from by arr: " << arr << std::endl;
    cout << "float pointer loc: " << a << "," << bb << std::endl;
	cout << "float pointer by [] loc: " << &(a[1]) << std::endl;
	
	cout << arr[0] << std::endl;
	
	int (*Parr)[5] = &arr;
	// int *Parr[5] = {&arr[0], &arr[1],&arr[2],&arr[3],&arr[4]};
	// int (*Parr22[10])[10];
	int *p = &arr[0];
	
	b = 2.0;
	cout << *a << std::endl;
	cout << "a:"<< a << ",bb:" << bb << std::endl;
	cout << "a:"<< &(a[0]) << ":" << &(a[1]) << std::endl;
	cout << "a:"<< a[0] << ":" << a[1] << std::endl;
	// cout << arr[0] << std::endl;
	// cout << *Parr[0] << std::endl;
	
	cout << "p:" << p << ",arr:" << arr << std::endl;
	p++;
	cout << "*p:" << *p << "," << arr[0] << std::endl;
	p++;
	cout << "*p:" << *p << std::endl;
	
	cout << "arr:" << &arr[0] << "," << &arr[1] << "," << &arr[2] << std::endl;
	cout << "parr:" << Parr[0] << "," << Parr[5] << "," << Parr[6] << std::endl;
	cout << "*parr:" << *Parr[0] << std::endl;

   return 0;
}