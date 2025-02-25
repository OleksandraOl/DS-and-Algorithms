//============================================================================
// Name        : ProjectTwo.cpp
// Version     : 1.0
// Description : Project Two/Course planner
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <cctype>
#include <algorithm>

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 179;


// define a structure to hold course information
struct Course {
    string courseNumber; // unique identifier
    string courseName;
    vector<string> prerequisites;

    //method to add prerequisites
    void addPrerequisite(string prerequisite) {
        prerequisites.push_back(prerequisite);
    }

};

//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:
    // Define structures to hold bids
    struct Node {
        Course course;
        unsigned int key;
        Node* next;

        // default constructor
        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // initialize with a bid
        Node(Course aCourse) : Node() {
            course = aCourse;
        }

        // initialize with a bid and a key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) {
            key = aKey;
        }
    };

    vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(int key);

    //helper function to conver courseNUmber to int
    int courseNumberToInt(string courseNumber) {
        //initial hash value
        int valueToHash = 0;
        //prime number to distribute hash values
        const int primeNumber = 7;

        //converts string characters to ASCII
        for (char c : courseNumber) {
            valueToHash = (valueToHash * primeNumber) + c;
        }

        return valueToHash;
    }

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAll(const set<string>& courseNumbers); 
    Course Search(string courseNumber);
};


/**
 * Default constructor
 */
HashTable::HashTable() {
    // Initalize node structure by resizing tableSize
    nodes.resize(tableSize);
}


/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    // resize tableSize to the inout value
    this->tableSize = size;
    // resize nodes 
    nodes.resize(size);
}


/**
 * Destructor
 */
HashTable::~HashTable() {
    // erase nodes beginning
    nodes.erase(nodes.begin());
}

/**
 * Hash value calculation.
 * Key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(int key) {
    // return key tableSize
    return key % tableSize;
}

/**
 * Insert a course
 *
 * @param course to insert
 */
void HashTable::Insert(Course course) {
    // create the key for the given course
    unsigned int key = hash(courseNumberToInt(course.courseNumber));
    // retrieve node using key
    Node* currNode = &nodes.at(key);

    // if no entry found for the key
    if (currNode == nullptr) {
        //create new node
        Node newNode = Node(course, key);
        // assign this node to the key position
        nodes.at(key) = newNode;
    }
    else {
        // node is not used
        if (currNode->key == UINT_MAX) {
            //assign the node with the current info
            currNode->key = key;
            currNode->course = course;
            currNode->next = nullptr;
        }
        // key collision 
        else {
            //find the last node in the list
            while (currNode->next != nullptr) {
                currNode = currNode->next;
            }

            // add new newNode to the ned of the list
            currNode->next = new Node(course, key);
        }
    }

}

//forward declaration
void displayCourse(Course course);
void displayCourseAndPrerequisites(Course course);
string toUpper(string str);                         //converts case 
string trim(const string& str);                     //trims inout


/**
 * Print all courses
 */
void HashTable::PrintAll(const set<string>& courseNumbers) { 
    // iterate through the set(already ordered) of courseNumbers
    for (auto it = courseNumbers.begin(); it != courseNumbers.end(); ++it) {
        //find the course in the hash table
        Course foundCourse = Search(*it);
        //display the found course
        displayCourse(foundCourse);
    }
}

/**
 * Search for the specified courseNumber
 *
 * @param courseNumber to search for
 */
Course HashTable::Search(string courseNumber) {
    Course course;

    // create the key for the given courseNumber
    unsigned int key = hash(courseNumberToInt(courseNumber));
    Node* node = &(nodes.at(key));

    // if no entry found for the key
    if (node == nullptr || node->key == UINT_MAX) {
        // return course
        return course;
    }

    // if entry found for the key
    if (node->course.courseNumber.compare(courseNumber) == 0) {
        //return node course
        return node->course;
    }

    // while node not equal to nullptr
    while (node != nullptr) {
        // if the current node matches, return it
        if (node->key != UINT_MAX && node->course.courseNumber.compare(courseNumber) == 0) {
            return node->course;
        }
        //else node is equal to next node
        else {
            node = node->next;
        }
    }

    return course;
}

//trim the whitespaces
string trim(const string& str) {
    size_t start = 0, end = str.size();

    // move start forward past leading whitespace
    while (start < end && isspace(static_cast<unsigned char>(str[start]))) {
        ++start;
    }

    // move end backward past trailing whitespace
    while (end > start && isspace(static_cast<unsigned char>(str[end - 1]))) {
        --end;
    }

    return str.substr(start, end - start);
}

//convert the string to upper case
string toUpper(string str) {
    transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return str;
}


/**
 * Load a CSV file containing courses into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the bids read
 */
void readAndParseFile(string& csvPath, HashTable* hashTable, set<string>& courseNumbers) {

    ifstream file;

    //open file
    file.open(csvPath);

    if (file) {

        string line;
        int counter = 0;                //keeping track of the number of lines for statistic
        bool fileNotEmpty = false;      //check if any of the lines were read

        cout << "Loading CSV file... " << csvPath << endl;
        while (getline(file, line)) {
            //iterate the number of line
            counter += 1;

            //skip empty line
            if (line.empty()) {
                continue;
            }

            //at least one of the lines was read
            fileNotEmpty = true;

            stringstream ss(line);
            string courseNumber, courseName, prerequisite;

            //parse the first two elements to check if the required fields are filled out
            getline(ss, courseNumber, ',');
            getline(ss, courseName, ',');

            //trim the fields
            courseNumber = trim(courseNumber);
            courseName = trim(courseName);

            //check if the required items are empty after trimming whitespaces
            if (courseNumber.empty() || courseName.empty()) {
                cout << "Line #" << counter << " was skipped due insufficient number of elements." << endl;
                //skip the line if not all required elements are available
                continue;
            }

            // Create a data structure and add to the collection of courses
            Course course;
            course.courseNumber = courseNumber;
            course.courseName = courseName;

            //add course number to a set for further use
            courseNumbers.insert(courseNumber);
            
            while (getline(ss, prerequisite, ',')) {
                //check if the field is not empty after removing whitespaces
                if (!(trim(prerequisite).empty())) {
                    course.addPrerequisite(prerequisite);
                }
            }

            // push this course into the hash table
            hashTable->Insert(course);
        }

        if (file.bad()) {
            cerr << "Error: an error occured when reading a file.";
        }

        //output error message if the file was empty
        if (!fileNotEmpty) {
            cout << "The file is empty. Please try a different one." << endl;
        }

        file.close();
        cout << "The file was read!" << endl;
    }
    else {
        cerr << "Error: failed to open the file." << endl;
    }
    
}

/**
 * Display the course information to the console 
 *
 * @param course struct containing the course info
 */
//display only course number and name for list printing purposes
void displayCourse(Course course) {
    cout << course.courseNumber << ", " << course.courseName << endl;
}

//display all the information for the search function
void displayCourseAndPrerequisites(Course course) {
    //display course number and name
    displayCourse(course);
    //check if course has any prerequisites
    if (!course.prerequisites.empty()) {
        cout << "Prererequisites: ";
        //output each prerequisite from a vector
        for (string& elem : course.prerequisites) {
            cout << elem << " ";
        }

        cout << endl;
    }
    
    return;
}


int main() {

    // Define a hash table to hold all the bids
    HashTable* courseTable;
    set<string> courseNumbers;
    string csvPathDefault;
    string csvPathInput;
   
    string courseNumber;

    Course course;
    courseTable = new HashTable();
    csvPathDefault = "CS 300 ABCU_Advising_Program_Input.csv";

    string input;
    int choice = 0;
    cout << "Welcome to the course planner!" << endl;

    //menu loop
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Courses" << endl;
        cout << "  2. Display All Courses" << endl;
        cout << "  3. Find Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";

        getline(cin, input);        //get input as a line to prevent crashes if more than one character is entered
        
        //check if the input is one digit only
        if (input.length() == 1 && isdigit(input[0])) {
            choice = input[0] - '0';

            switch (choice) {

            case 1:
                cout << "Enter CSV file path to read (or press Enter to use default: \"CS 300 ABCU_Advising_Program_Input.csv\")" << endl;
                //get file name
                getline(cin, csvPathInput);
                
                //if input is empty when trimmed, use the default path
                if (trim(csvPathInput).empty()) {
                    readAndParseFile(csvPathDefault, courseTable, courseNumbers);
                }
                //read user's file
                else {
                    readAndParseFile(csvPathInput, courseTable, courseNumbers);
                }
                break;

            case 2:
                courseTable->PrintAll(courseNumbers);
                break;

            case 3:
                //prompt a user for an input
                cout << "Enter a course number to search: " << endl;
                cin >> courseNumber;

                // Ignore the rest of the line (including the newline)
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

                //converts the input to upper case 
                course = courseTable->Search(toUpper(courseNumber));

                if (!course.courseNumber.empty()) {
                    displayCourseAndPrerequisites(course);
                }
                else {
                    cout << "Course " << courseNumber << " not found." << endl;
                }
                break;

            default:
                cout << choice << " is not a valid option. Please choose from the available options." << endl;
                break;
            }
        }
        //if more than one character was entered. Prevents the program from crashing
        else {
            cout << "Invalid input. Please choose from the available options." << endl;
        }

    }

    cout << "Good bye." << endl;

	return 0;
}