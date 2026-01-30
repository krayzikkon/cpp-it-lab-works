#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>

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
            throw runtime_error("Error opening output file: " + filepath);
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
     * @param manip Stream manipulator function
     * @return Reference to this object for method chaining
     */
    DualOutputWriter& operator<<(ostream& (*manip)(ostream&)) {
        cout << manip;
        file_ << manip;
        return *this;
    }
};

/**
 * @brief Executes arithmetic operations on two user-provided values.
 * Demonstrates basic arithmetic, division-by-zero handling, and increment/decrement operations.
 * All results are logged to both console and file using DualOutputWriter.
 *
 * @note The increment/decrement demonstrations use temporary variables to preserve
 *       original input values for accurate documentation.
 */
void task1() {
    try {
        // Initialize dual output writer with automatic file management
        DualOutputWriter output("output_task1.txt");

        // Retrieve user input with validation
        double a, b;
        cout << "Enter value A: ";
        cin >> a;
        cout << "Enter value B: ";
        cin >> b;

        // Validate input stream state
        if (!cin) {
            throw runtime_error("Invalid input: Please enter valid numeric values");
        }

        // --- Arithmetic Operations Section ---
        output << "\n--- Arithmetic Operations ---\n";

        output << "A + B = " << (a + b) << "\n";
        output << "A - B = " << (a - b) << "\n";
        output << "A * B = " << (a * b) << "\n";

        // Division with explicit zero-check for safety
        if (b != 0.0) {
            output << "A / B = " << (a / b) << "\n";
        }
        else {
            output << "A / B = Error (Division by zero)\n";
        }

        // --- Increment/Decrement Operations Section ---
        output << "\n--- Increment/Decrement (Postfix) ---\n";

        // Demonstrate postfix increment on temporary copies
        double temp_a = a;
        output << "A++: " << temp_a++ << " (now: " << temp_a << ")\n";

        double temp_b = b;
        output << "B++: " << temp_b++ << " (now: " << temp_b << ")\n";

        temp_a = a;
        output << "A--: " << temp_a-- << " (now: " << temp_a << ")\n";

        temp_b = b;
        output << "B--: " << temp_b-- << " (now: " << temp_b << ")\n";

        // Success message
        output << "\nResults saved to output_task1.txt\n";

    }
    catch (const runtime_error& e) {
        cerr << "Task 1 Error: " << e.what() << endl;
    }
    catch (const exception& e) {
        cerr << "Unexpected error: " << e.what() << endl;
    }
}