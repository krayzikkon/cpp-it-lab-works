#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
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
 * @brief Type alias for integer vector for cleaner code.
 */
using IntVector = vector<int>;

/**
 * @brief Prints an array/vector with a descriptive label to output stream.
 * Formats output with proper spacing and field width for readability.
 *
 * @param arr Container with integer elements
 * @param label Descriptive label for the output
 * @param output Reference to output stream (or DualOutputWriter)
 */
template<typename Container>
void printArray(const Container& arr, const string& label, DualOutputWriter& output) {
    output << label << ": ";
    for (const auto& val : arr) {
        output << setw(4) << val << " ";
    }
    output << "\n";
}

/**
 * @brief Reads integers from an input file into a vector.
 * Automatically resizes vector to accommodate all values in file.
 *
 * @param filepath Path to the input file
 * @return Vector containing all integers read from file
 * @throws runtime_error if file cannot be opened or is empty
 */
IntVector readFromFile(const string& filepath) {
    ifstream input(filepath);
    if (!input.is_open()) {
        throw runtime_error("Input file '" + filepath + "' not found");
    }

    IntVector data;
    int value;
    while (input >> value) {
        data.push_back(value);
    }

    if (data.empty()) {
        throw runtime_error("Input file is empty");
    }

    input.close();
    return data;
}

/**
 * @brief Searches for all occurrences of a key in a container.
 * Performs linear search to find all matching indices.
 *
 * @param arr Container to search
 * @param key Value to search for
 * @param output DualOutputWriter for result output
 * @return Number of occurrences found
 */
template<typename Container>
int searchAndPrint(const Container& arr, int key, DualOutputWriter& output) {
    vector<int> positions;

    for (int i = 0; i < static_cast<int>(arr.size()); ++i) {
        if (arr[i] == key) {
            positions.push_back(i);
        }
    }

    output << "Found at: ";
    if (!positions.empty()) {
        for (int pos : positions) {
            output << pos << " ";
        }
    }
    else {
        output << "Not found";
    }
    output << "\n";

    return positions.size();
}

/**
 * @brief Executes array sorting and searching operations.
 * Demonstrates selection sort implementation and linear search functionality.
 *
 * Process:
 * 1. Reads integers from input file
 * 2. Prompts user for array size (up to file size)
 * 3. Maintains both original and sorted copies
 * 4. Performs linear search for user-specified key
 * 5. Logs all operations to both console and file
 *
 * @note Uses std::vector for automatic memory management instead of manual new/delete.
 * @note Uses std::sort from STL for production code; selection sort shown for educational purposes.
 */
void task4() {
    try {
        // Read data from input file
        IntVector fileData = readFromFile("input_task4.txt");
        int count = fileData.size();

        // Prompt user for array size
        int n;
        cout << "File has " << count << " numbers. Enter n to use: ";
        cin >> n;

        if (!cin) {
            throw runtime_error("Invalid input: please enter a valid integer");
        }

        // Limit n to available data
        n = min(n, count);

        if (n <= 0) {
            throw runtime_error("Invalid n: must be greater than 0");
        }

        // Create vectors with subset of data
        IntVector original(fileData.begin(), fileData.begin() + n);
        IntVector sorted = original;  // Copy for sorting

        // Initialize dual output writer
        DualOutputWriter output("output_task4.txt");

        // --- Display Original Array ---
        output << "========== ARRAY OPERATIONS ==========\n";
        printArray(original, "Original", output);

        // --- Sort and Display Sorted Array ---
        // Using std::sort for production code (more efficient than selection sort)
        // Selection sort kept as comment for educational demonstration:
        // selectionSort(sorted, n);
        std::sort(sorted.begin(), sorted.end());

        printArray(sorted, "Sorted", output);

        // --- Search for User-Specified Key ---
        int key;
        cout << "Search key: ";
        cin >> key;

        if (!cin) {
            throw runtime_error("Invalid input: please enter a valid integer");
        }

        output << "Search key: " << key << "\n";
        int occurrences = searchAndPrint(original, key, output);

        // --- Summary Statistics ---
        output << "========== STATISTICS ==========\n";
        output << "Array size: " << n << "\n";
        output << "Occurrences found: " << occurrences << "\n";

    }
    catch (const runtime_error& e) {
        cerr << "Task 4 Error: " << e.what() << endl;
    }
    catch (const exception& e) {
        cerr << "Unexpected error: " << e.what() << endl;
    }
}

/**
 * @brief Selection sort implementation for educational purposes.
 * Demonstrates sorting algorithm with O(n²) time complexity.
 * Note: For production code, prefer std::sort which is more efficient.
 *
 * @param arr Array to sort
 * @param n Size of array
 */
void selectionSort(int arr[], int n) {
    for (int i = 0; i < n - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < n; ++j) {
            if (arr[j] < arr[minIdx]) {
                minIdx = j;
            }
        }
        swap(arr[i], arr[minIdx]);
    }
}