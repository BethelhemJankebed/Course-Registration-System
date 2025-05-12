#include <iostream>
#include "sqlite3.h"
#include <string>
using namespace std;

struct Student {
    string name;
    int ID=0;
    string department;
    string semester;
    string year;

    void getdata() {
        cout << "Enter your name: ";
        cin >> name;
        cout << "Enter your ID: ";
        cin >> ID;
        cout << "Enter your department: ";
        cin >> department;
        cout << "Enter your semester [1 or 2]: ";
        cin >> semester;
        cout << "Enter your academic year : ";
        cin >> year;
    }

    void saveToDatabase(sqlite3* db) {
        string query = "INSERT OR IGNORE INTO Students (studentID, name, department, semester, year) VALUES (?, ?, ?, ?, ?)";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, ID);
            sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, department.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 4, semester.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 5, year.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
        }
        sqlite3_finalize(stmt);
    }

    void viewcourses(sqlite3* db) {
        string query = "SELECT * FROM Courses WHERE department = ? AND semester = ?";
        
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, department.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, semester.c_str(), -1, SQLITE_TRANSIENT);
            bool found = false;
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                found = true;
                cout << "Course ID: " << sqlite3_column_text(stmt, 0) << " ";
                cout << "Course Name: " << sqlite3_column_text(stmt, 1) << endl;
            }
            if (!found) {
                cout << "No matching courses found." << endl;
            }
        }
        sqlite3_finalize(stmt);
    }

    void registercourse(sqlite3* db) {
        string courseid;
        cout << "Enter the course ID to register: ";
        cin >> courseid;

        // Step 1: Check for prerequisite of the selected course
        string prereq_query = "SELECT prerequisite FROM Courses WHERE courseID = ?";
        sqlite3_stmt* prereq_stmt;

        if (sqlite3_prepare_v2(db, prereq_query.c_str(), -1, &prereq_stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(prereq_stmt, 1, courseid.c_str(), -1, SQLITE_TRANSIENT);

            if (sqlite3_step(prereq_stmt) == SQLITE_ROW) {
                const unsigned char* prereq = sqlite3_column_text(prereq_stmt, 0);
                string prerequisite = prereq ? reinterpret_cast<const char*>(prereq) : "";
                sqlite3_finalize(prereq_stmt);

                // Step 2: If there's a prerequisite, check if student has completed it
                if (!prerequisite.empty()) {
                    string check_query = "SELECT * FROM Registrations WHERE studentID = ? AND courseID = ?";
                    sqlite3_stmt* check_stmt;
                    if (sqlite3_prepare_v2(db, check_query.c_str(), -1, &check_stmt, nullptr) == SQLITE_OK) {
                        sqlite3_bind_int(check_stmt, 1, ID);
                        sqlite3_bind_text(check_stmt, 2, prerequisite.c_str(), -1, SQLITE_TRANSIENT);

                        if (sqlite3_step(check_stmt) != SQLITE_ROW) {
                            cout << "You must complete prerequisite course '" << prerequisite << "' before registering." << endl;
                            sqlite3_finalize(check_stmt);
                            return;
                        }
                        sqlite3_finalize(check_stmt);
                    }
                }
            }
            else {
                cout << "Course not found." << endl;
                sqlite3_finalize(prereq_stmt);
                return;
            }
        }

        // Step 3: Register the student for the course
        string insert_query = "INSERT INTO Registrations (studentID, courseID) VALUES (?, ?)";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, insert_query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, ID);
            sqlite3_bind_text(stmt, 2, courseid.c_str(), -1, SQLITE_TRANSIENT);

            if (sqlite3_step(stmt) == SQLITE_DONE) {
                cout << "Course registered successfully!" << endl;
            }
            else {
                cout << "Registration failed. You may already be registered for this course." << endl;
            }
        }
        else {
            cout << "Failed to prepare registration statement." << endl;
        }

        sqlite3_finalize(stmt);
    }


    void dropcourse(sqlite3* db) {
        string courseid;
        cout << "Enter the course ID to drop: ";
        cin >> courseid;
        string query = "DELETE FROM Registrations WHERE studentID = ? AND courseID = ?";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, ID);
            sqlite3_bind_text(stmt, 2, courseid.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);

            cout << "Course dropped." << endl;
        }
		else {
			cout << "Error dropping course." << endl;
		}
		
        sqlite3_finalize(stmt);
    }

    void viewprerequisites(sqlite3* db) {
        string courseid;
        cout << "Enter the course ID to view prerequisites: ";
        cin >> courseid;

        string query = "SELECT prerequisite FROM Courses WHERE courseID = ?";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, courseid.c_str(), -1, SQLITE_TRANSIENT);

            if (sqlite3_step(stmt) == SQLITE_ROW) {
                const unsigned char* prereq = sqlite3_column_text(stmt, 0);
                if (prereq != nullptr && strlen(reinterpret_cast<const char*>(prereq)) > 0) {
                    cout << "Prerequisite: " << prereq << endl;
                }
                else {
                    cout << "This course has no prerequisite." << endl;
                }
            }
        }

        sqlite3_finalize(stmt);
    }
};

struct Admin {
    string name;
    int empid=0;

    void getdata() {
        cout << "Enter your name: ";
        cin >> name;
        cout << "Enter your employee ID: ";
        cin >> empid;
    }

    void addcourse(sqlite3* db) {
        string courseid, coursename, department, semester, prerequisite;

        cout << "Enter course ID: ";
        cin >> courseid;
        cout << "Enter course name: ";
        cin >> coursename;
        cout << "Enter department: ";
        cin >> department;
        cout << "Enter semester [1 or 2]: ";
        cin >> semester;
        cout << "Enter prerequisite (or type NONE if there is no prerequisite): ";
        cin >> prerequisite;

        if (prerequisite == "NONE") {
            prerequisite = "";
        }

        string query = "INSERT INTO Courses (courseID, courseName, department, semester, prerequisite) VALUES (?, ?, ?, ?, ?)";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, courseid.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, coursename.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, department.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 4, semester.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 5, prerequisite.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            cout << "Course added successfully!" << endl;
        }
        sqlite3_finalize(stmt);
    }

    void deletecourse(sqlite3* db) {
        string courseid;
        cout << "Enter course ID to delete: ";
        cin >> courseid;
        string query = "DELETE FROM Courses WHERE courseID = ?";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, courseid.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
        }
        sqlite3_finalize(stmt);
        cout << "Course deleted." << endl;
    }

    void viewcourses(sqlite3* db) {
        sqlite3_stmt* stmt;
        string query = "SELECT * FROM Courses";
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                cout << "Course ID: " << sqlite3_column_text(stmt, 0) << " ";
                cout << "Course Name: " << sqlite3_column_text(stmt, 1) << endl;
            }
        }
        sqlite3_finalize(stmt);
    }

    void viewregistrations(sqlite3* db) {
        sqlite3_stmt* stmt;
        string query = "SELECT * FROM Registrations";
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                cout << "Student ID: " << sqlite3_column_text(stmt, 0) << " ";
                cout << "Course ID: " << sqlite3_column_text(stmt, 1) << endl;
            }
        }
        sqlite3_finalize(stmt);
    }
};

int main() {
    sqlite3* db;
    if (sqlite3_open("C:/Users/beka/Desktop/sqlite db/registration.db", &db)) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return 1;
    }
    // Enable foreign key support (must be done immediately after opening the DB)
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    // Error message pointer for diagnostics
    char* errMsg = nullptr;

    // Create Students table
    string createStudentsTable = R"(
    CREATE TABLE IF NOT EXISTS Students (
        studentID INTEGER PRIMARY KEY,
        name TEXT,
        department TEXT,
        semester TEXT,
        year TEXT
    );
)";
    int rc1 = sqlite3_exec(db, createStudentsTable.c_str(), nullptr, nullptr, &errMsg);
    if (rc1 != SQLITE_OK) {
        cerr << "Error creating Students table: " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    // Create Courses table
    string createCoursesTable = R"(
    CREATE TABLE IF NOT EXISTS Courses (
        courseID TEXT PRIMARY KEY,
        courseName TEXT,
        department TEXT,
        semester TEXT,
        prerequisite TEXT
    );
)";
    int rc2 = sqlite3_exec(db, createCoursesTable.c_str(), nullptr, nullptr, &errMsg);
    if (rc2 != SQLITE_OK) {
        cerr << "Error creating Courses table: " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    // Create Registrations table
    string createRegistrationsTable = R"(
    CREATE TABLE IF NOT EXISTS Registrations (
        studentID INTEGER,
        courseID TEXT,
        PRIMARY KEY (studentID, courseID),
        FOREIGN KEY (studentID) REFERENCES Students(studentID),
        FOREIGN KEY (courseID) REFERENCES Courses(courseID)
    );
)";
    int rc3 = sqlite3_exec(db, createRegistrationsTable.c_str(), nullptr, nullptr, &errMsg);
    if (rc3 != SQLITE_OK) {
        cerr << "Error creating Registrations table: " << errMsg << endl;
        sqlite3_free(errMsg);
    }

   
    char choice;
    cout << " Enter 's' for student or 'a' for admin: ";
    cin >> choice;

    if (choice == 's' || choice == 'S') {
        Student s;
        s.getdata();
        s.saveToDatabase(db);
        int option;
        cout << "1. View courses\n2. Register for course\n3. Drop course\n4. View prerequisites\nChoice: ";
        cin >> option;
        switch (option) {
        case 1: s.viewcourses(db); break;
        case 2: s.registercourse(db); break;
        case 3: s.dropcourse(db); break;
        case 4: s.viewprerequisites(db); break;
        default: cout << "Invalid choice." << endl;
        }
    }
    else if (choice == 'a' || choice == 'A') {
        Admin a;
        a.getdata();
		string adminpassword = "admin123"; 
		string inputPass;
        cout << "Enter admin password: ";
        cin >> inputPass;

        if (inputPass != adminpassword) {
            cout << "Access denied. Incorrect password." << endl;
            return 0; 
        }
        int option;
        cout << "1. Add course\n2. Delete course\n3. View courses\n4. View registrations\nChoice: ";
        cin >> option;
        switch (option) {
        case 1: a.addcourse(db); break;
        case 2: a.deletecourse(db); break;
        case 3: a.viewcourses(db); break;
        case 4: a.viewregistrations(db); break;
        default: cout << "Invalid choice." << endl;
        }
    }

    sqlite3_close(db);
    return 0;
}
