#include <iostream>

// Logic check that returns true or false for a single value
typedef bool (*Predicate)(int);

// Determines if two values should be swapped based on specific logic
typedef bool (*Comparator)(int, int);

bool isPositive(int value) {
    return value > 0;
}

bool isEven(int value) {
    return (value % 2) == 0;
}

// Returns true if first value is greater than second (for ascending sort)
bool compareAsc(int a, int b) {
    return a > b;
}

// Returns true if first value is less than second (for descending sort)
bool compareDesc(int a, int b) {
    return a < b;
}

void extractValidData(int*& ptr, int& size, Predicate pred) {
    if (!ptr || size <= 0) {
        return;
    }

    int validCount = 0;

    for (int i = 0; i < size; ++i) {
        if (pred(ptr[i])) {
            ++validCount;
        }
    }

    if (validCount == 0) {
        delete[] ptr;
        ptr = nullptr;
        size = 0;

        return;
    }

    int* newData = new(std::nothrow) int[validCount];

    if (!newData) {
        return;
    }

    int currentIndex = 0;

    for (int i = 0; i < size; ++i) {
        if (pred(ptr[i])) {
            newData[currentIndex++] = ptr[i];
        }
    }

    // Clean up old heap memory and update metadata
    delete[] ptr;
    ptr = newData;
    size = validCount;
}

void sortArray(int* arr, int size, Comparator comp) {
    if (!arr || size <= 1) {
        return;
    }

    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - i - 1; ++j) {
            // Decision injected via function pointer
            if (comp(arr[j], arr[j + 1])) {
                int temp = arr[j];

                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

int main() {
    int n;
    std::cout << "Enter number of elements: ";

    if (!(std::cin >> n) || n <= 0) {
        return 1;
    }

    int* data = new(std::nothrow) int[n];

    if (!data) {
        return 1;
    }

    std::cout << "Enter " << n << " integers: ";

    for (int i = 0; i < n; ++i) {
        if (!(std::cin >> data[i])) {
            delete[] data;

            return 1;
        }
    }

    std::cout << "Filtering positive numbers...\n";

    extractValidData(data, n, isPositive);

    if (n == 0 || !data) {
        std::cout << "No valid data found.\n";

        return 0;
    }

    // Setup sorting strategies
    Comparator myStrategies[] = { compareAsc, compareDesc };

    std::cout << "Choose sort order (0 - ascending, 1 - descending): ";

    int choice;

    if (!(std::cin >> choice) || (choice < 0 || choice > 1)) {
        std::cerr << "Invalid choice.\n";

        delete[] data;

        return 1;
    }

    // Apply the selected sorting logic
    sortArray(data, n, myStrategies[choice]);

    std::cout << "Processed and sorted array: ";

    for (int i = 0; i < n; ++i) {
        std::cout << data[i] << " ";
    }

    std::cout << "\n";

    delete[] data;

    return 0;
}