// C++ Program to demonstrate
// the use of Address-of operator (&)
#include <iostream>
using namespace std;
int myFunc(int x)
{
    return x += 20;
}
int main()
{
     int y=10;
    int *x = &y;
  
    // Pointer pointing towards x
    int **ptr = &x;
    cout<<**ptr;
    return 0;
// int (*funcptr)(int)=myFunc;

// int add=funcptr(&ptr);

    // cout << "The address of the variable x is :- " << add;
    return 0;
}