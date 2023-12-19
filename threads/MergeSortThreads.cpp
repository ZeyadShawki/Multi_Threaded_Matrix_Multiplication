#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <pthread.h>

using namespace std;

struct ThreadData
{
    int *arr;
    int left;
    int right;
};

void merge(int array[], int left, int mid, int right)
{

    int subArrayleft = mid - left + 1;
    int subArrayRight = right - mid;

    auto *leftArray = new int[subArrayleft];
    auto *rightArray = new int[subArrayRight];

    for (int i = 0; i < subArrayleft; i++)
    {
        leftArray[i] = array[left + i];
    }
    for (int i = 0; i < subArrayRight; i++)
    {
        rightArray[i] = array[mid + 1 + i];
    }

    auto indexOfSubArrayOne = 0, indexOfSubArrayTwo = 0;
    int indexOfMergedArray = left;

    while (indexOfSubArrayOne < subArrayleft && indexOfSubArrayTwo < subArrayRight

    )
    {
        if (leftArray[indexOfSubArrayOne] <= rightArray[indexOfSubArrayTwo])
        {
            array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
            indexOfSubArrayOne++;
        }else{
            array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
            indexOfSubArrayTwo++;
        }
        indexOfMergedArray++;
    }

    while (indexOfSubArrayOne < subArrayleft)
    {
        array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
        indexOfSubArrayOne++;
        indexOfMergedArray++;
}

while (indexOfSubArrayTwo < subArrayRight)
{
    array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
    indexOfSubArrayTwo++;
    indexOfMergedArray++;
}
delete[] leftArray;
delete[] rightArray;
}

void *mergeSortThread(void *data)
{
    ThreadData *td = static_cast<ThreadData *>(data);

    if (td->left < td->right)
    {
        int mid = td->left + (td->right - td->left) / 2;

        // Create threads for the left and right halves
        pthread_t leftThread, rightThread;
        ThreadData leftData = {td->arr, td->left, mid};
        ThreadData rightData = {td->arr, mid + 1, td->right};

        pthread_create(&leftThread, nullptr, mergeSortThread, &leftData);
        pthread_create(&rightThread, nullptr, mergeSortThread, &rightData);

        // Wait for the threads to finish
        pthread_join(leftThread, nullptr);
        pthread_join(rightThread, nullptr);

        // Merge the sorted halves
        merge(td->arr, td->left, mid, td->right);
    }

    return nullptr;
}

// UTILITY FUNCTIONS
// Function to print an array
void printArray(int A[], int size)
{
    for (int i = 0; i < size; i++)
        cout << A[i] << " ";
    cout << endl;
}
void mergeSort(int arr[], int size)
{
    ThreadData data = {arr, 0, size - 1};
    mergeSortThread(&data);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <sort-file>" << endl;
        return 1;
    }

    ifstream file(argv[1]);

    if (!file.is_open())
    {
        cerr << "Error: Unable to open the file." << endl;
        return 1;
    }

    int n;
    file >> n;

    int *arr = new int[n];

    for (int i = 0; i < n; ++i)
    {
        file >> arr[i];
    }

    file.close();

    cout << "Given array is \n";
    for (int i = 0; i < n; i++)
        cout << arr[i] << " ";
    cout << endl;

    // Apply merge sort
    mergeSort(arr, n);

    cout << "Sorted array is \n";
    for (int i = 0; i < n; i++)
        cout << arr[i] << " ";
    cout << endl;

    delete[] arr;

    return 0;
}
