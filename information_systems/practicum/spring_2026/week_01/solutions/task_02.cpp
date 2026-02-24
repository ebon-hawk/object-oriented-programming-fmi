#include <iostream>

typedef void (*TransformFunc)(unsigned char&);

// Applies mask 0xAA (10101010 in binary)
void encryptXOR(unsigned char& data) {
    data ^= 0xAA;
}

// Flips every 0 to 1 and every 1 to 0 across the 8-bit block
void invertBits(unsigned char& data) {
    data = ~data;
}

/*
 * A 'nibble' is 4 bits. This function swaps the High Nibble with the Low Nibble.
 *
 * Process:
 * - Isolate bits 7-4 and shift them to positions 3-0: (data & 0xF0) >> 4
 * - Isolate bits 3-0 and shift them to positions 7-4: (data & 0x0F) << 4
 * - Merge into the final 8-bit result: leftPart | rightPart
 */
void swapNibbles(unsigned char& data) {
    // Mask 1111 0000
    unsigned char leftPart = (data & 0xF0) >> 4;

    // Mask 0000 1111
    unsigned char rightPart = (data & 0x0F) << 4;

    data = leftPart | rightPart;
}

void applyTransformation(unsigned char* arr, int n, TransformFunc func) {
    for (int i = 0; i < n; ++i) {
        // Executes the pointed-to function
        func(arr[i]);
    }
}

// Helper to print data as integers (since 'char' prints as ASCII symbols)
void printData(unsigned char* arr, int n) {
    for (int i = 0; i < n; ++i) {
        std::cout << (int)arr[i] << " ";
    }

    std::cout << "\n";
}

int main() {
    int n;
    std::cout << "Enter number of bytes: ";

    if (!(std::cin >> n) || n <= 0) {
        std::cout << "Invalid array size.\n";

        return 1;
    }

    // Dynamic allocation for N bytes (8-bit unsigned integers)
    unsigned char* data = new(std::nothrow) unsigned char[n];

    if (!data) {
        std::cout << "Memory allocation failed.\n";

        return 1;
    }

    // Initialize array
    for (int i = 0; i < n; ++i) {
        data[i] = (unsigned char)(i % 256);
    }

    std::cout << "Original data: ";

    printData(data, n);

    // Mode selection
    std::cout << "Choose transformation (0 - encrypt, 1 - invert, 2 - swap): ";

    int choice;

    if (!(std::cin >> choice)) {
        // Clean up before exit
        delete[] data;

        return 1;
    }

    TransformFunc selectedFunc;

    switch (choice) {
    case 0: selectedFunc = encryptXOR; break;
    case 1: selectedFunc = invertBits; break;
    case 2: selectedFunc = swapNibbles; break;
    default:
        std::cout << "Invalid selection.\n";

        delete[] data;

        return 1;
    }

    // Process the data through the engine
    applyTransformation(data, n, selectedFunc);

    std::cout << "Data after transformation: ";

    printData(data, n);

    // Clean up heap memory
    delete[] data;

    return 0;
}