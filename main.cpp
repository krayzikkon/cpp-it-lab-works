#include <iostream>
#include <cstdlib>
#include <string>

// Task enumeration for compile-time task selection
enum class TaskID {
    TASK_1 = 1,
    TASK_2 = 2,
    TASK_3 = 3,
    TASK_4 = 4,
    TASK_5 = 5,
    TASK_6 = 6
};

// Current task to execute - change this value to switch tasks
constexpr TaskID CURRENT_TASK = TaskID::TASK_1;

using namespace std;

/**
 * @brief Clears the console screen in a cross-platform manner.
 * Uses preprocessor directives to handle Windows (cls) and Unix-like systems (clear).
 *
 * @note Consider using platform-specific libraries for production code instead of system()
 *       to improve security and reliability.
 */
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/**
 * @brief Pauses console execution and waits for user input.
 * Clears the input buffer before waiting for Enter key press.
 */
void pauseConsole() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Forward declarations for all task functions
void task1();
void task2();
void task3();
void task4();
void task5();
void task6();

/**
 * @brief Executes the currently selected task based on CURRENT_TASK constant.
 * Uses compile-time if-constexpr for zero runtime overhead.
 *
 * @return int Exit code (0 for success)
 */
int main() {
    clearScreen();

    // Execute selected task based on compile-time constant
    if constexpr (CURRENT_TASK == TaskID::TASK_1) {
        cout << "EXECUTING TASK 1" << endl;
        task1();
    }
    else if constexpr (CURRENT_TASK == TaskID::TASK_2) {
        cout << "EXECUTING TASK 2" << endl;
        task2();
    }
    else if constexpr (CURRENT_TASK == TaskID::TASK_3) {
        cout << "EXECUTING TASK 3" << endl;
        task3();
    }
    else if constexpr (CURRENT_TASK == TaskID::TASK_4) {
        cout << "EXECUTING TASK 4" << endl;
        task4();
    }
    else if constexpr (CURRENT_TASK == TaskID::TASK_5) {
        cout << "EXECUTING TASK 5" << endl;
        task5();
    }
    else if constexpr (CURRENT_TASK == TaskID::TASK_6) {
        cout << "EXECUTING TASK 6" << endl;
        task6();
    }
    else {
        cerr << "NO TASK SELECTED OR INVALID ID" << endl;
        return 1;
    }

    pauseConsole();
    return 0;
}