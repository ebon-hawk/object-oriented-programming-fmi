// For rand() and srand()
#include <cstdlib>

// For time() seeding
#include <ctime>

// For standard I/O only
#include <iostream>

const unsigned int FLAG_READY = 1 << 0;
const unsigned int FLAG_BUSY = 1 << 1;
const unsigned int FLAG_ERROR = 1 << 2;

void setFlag(unsigned int& reg, unsigned int mask) {
    reg |= mask;
}

void clearFlag(unsigned int& reg, unsigned int mask) {
    reg &= (~mask);
}

// Function pointer type definition
typedef bool (*CheckFunc)(unsigned int);

bool isBroken(unsigned int reg) {
    return (reg & FLAG_ERROR) != 0;
}

bool isAvailable(unsigned int reg) {
    return (reg & FLAG_READY) && !(reg & FLAG_BUSY);
}

void processRegisters(unsigned int* registers, unsigned int size, CheckFunc check) {
    for (unsigned int i = 0; i < size; ++i) {
        if (check(registers[i])) {
            std::cout << i << " ";
        }
    }

    std::cout << "\n";
}

int main() {
    // Seed the randomizer once at startup
    srand((unsigned int)time(0));

    unsigned int regCnt = 0;

    std::cout << "Enter number of registers: ";

    // Basic validation to ensure we have a valid count
    if (!(std::cin >> regCnt) || regCnt == 0) {
        std::cout << "Invalid count.\n";

        return 1;
    }

    // Manual dynamic memory allocation
    unsigned int* registers = new(std::nothrow) unsigned int[regCnt];

    if (!registers) {
        std::cout << "Allocation failed.\n";

        return 1;
    }

    // Simulation loop
    for (unsigned int i = 0; i < regCnt; ++i) {
        // Ensure fresh start
        registers[i] = 0;

        int roll = rand() % 100;

        if (roll < 20) {
            setFlag(registers[i], FLAG_ERROR);
        }
        else if (roll < 60) {
            setFlag(registers[i], FLAG_BUSY);
        }
        else {
            setFlag(registers[i], FLAG_READY);

            clearFlag(registers[i], FLAG_BUSY);
        }
    }

    std::cout << "Choose check (0 - available, 1 - broken): ";

    int choice;

    if (!(std::cin >> choice) || (choice < 0 || choice > 1)) {
        std::cout << "Invalid choice.\n";

        // Clean up before exit
        delete[] registers;

        return 1;
    }

    // Map choice to function pointer
    CheckFunc activeCheck = (choice == 0) ? isAvailable : isBroken;

    std::cout << "Matching indices: ";

    processRegisters(registers, regCnt, activeCheck);

    // Manual memory cleanup
    delete[] registers;

    return 0;
}