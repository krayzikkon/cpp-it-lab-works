#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <array>
#include <stdexcept>
#include <functional>

using namespace std;

// Matrix dimensions as constexpr for compile-time usage
constexpr int ROWS = 4;
constexpr int COLS = 3;

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

/**
 * @brief Type alias for 2D matrix with dimensions ROWS x COLS.
 */
using Matrix = array<array<double, COLS>, ROWS>;

/**
 * @brief Reads a matrix from an input file.
 * @param input Input stream to read from
 * @param matrix Reference to matrix to populate
 * @throws runtime_error if read operation fails
 */
void readMatrix(ifstream& input, Matrix& matrix) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            input >> matrix[i][j];
            if (!input) {
                throw runtime_error("Error reading matrix data from file");
            }
        }
    }
}

/**
 * @brief Displays a matrix with formatted output.
 * Outputs matrix to both console and file with proper spacing and precision.
 * @param matrix Matrix to display
 * @param output DualOutputWriter for simultaneous console and file output
 */
void displayMatrix(const Matrix& matrix, DualOutputWriter& output) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            output << setw(8) << fixed << setprecision(2) << matrix[i][j];
        }
        output << "\n";
    }
}

/**
 * @brief Applies a binary operation to two matrices element-wise.
 * Stores the result in a third matrix.
 * @param a First operand matrix
 * @param b Second operand matrix
 * @param result Output matrix for results
 * @param op Binary operation function (e.g., std::plus, std::minus)
 * @param op_name Name of operation for error messages
 */
void applyElementWiseOperation(const Matrix& a, const Matrix& b, Matrix& result,
    function<double(double, double)> op,
    const string& op_name) {
    try {
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                result[i][j] = op(a[i][j], b[i][j]);
            }
        }
    }
    catch (const exception& e) {
        throw runtime_error("Error during " + op_name + " operation: " + e.what());
    }
}

/**
 * @brief Computes element-wise maximum of two matrices.
 * @param a First matrix
 * @param b Second matrix
 * @param result Output matrix containing maximum of corresponding elements
 */
void maxElementWise(const Matrix& a, const Matrix& b, Matrix& result) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            result[i][j] = (a[i][j] > b[i][j]) ? a[i][j] : b[i][j];
        }
    }
}

/**
 * @brief Transposes a matrix from ROWS×COLS to COLS×ROWS.
 * Note: Returns a different size type; only works when source is 4×3 (result: 3×4).
 * @param matrix Source matrix
 * @param transposed Output transposed matrix
 */
void transposeMatrix(const Matrix& matrix, array<array<double, ROWS>, COLS>& transposed) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            transposed[j][i] = matrix[i][j];
        }
    }
}

/**
 * @brief Executes matrix arithmetic operations.
 * Demonstrates element-wise operations (addition, subtraction, multiplication, division),
 * element-wise maximum, and matrix transposition.
 * All results are logged to both console and file.
 *
 * Reads two 4×3 matrices from input file and performs:
 * - Arithmetic operations: +, -, *, /
 * - Element-wise maximum comparison
 * - Matrix transposition (4×3 → 3×4)
 */
void task3() {
    try {
        // Open and validate input file
        ifstream input("input_task3.txt");
        if (!input.is_open()) {
            throw runtime_error("Input file 'input_task3.txt' not found");
        }

        // Read matrices from input file
        Matrix a, b;
        readMatrix(input, a);
        readMatrix(input, b);
        input.close();

        // Initialize output writer
        DualOutputWriter output("output_task3.txt");

        // --- Display Input Matrices ---
        output << "--- Array 1 ---\n";
        displayMatrix(a, output);

        output << "\n--- Array 2 ---\n";
        displayMatrix(b, output);

        // --- Perform Element-Wise Arithmetic Operations ---
        Matrix result;

        // Addition
        output << "\n--- Sum (+) ---\n";
        applyElementWiseOperation(a, b, result,
            [](double x, double y) { return x + y; }, "addition");
        displayMatrix(result, output);

        // Subtraction
        output << "\n--- Diff (-) ---\n";
        applyElementWiseOperation(a, b, result,
            [](double x, double y) { return x - y; }, "subtraction");
        displayMatrix(result, output);

        // Multiplication
        output << "\n--- Mult (*) ---\n";
        applyElementWiseOperation(a, b, result,
            [](double x, double y) { return x * y; }, "multiplication");
        displayMatrix(result, output);

        // Division with zero-check
        output << "\n--- Div (/) ---\n";
        applyElementWiseOperation(a, b, result,
            [](double x, double y) {
                return (y != 0.0) ? x / y : 0.0;
            }, "division");
        displayMatrix(result, output);

        // --- Element-Wise Maximum ---
        output << "\n--- Max Elements ---\n";
        maxElementWise(a, b, result);
        displayMatrix(result, output);

        // --- Transposed Max Array (3x4) ---
        output << "\n--- Transposed Max Array (3x4) ---\n";
        array<array<double, ROWS>, COLS> transposed;
        transposeMatrix(result, transposed);
        for (int j = 0; j < COLS; ++j) {
            for (int i = 0; i < ROWS; ++i) {
                output << setw(8) << fixed << setprecision(2) << transposed[j][i];
            }
            output << "\n";
        }

        output << "\nTask 3 completed. Results saved to 'output_task3.txt'\n";

    }
    catch (const runtime_error& e) {
        cerr << "Task 3 Error: " << e.what() << endl;
    }
    catch (const exception& e) {
        cerr << "Unexpected error: " << e.what() << endl;
    }
}