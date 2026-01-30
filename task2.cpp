#include <iostream>
#include <fstream>
#include <stdexcept>
#include <iomanip>
#include <limits>

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

/**
 * @brief Reads the initial value (A0) from input file.
 * @param filepath Path to the input file
 * @return The initial value A0
 * @throws runtime_error if file cannot be opened or contains invalid data
 */
double readInitialValue(const string& filepath) {
    ifstream inputFile(filepath);
    if (!inputFile.is_open()) {
        throw runtime_error("Input file '" + filepath + "' not found.\n"
            "Please create the file with the A0 value inside.");
    }

    double a0;
    inputFile >> a0;

    if (!inputFile) {
        throw runtime_error("Invalid data in input file: expected numeric A0 value.");
    }

    return a0;
}

/**
 * @brief Calculates arithmetic sequence statistics.
 * @param a0 Initial term
 * @param d Common difference
 * @param n Number of terms
 * @param output Dual output writer for console and file
 * @param term_limit Optional limit for cumulative sum (0 = no limit)
 * @return Number of terms generated
 */
int generateSequence(double a0, double d, int n, DualOutputWriter& output,
    double term_limit = 0.0) {
    int count = 0;
    double sum = 0.0;

    for (int i = 1; i <= n; ++i) {
        double term = a0 + (i - 1) * d;

        // Check sum limit if specified
        if (term_limit > 0 && sum + term >= term_limit) {
            break;
        }

        output << term << " ";
        sum += term;
        count++;
    }

    // Output statistics
    double average = (count > 0) ? sum / count : 0.0;
    output << "\nSum: " << sum << "\nAverage: " << average << "\n";

    return count;
}

/**
 * @brief Executes arithmetic sequence calculations using three loop types.
 * Demonstrates for-loop, while-loop, and do-while loop implementations.
 * All results are logged to both console and file.
 *
 * Reads initial value (A0) from file, then generates arithmetic sequences
 * with specified number of terms and common difference using different loop structures.
 */
void task2() {
    try {
        // Read initial value from input file
        double a0 = readInitialValue("input_task2.txt");

        // Get user input with validation
        int n;
        double d;

        cout << "Enter n (number of terms): ";
        cin >> n;
        cout << "Enter d (common difference): ";
        cin >> d;

        if (!cin) {
            throw runtime_error("Invalid input: Please enter valid numeric values");
        }

        if (n < 0) {
            throw runtime_error("Error: 'n' cannot be negative");
        }

        // Initialize dual output writer
        DualOutputWriter output("output_task2.txt");

        // --- PART 1: FOR LOOP ---
        output << "=== PART 1: FOR LOOP ===\nSequence terms: ";

        double sum = 0.0;
        for (int i = 1; i <= n; ++i) {
            double term = a0 + (i - 1) * d;
            output << term << " ";
            sum += term;
        }

        double average = (n > 0) ? sum / n : 0.0;
        output << "\nSum: " << sum << "\nAverage: " << average << "\n";

        // --- PART 2: WHILE LOOP ---
        output << "\n=== PART 2: WHILE LOOP ===\nSequence terms: ";

        sum = 0.0;
        int i = 1;
        while (i <= n) {
            double term = a0 + (i - 1) * d;
            output << term << " ";
            sum += term;
            ++i;
        }

        int count = i - 1;
        average = (count > 0) ? sum / count : 0.0;
        output << "\nSum: " << sum << "\nAverage: " << average << "\n";

        // --- PART 3: DO...WHILE LOOP ---
        output << "\n=== PART 3: DO...WHILE LOOP ===\n";

        constexpr double SUM_LIMIT = 120.0;
        output << "Sequence terms (sum < " << SUM_LIMIT << "): ";

        sum = 0.0;
        i = 1;
        count = 0;

        if (n > 0) {
            do {
                if (i > n) break;

                double term = a0 + (i - 1) * d;

                // Stop if adding next term exceeds limit
                if (sum + term >= SUM_LIMIT) break;

                output << term << " ";
                sum += term;
                count++;
                ++i;
            } while (true);
        }

        average = (count > 0) ? sum / count : 0.0;
        output << "\nSum: " << sum << "\nAverage: " << average << "\n";

        output << "\nResults saved to output_task2.txt\n";

    }
    catch (const runtime_error& e) {
        cerr << "Task 2 Error: " << e.what() << endl;
    }
    catch (const exception& e) {
        cerr << "Unexpected error: " << e.what() << endl;
    }
}