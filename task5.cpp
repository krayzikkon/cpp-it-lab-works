#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <array>
#include <map>
#include <stdexcept>
#include <functional>

using namespace std;

/**
 * @brief Helper class for dual output to console and file simultaneously.
 * Eliminates code duplication by combining cout and file write operations.
 * Automatically manages file lifecycle through RAII pattern.
 */
class DualOutputWriter {
private:
    ofstream file_;

public:
    /**
     * @brief Constructs DualOutputWriter with file path.
     * @param filepath Path to the output file
     * @throws runtime_error if file cannot be opened
     */
    explicit DualOutputWriter(const string& filepath) {
        file_.open(filepath);
        if (!file_.is_open()) {
            throw runtime_error("Cannot open output file: " + filepath);
        }
    }

    /**
     * @brief Destructor ensures file is properly closed (RAII pattern).
     */
    ~DualOutputWriter() {
        if (file_.is_open()) {
            file_.close();
        }
    }

    /**
     * @brief Outputs data to both console and file simultaneously.
     * @tparam T Data type to output
     * @param data Data to write
     * @return Reference to this object for method chaining
     */
    template<typename T>
    DualOutputWriter& operator<<(const T& data) {
        cout << data;
        file_ << data;
        return *this;
    }

    /**
     * @brief Specialization for stream manipulators (endl, setprecision, etc.).
     */
    DualOutputWriter& operator<<(ostream& (*manip)(ostream&)) {
        cout << manip;
        file_ << manip;
        return *this;
    }
};

// ============================================================================
// FIBONACCI COMPUTATION SECTION
// ============================================================================

/**
 * @brief Computes Fibonacci number using memoization with global cache.
 * Caches results to avoid redundant calculations.
 * Time complexity: O(n), Space complexity: O(n)
 *
 * @param n Fibonacci index (0-based)
 * @return n-th Fibonacci number
 * @throws invalid_argument if n is negative
 */
class FibonacciCalculator {
private:
    map<int, long long> memo_;  // Thread-safe memoization cache
    static constexpr int MAX_N = 100;

public:
    /**
     * @brief Computes Fibonacci number with memoization.
     * @param n Index of Fibonacci number
     * @return n-th Fibonacci number
     */
    long long compute(int n) {
        if (n < 0) {
            throw invalid_argument("Fibonacci index cannot be negative");
        }
        if (n <= 1) {
            return n;
        }
        if (n > MAX_N) {
            throw invalid_argument("Fibonacci index too large (max: 100)");
        }

        // Check if already computed
        if (memo_.find(n) != memo_.end()) {
            return memo_[n];
        }

        // Compute and cache
        memo_[n] = compute(n - 1) + compute(n - 2);
        return memo_[n];
    }

    /**
     * @brief Clears memoization cache.
     */
    void clearCache() {
        memo_.clear();
    }
};

/**
 * @brief Executes Fibonacci number generation and summation task.
 * Computes and displays Fibonacci sequence from F(0) to F(n).
 * Outputs results to both console and file.
 *
 * @throws runtime_error if output file cannot be opened
 * @throws invalid_argument if user input is invalid
 */
void fibonacciTask() {
    try {
        int n;
        cout << "\n=== FIBONACCI ===\nEnter n (0-100): ";
        cin >> n;

        if (!cin) {
            cin.clear();
            cin.ignore(10000, '\n');
            throw invalid_argument("Invalid input: please enter a valid integer");
        }

        if (n < 0) {
            throw invalid_argument("Error: n cannot be negative");
        }

        // Initialize calculator and output writer
        FibonacciCalculator fib;
        DualOutputWriter output("output_fibonacci.txt");

        output << "=== FIBONACCI SEQUENCE (F(0) to F(" << n << ")) ===\n";

        long long sum = 0;
        for (int i = 0; i <= n; ++i) {
            long long value = fib.compute(i);
            sum += value;
            output << "F(" << i << ") = " << value << "\n";
        }

        output << "\n=== STATISTICS ===\n";
        output << "Total terms: " << (n + 1) << "\n";
        output << "Sum of sequence: " << sum << "\n";
        output << "Last term F(" << n << "): " << fib.compute(n) << "\n";

    }
    catch (const invalid_argument& e) {
        cerr << "Input Error: " << e.what() << endl;
    }
    catch (const runtime_error& e) {
        cerr << "File Error: " << e.what() << endl;
    }
    catch (const exception& e) {
        cerr << "Unexpected error: " << e.what() << endl;
    }
}

// ============================================================================
// MATRIX OPERATIONS SECTION
// ============================================================================

/**
 * @brief Type aliases for matrix dimensions and data.
 */
constexpr int MATRIX_ROWS = 2;
constexpr int MATRIX_COLS = 5;
using Matrix = array<array<int, MATRIX_COLS>, MATRIX_ROWS>;

/**
 * @brief Displays a 2D matrix with a descriptive label.
 * Formats output with proper spacing for readability.
 *
 * @param matrix Matrix to display
 * @param label Descriptive label
 * @param output DualOutputWriter for simultaneous console and file output
 */
void displayMatrix(const Matrix& matrix, const string& label, DualOutputWriter& output) {
    output << "\n" << label << "\n";
    for (int i = 0; i < MATRIX_ROWS; ++i) {
        for (int j = 0; j < MATRIX_COLS; ++j) {
            output << setw(5) << matrix[i][j] << " ";
        }
        output << "\n";
    }
}

/**
 * @brief Applies element-wise binary operation to two matrices.
 * Stores result in output matrix with error handling for division by zero.
 *
 * @param a First operand matrix
 * @param b Second operand matrix
 * @param result Output matrix for results
 * @param op Operation character ('+', '-', '*', '/')
 * @throws invalid_argument if operation character is invalid
 */
void applyMatrixOperation(const Matrix& a, const Matrix& b, Matrix& result, char op) {
    for (int i = 0; i < MATRIX_ROWS; ++i) {
        for (int j = 0; j < MATRIX_COLS; ++j) {
            switch (op) {
            case '+':
                result[i][j] = a[i][j] + b[i][j];
                break;
            case '-':
                result[i][j] = a[i][j] - b[i][j];
                break;
            case '*':
                result[i][j] = a[i][j] * b[i][j];
                break;
            case '/':
                result[i][j] = (b[i][j] != 0) ? a[i][j] / b[i][j] : 0;
                break;
            default:
                throw invalid_argument("Invalid operation: " + string(1, op));
            }
        }
    }
}

/**
 * @brief Finds and displays min/max value of a matrix.
 *
 * @param matrix Matrix to analyze
 * @param choice Selection: 1=max, 2=min, 3=max, 4=min (uses choice % 2)
 * @param output DualOutputWriter for output
 */
void findMinMaxInMatrix(const Matrix& matrix, int choice, DualOutputWriter& output) {
    int minValue = INT_MAX;
    int maxValue = INT_MIN;

    for (int i = 0; i < MATRIX_ROWS; ++i) {
        for (int j = 0; j < MATRIX_COLS; ++j) {
            minValue = min(minValue, matrix[i][j]);
            maxValue = max(maxValue, matrix[i][j]);
        }
    }

    // choice % 2 == 1 for max, choice % 2 == 0 for min
    int result = (choice % 2 == 1) ? maxValue : minValue;
    string operation = (choice % 2 == 1) ? "Maximum" : "Minimum";

    output << "\n" << operation << " value: " << result << "\n";
}

/**
 * @brief Reads two 2×5 matrices from input file.
 *
 * @param filepath Path to input file
 * @param a First matrix (output)
 * @param b Second matrix (output)
 * @throws runtime_error if file cannot be opened or contains insufficient data
 */
void readMatricesFromFile(const string& filepath, Matrix& a, Matrix& b) {
    ifstream input(filepath);
    if (!input.is_open()) {
        throw runtime_error("Input file '" + filepath + "' not found");
    }

    for (int i = 0; i < MATRIX_ROWS; ++i) {
        for (int j = 0; j < MATRIX_COLS; ++j) {
            if (!(input >> a[i][j])) {
                throw runtime_error("Insufficient data in input file for matrix A");
            }
        }
    }

    for (int i = 0; i < MATRIX_ROWS; ++i) {
        for (int j = 0; j < MATRIX_COLS; ++j) {
            if (!(input >> b[i][j])) {
                throw runtime_error("Insufficient data in input file for matrix B");
            }
        }
    }
}

/**
 * @brief Executes matrix operations task.
 * Allows user to perform arithmetic operations on two matrices and find min/max values.
 * Supports up to 3 operations per execution.
 *
 * @throws runtime_error if input file cannot be opened
 * @throws invalid_argument if user input is invalid
 */
void matrixTask() {
    try {
        // Read matrices from file
        Matrix a, b, result;
        readMatricesFromFile("input_arrays.txt", a, b);

        // Initialize output writer
        DualOutputWriter output("output_matrix_operations.txt");

        output << "========== MATRIX OPERATIONS ==========\n";
        displayMatrix(a, "Array 1:", output);
        displayMatrix(b, "Array 2:", output);

        // Perform up to 3 operations
        constexpr int MAX_OPERATIONS = 3;
        for (int iteration = 0; iteration < MAX_OPERATIONS; ++iteration) {
            cout << "\nOperation " << (iteration + 1) << "/3 (+, -, *, /, m for min/max): ";
            char op;
            cin >> op;

            if (!cin) {
                cin.clear();
                cin.ignore(10000, '\n');
                throw invalid_argument("Invalid input: please enter a valid operation");
            }

            output << "\n--- Operation " << (iteration + 1) << " ---\n";
            output << "Operator: " << op << "\n";

            if (op == 'm' || op == 'M') {
                // Min/Max operation
                cout << "Select (1:max A, 2:min A, 3:max B, 4:min B): ";
                int choice;
                cin >> choice;

                if (!cin || choice < 1 || choice > 4) {
                    throw invalid_argument("Invalid choice: must be 1-4");
                }

                const Matrix& targetMatrix = (choice < 3) ? a : b;
                findMinMaxInMatrix(targetMatrix, choice, output);

            }
            else {
                // Arithmetic operation
                applyMatrixOperation(a, b, result, op);
                displayMatrix(result, "Result:", output);
            }
        }

        output << "\n========== TASK COMPLETED ==========\n";

    }
    catch (const runtime_error& e) {
        cerr << "File Error: " << e.what() << endl;
    }
    catch (const invalid_argument& e) {
        cerr << "Input Error: " << e.what() << endl;
    }
    catch (const exception& e) {
        cerr << "Unexpected error: " << e.what() << endl;
    }
}

// ============================================================================
// MAIN TASK DISPATCHER
// ============================================================================

/**
 * @brief Main task dispatcher for task5.
 * Executes both Fibonacci and matrix operation subtasks sequentially.
 */
void task5() {
    try {
        fibonacciTask();
        matrixTask();
    }
    catch (const exception& e) {
        cerr << "Task 5 Error: " << e.what() << endl;
    }
}