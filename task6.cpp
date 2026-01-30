#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <sstream>

using namespace std;

/**
 * @brief Represents a student record with academic information.
 * Contains all necessary fields for student database management.
 */
struct Student {
    int id;              ///< Unique student identifier
    string surname;      ///< Student's last name
    int birthYear;       ///< Year of birth
    int studyYear;       ///< Current study year (1-4)
    double gpa;          ///< Grade Point Average

    /**
     * @brief Validates student record for consistency.
     * @return true if all fields are valid, false otherwise
     */
    bool isValid() const {
        return id > 0 && !surname.empty() && birthYear >= 1950 && birthYear <= 2015 &&
            studyYear >= 1 && studyYear <= 4 && gpa >= 0.0 && gpa <= 5.0;
    }
};

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
     * @brief Constructs DualOutputWriter with file path and optional append mode.
     * @param filepath Path to the output file
     * @param append If true, appends to existing file; otherwise overwrites
     * @throws runtime_error if file cannot be opened
     */
    explicit DualOutputWriter(const string& filepath, bool append = false) {
        ios_base::openmode mode = ios::out;
        if (append) mode |= ios::app;
        file_.open(filepath, mode);
        if (!file_.is_open()) {
            throw runtime_error("Cannot open file: " + filepath);
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
 * @brief Student database management system.
 * Handles loading, saving, searching, and displaying student records.
 */
class StudentDatabase {
private:
    vector<Student> students_;
    static constexpr const char* DB_FILE = "students_database.txt";
    static constexpr const char* OUTPUT_FILE = "output_students.txt";

public:
    /**
     * @brief Constructs database and loads existing records from file.
     * If no file exists, initializes with default sample data.
     */
    StudentDatabase() {
        loadFromFile();
        if (students_.empty()) {
            initializeSampleData();
        }
    }

    /**
     * @brief Loads student records from database file.
     * @throws runtime_error if file read operation fails
     */
    void loadFromFile() {
        ifstream input(DB_FILE);
        if (!input.is_open()) {
            cerr << "Warning: Database file '" << DB_FILE << "' not found. Starting fresh.\n";
            return;
        }

        string line;
        while (getline(input, line)) {
            if (line.empty()) continue;

            istringstream iss(line);
            Student student;
            if (iss >> student.id >> student.surname >> student.birthYear
                >> student.studyYear >> student.gpa) {
                if (student.isValid()) {
                    students_.push_back(student);
                }
                else {
                    cerr << "Warning: Skipping invalid record (ID: " << student.id << ")\n";
                }
            }
        }
        input.close();
    }

    /**
     * @brief Initializes database with sample student records.
     * Used when no existing database file is found.
     */
    void initializeSampleData() {
        students_ = {
            {101, "Ivanov", 2005, 1, 4.5},
            {102, "Petrov", 2004, 2, 3.8},
            {103, "Sidorov", 2006, 1, 4.2},
            {104, "Sokolov", 2003, 3, 3.9},
            {105, "Kozlov", 2004, 2, 4.1}
        };
    }

    /**
     * @brief Saves all student records to database file.
     * Overwrites existing file with current database state.
     * @throws runtime_error if file write operation fails
     */
    void saveToFile() {
        ofstream output(DB_FILE);
        if (!output.is_open()) {
            throw runtime_error("Cannot open database file for writing: " + string(DB_FILE));
        }

        for (const auto& student : students_) {
            output << student.id << " " << student.surname << " "
                << student.birthYear << " " << student.studyYear << " "
                << fixed << setprecision(1) << student.gpa << "\n";
        }
        output.close();
    }

    /**
     * @brief Adds a new student record to the database.
     * Validates data before insertion and saves to file.
     * @param student Student record to add
     * @throws invalid_argument if student record is invalid
     * @throws runtime_error if file save operation fails
     */
    void addStudent(const Student& student) {
        if (!student.isValid()) {
            throw invalid_argument("Invalid student record: check ID, year ranges, and GPA bounds");
        }

        // Check for duplicate ID
        if (findById(student.id) != nullptr) {
            throw invalid_argument("Student with ID " + to_string(student.id) + " already exists");
        }

        students_.push_back(student);
        saveToFile();
        cout << "Student record added successfully.\n";
    }

    /**
     * @brief Searches for a student by ID.
     * @param id Student ID to search for
     * @return Pointer to student if found, nullptr otherwise
     */
    const Student* findById(int id) const {
        auto it = find_if(students_.begin(), students_.end(),
            [id](const Student& s) { return s.id == id; });
        return (it != students_.end()) ? &(*it) : nullptr;
    }

    /**
     * @brief Searches database using a custom predicate function.
     * Displays results in formatted table with optional file output.
     * @param predicate Search condition function
     * @param title Display title for results
     */
    void search(function<bool(const Student&)> predicate, const string& title) {
        vector<Student> results;
        copy_if(students_.begin(), students_.end(), back_inserter(results), predicate);

        if (results.empty()) {
            cout << "No records found matching criteria.\n";
            return;
        }

        displayTable(results, title);
    }

    /**
     * @brief Displays all student records in formatted table.
     */
    void displayAll() {
        if (students_.empty()) {
            cout << "Database is empty.\n";
            return;
        }
        displayTable(students_, "ALL STUDENTS");
    }

    /**
     * @brief Returns the total number of students in database.
     * @return Number of student records
     */
    size_t getSize() const {
        return students_.size();
    }

private:
    /**
     * @brief Displays a collection of student records in formatted table.
     * Outputs to both console and file.
     * @param records Collection of students to display
     * @param title Table title
     */
    void displayTable(const vector<Student>& records, const string& title) {
        try {
            DualOutputWriter output(OUTPUT_FILE, true);  // Append mode

            output << "\n" << string(60, '=') << "\n";
            output << "=== " << title << " ===\n";
            output << string(60, '=') << "\n";
            output << setw(6) << "ID" << " | "
                << setw(15) << "Surname" << " | "
                << setw(11) << "Birth Year" << " | "
                << setw(5) << "Year" << " | "
                << setw(6) << "GPA\n";
            output << string(60, '-') << "\n";

            for (const auto& student : records) {
                output << setw(6) << student.id << " | "
                    << setw(15) << student.surname << " | "
                    << setw(11) << student.birthYear << " | "
                    << setw(5) << student.studyYear << " | "
                    << fixed << setprecision(2) << student.gpa << "\n";
            }

            output << string(60, '=') << "\n";
            output << "Total records: " << records.size() << "\n";

        }
        catch (const runtime_error& e) {
            cerr << "Error writing to output file: " << e.what() << "\n";
        }
    }
};

/**
 * @brief Displays interactive menu and returns user choice.
 * @return Menu selection (1-7)
 */
int displayMenu() {
    cout << "\n" << string(50, '=') << "\n";
    cout << "=== STUDENT DATABASE MENU ===\n";
    cout << string(50, '=') << "\n";
    cout << "1. Search by ID\n"
        << "2. Search by Surname\n"
        << "3. Search by Birth Year\n"
        << "4. Search by Study Year\n"
        << "5. Search by GPA (>= threshold)\n"
        << "6. Add New Student\n"
        << "7. Display All Students\n"
        << "8. Exit\n";
    cout << string(50, '=') << "\n";
    cout << "Enter choice (1-8): ";

    int choice;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(10000, '\n');
        return -1;
    }
    cin.ignore(10000, '\n');  // Clear input buffer
    return choice;
}

/**
 * @brief Handles student addition with input validation.
 * @param db Reference to StudentDatabase
 */
void handleAddStudent(StudentDatabase& db) {
    cout << "\n=== ADD NEW STUDENT ===\n";
    cout << "Enter ID: ";
    int id;
    if (!(cin >> id)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cerr << "Invalid input: ID must be a number.\n";
        return;
    }

    cout << "Enter Surname: ";
    string surname;
    cin.ignore();
    getline(cin, surname);
    if (surname.empty()) {
        cerr << "Error: Surname cannot be empty.\n";
        return;
    }

    cout << "Enter Birth Year (1950-2015): ";
    int birthYear;
    if (!(cin >> birthYear)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cerr << "Invalid input: Birth year must be a number.\n";
        return;
    }

    cout << "Enter Study Year (1-4): ";
    int studyYear;
    if (!(cin >> studyYear)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cerr << "Invalid input: Study year must be a number.\n";
        return;
    }

    cout << "Enter GPA (0.0-5.0): ";
    double gpa;
    if (!(cin >> gpa)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cerr << "Invalid input: GPA must be a number.\n";
        return;
    }
    cin.ignore(10000, '\n');

    try {
        db.addStudent({ id, surname, birthYear, studyYear, gpa });
    }
    catch (const invalid_argument& e) {
        cerr << "Error: " << e.what() << "\n";
    }
    catch (const runtime_error& e) {
        cerr << "File Error: " << e.what() << "\n";
    }
}

/**
 * @brief Handles numeric search with user input.
 * @param db Reference to StudentDatabase
 * @param searchType Type of search (1=ID, 3=BirthYear, 4=StudyYear, 5=GPA)
 */
void handleNumericSearch(StudentDatabase& db, int searchType) {
    cout << "Enter search value: ";
    double value;
    if (!(cin >> value)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cerr << "Invalid input: Please enter a valid number.\n";
        return;
    }
    cin.ignore(10000, '\n');

    if (searchType == 1) {
        db.search([value](const Student& s) { return s.id == static_cast<int>(value); },
            "SEARCH RESULTS: ID = " + to_string(static_cast<int>(value)));
    }
    else if (searchType == 3) {
        db.search([value](const Student& s) { return s.birthYear == static_cast<int>(value); },
            "SEARCH RESULTS: Birth Year = " + to_string(static_cast<int>(value)));
    }
    else if (searchType == 4) {
        db.search([value](const Student& s) { return s.studyYear == static_cast<int>(value); },
            "SEARCH RESULTS: Study Year = " + to_string(static_cast<int>(value)));
    }
    else if (searchType == 5) {
        db.search([value](const Student& s) { return s.gpa >= value; },
            "SEARCH RESULTS: GPA >= " + to_string(value));
    }
}

/**
 * @brief Executes student database management system.
 * Provides interactive menu for searching, adding, and displaying student records.
 * All operations are logged to output file.
 *
 * Features:
 * - Search by ID, surname, birth year, study year, or GPA
 * - Add new student records with validation
 * - Display all records
 * - Persistent storage in text file
 * - Data validation and error handling
 */
void task6() {
    try {
        StudentDatabase db;

        cout << "\n========== STUDENT DATABASE SYSTEM ==========\n";
        cout << "Total students loaded: " << db.getSize() << "\n";

        while (true) {
            int choice = displayMenu();

            switch (choice) {
            case 1:
                handleNumericSearch(db, 1);
                break;
            case 2: {
                cout << "Enter surname to search: ";
                string surname;
                getline(cin, surname);
                if (!surname.empty()) {
                    db.search([surname](const Student& s) { return s.surname == surname; },
                        "SEARCH RESULTS: Surname = " + surname);
                }
                else {
                    cerr << "Error: Surname cannot be empty.\n";
                }
                break;
            }
            case 3:
                handleNumericSearch(db, 3);
                break;
            case 4:
                handleNumericSearch(db, 4);
                break;
            case 5:
                handleNumericSearch(db, 5);
                break;
            case 6:
                handleAddStudent(db);
                break;
            case 7:
                db.displayAll();
                break;
            case 8:
                cout << "Exiting student database system. Goodbye!\n";
                return;
            case -1:
                cerr << "Invalid input: Please enter a number 1-8.\n";
                break;
            default:
                cerr << "Invalid choice: Please select an option 1-8.\n";
            }
        }

    }
    catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << "\n";
    }
}