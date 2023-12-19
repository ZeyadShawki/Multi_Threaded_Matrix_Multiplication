#include <iostream>
#include <fstream>
#include "pthread.h"
#include <chrono>

using namespace std;

const int MAX_ROWS = 400;
const int MAX_COLS = 400;

struct ThreadData
{
    int start_row;
    int end_row;
    int cols1;
    int cols2;
    int (*mat1)[MAX_COLS];
    int (*mat2)[MAX_COLS];
    int (*result)[MAX_COLS];
};

// Matrix multiplication where each element is computed in a thread
void *multiplyElementsThread(void *data)
{
    ThreadData *td = static_cast<ThreadData *>(data);

    for (int i = td->start_row; i < td->end_row; ++i)
    {
        for (int j = 0; j < td->cols2; ++j)
        {
            td->result[i][j] = 0;
            for (int k = 0; k < td->cols1; ++k)
            {
                td->result[i][j] += td->mat1[i][k] * td->mat2[k][j];
            }
        }
    }

    return nullptr;
}

// Matrix multiplication where each row is computed in a thread
void *multiplyRowsThread(void *data)
{
    ThreadData *td = static_cast<ThreadData *>(data);

    for (int i = td->start_row; i < td->end_row; ++i)
    {
        for (int j = 0; j < td->cols2; ++j)
        {
            td->result[i][j] = 0;
            for (int k = 0; k < td->cols1; ++k)
            {
                td->result[i][j] += td->mat1[i][k] * td->mat2[k][j];
            }
        }
    }

    return nullptr;
}

// Function to print a matrix
void printMatrix(int mat[MAX_ROWS][MAX_COLS], int rows, int cols)
{
    cout << rows << " " << cols << endl;
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            cout << mat[i][j] << " ";
        }
        cout << endl;
    }
}

bool readMatricesFromFile(const char *filename, int mat1[MAX_ROWS][MAX_COLS], int &rows1, int &cols1,
                          int mat2[MAX_ROWS][MAX_COLS], int &rows2, int &cols2)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Unable to open the file." << endl;
        return false;
    }

    file >> rows1 >> cols1;

    if (rows1 <= 0 || cols1 <= 0)
    {
        cerr << "Error: Invalid matrix dimensions in the file." << endl;
        file.close();
        return false;
    }

    for (int i = 0; i < rows1; ++i)
    {
        for (int j = 0; j < cols1; ++j)
        {
            if (!(file >> mat1[i][j]))
            {
                cerr << "Error: Unable to read matrix entries from the file." << endl;
                file.close();
                return false;
            }
        }
    }

    // Read the second matrix
    file >> rows2 >> cols2;

    if (rows2 <= 0 || cols2 <= 0)
    {
        cerr << "Error: Invalid matrix dimensions in the file for the second matrix." << endl;
        file.close();
        return false;
    }

    for (int i = 0; i < rows2; ++i)
    {
        for (int j = 0; j < cols2; ++j)
        {
            if (!(file >> mat2[i][j]))
            {
                cerr << "Error: Unable to read matrix entries for the second matrix." << endl;
                file.close();
                return false;
            }
        }
    }

    file.close();
    return true;
}

// Matrix multiplication where each element is computed in a thread
void multiplyElements(int mat1[MAX_ROWS][MAX_COLS], int mat2[MAX_ROWS][MAX_COLS], int result[MAX_ROWS][MAX_COLS],
                      int rows1, int cols1, int rows2, int cols2)
{
    auto start_time = chrono::high_resolution_clock::now();

    pthread_t threads[rows1 * cols2];
    ThreadData threadData[rows1 * cols2];

    int rows = rows1;
    int cols = cols2;

    int threadCount = 0;

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols2; ++j)
        {
            threadData[threadCount].start_row = i;
            threadData[threadCount].end_row = i + 1;
            threadData[threadCount].cols1 = cols1;
            threadData[threadCount].cols2 = cols2;
            threadData[threadCount].mat1 = mat1;
            threadData[threadCount].mat2 = mat2;
            threadData[threadCount].result = result;

            pthread_create(&threads[threadCount], nullptr, multiplyElementsThread, &threadData[threadCount]);

            ++threadCount;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < threadCount; ++i)
    {
        pthread_join(threads[i], nullptr);
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto elapsed_time = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();

    // Output the result matrix and additional information
    printMatrix(result, rows, cols);
    cout << "END1 " << elapsed_time << " microseconds" << endl;
    cout << "-----------------------------------" << endl;
}

// Matrix multiplication where each row is computed in a thread
void multiplyRows(int mat1[MAX_ROWS][MAX_COLS], int mat2[MAX_ROWS][MAX_COLS], int result[MAX_ROWS][MAX_COLS],
                  int rows1, int cols1, int rows2, int cols2)
{
    auto start_time = chrono::high_resolution_clock::now();

    pthread_t threads[rows1];
    ThreadData threadData[rows1];

    int rows = rows1;
    int cols = cols2;

    for (int i = 0; i < rows; ++i)
    {
        threadData[i].start_row = i;
        threadData[i].end_row = i + 1;
        threadData[i].cols1 = cols1;
        threadData[i].cols2 = cols2;
        threadData[i].mat1 = mat1;
        threadData[i].mat2 = mat2;
        threadData[i].result = result;

        pthread_create(&threads[i], nullptr, multiplyRowsThread, &threadData[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < rows; ++i)
    {
        pthread_join(threads[i], nullptr);
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto elapsed_time = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();

    // Output the result matrix and additional information
    printMatrix(result, rows, cols);
    cout << "END2 " << elapsed_time << " microseconds" << endl;
    cout << "-----------------------------------" << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <matrix-file>" << endl;
        return 1;
    }

    int mat1[MAX_ROWS][MAX_COLS];
    int mat2[MAX_ROWS][MAX_COLS];
    int result[MAX_ROWS][MAX_COLS];
    int rows1, cols1, rows2, cols2;

    // Read both matrices from the file provided as a command-line argument
    if (!readMatricesFromFile(argv[1], mat1, rows1, cols1, mat2, rows2, cols2))
    {
        return 1;
    }

    // Check if matrices can be multiplied
    if (cols1 != rows2)
    {
        cerr << "Error: Matrices cannot be multiplied. Columns of the first matrix must be equal to rows of the second matrix." << endl;
        return 1;
    }

    // Multiply matrices using each element in a thread
    multiplyElements(mat1, mat2, result, rows1, cols1, rows2, cols2);

    // Multiply matrices using each row in a thread
    multiplyRows(mat1, mat2, result, rows1, cols1, rows2, cols2);

    return 0;
}