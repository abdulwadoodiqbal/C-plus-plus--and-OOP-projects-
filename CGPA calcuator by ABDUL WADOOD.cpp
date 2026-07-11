// TASK 1: CGPA Calculator


#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

// Represents a single course entry
struct Course {
    string grade;
    int creditHours;
    double gradePoint;  
    double qualityPoints; 
};


double letterToPoint(string grade) {
    // normalize to uppercase for comparison
    for (char& c : grade) c = toupper(c);

    if (grade == "A")  return 4.0;
    if (grade == "A-") return 3.7;
    if (grade == "B+") return 3.3;
    if (grade == "B")  return 3.0;
    if (grade == "B-") return 2.7;
    if (grade == "C+") return 2.3;
    if (grade == "C")  return 2.0;
    if (grade == "C-") return 1.7;
    if (grade == "D+") return 1.3;
    if (grade == "D")  return 1.0;
    if (grade == "F")  return 0.0;

    return -1.0; 
}

int main() {
    int numCourses;

    cout << "===== CGPA CALCULATOR =====\n\n";

    // Number of courses
    cout << "Enter the number of courses taken this semester: ";
    cin >> numCourses;

    while (numCourses <= 0) {
        cout << "Please enter a valid number of courses (greater than 0): ";
        cin >> numCourses;
    }

    vector<Course> courses(numCourses);

    // Input grade and credit hours for each course
    for (int i = 0; i < numCourses; i++) {
        double point = -1.0;
        string grade;
        int credits;

        cout << "\n--- Course " << i + 1 << " ---\n";

        do {
            cout << "Enter grade (A, A-, B+, B, B-, C+, C, C-, D+, D, F): ";
            cin >> grade;
            point = letterToPoint(grade);
            if (point < 0)
                cout << "Invalid grade entered. Please try again.\n";
        } while (point < 0);

       
        do {
            cout << "Enter credit hours for this course: ";
            cin >> credits;
            if (credits <= 0)
                cout << "Credit hours must be greater than 0.\n";
        } while (credits <= 0);

        courses[i].grade = grade;
        courses[i].creditHours = credits;
        courses[i].gradePoint = point;
        courses[i].qualityPoints = point * credits;
    }

    // Calculate totals for this semester
    double totalCredits = 0.0;
    double totalQualityPoints = 0.0;

    for (const auto& c : courses) {
        totalCredits += c.creditHours;
        totalQualityPoints += c.qualityPoints;
    }

    double semesterGPA = totalQualityPoints / totalCredits;

    // Optionally combine with previous CGPA to get overall CGPA
    char hasPrevious;
    double prevCGPA = 0.0, prevCredits = 0.0;

    cout << "\nDo you have a previous CGPA to combine with this semester? (y/n): ";
    cin >> hasPrevious;

    if (hasPrevious == 'y' || hasPrevious == 'Y') {
        cout << "Enter your previous CGPA: ";
        cin >> prevCGPA;
        cout << "Enter your total credit hours completed before this semester: ";
        cin >> prevCredits;
    }

    double overallCredits = totalCredits + prevCredits;
    double overallQualityPoints = totalQualityPoints + (prevCGPA * prevCredits);
    double overallCGPA = overallQualityPoints / overallCredits;

    // Display results
    cout << fixed << setprecision(2);

    cout << "\n===== COURSE SUMMARY =====\n";
    cout << left << setw(10) << "Course" << setw(10) << "Grade"
        << setw(15) << "Credit Hrs" << setw(15) << "Quality Pts" << "\n";
    cout << "--------------------------------------------------\n";

    for (int i = 0; i < numCourses; i++) {
        cout << left << setw(10) << ("Course " + to_string(i + 1))
            << setw(10) << courses[i].grade
            << setw(15) << courses[i].creditHours
            << setw(15) << courses[i].qualityPoints << "\n";
    }

    cout << "\n===== RESULTS =====\n";
    cout << "Total Credit Hours (this semester): " << totalCredits << "\n";
    cout << "Total Quality Points (this semester): " << totalQualityPoints << "\n";
    cout << "Semester GPA: " << semesterGPA << "\n";

    if (hasPrevious == 'y' || hasPrevious == 'Y') {
        cout << "\nOverall Credit Hours: " << overallCredits << "\n";
        cout << "Overall CGPA: " << overallCGPA << "\n";
    }
    else {
        cout << "\nOverall CGPA (this semester only): " << overallCGPA << "\n";
    }

    cout << "\nThank you for using the CGPA Calculator!\n";

    return 0;
}
