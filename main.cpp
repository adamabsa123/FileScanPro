#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

enum Department { DEFAULT, SALES, CUSTOMER_SERVICE, PRODUCTION, HUMAN_RESOURCES, ACCOUNTING, RESEARCH_AND_DEVELOPMENT };

string departmentToString(Department dept) {
    switch (dept) {
        case SALES: return "SALES";
        case CUSTOMER_SERVICE: return "CUSTOMER SERVICE";
        case PRODUCTION: return "PRODUCTION";
        case HUMAN_RESOURCES: return "HUMAN RESOURCES";
        case ACCOUNTING: return "ACCOUNTING";
        case RESEARCH_AND_DEVELOPMENT: return "RESEARCH AND DEVELOPMENT";
        default: return "DEFAULT";
    }
}

class Employee {
public:
    static int count;
    int aID;
    string fullName;
    Department dept;

    Employee() : aID(0), fullName("NONAME"), dept(DEFAULT) { count++; }
    Employee(int id, string name, Department d) : aID(id), fullName(name), dept(d) { count++; }
    ~Employee() { count--; }

    void display() const {
        cout << left << setw(10) << aID << setw(20) << (fullName.empty() ? "NONAME" : fullName) << setw(30) << departmentToString(dept) << endl;
    }

    bool operator < (const Employee& emp) const {
        return aID < emp.aID;
    }
};

int Employee::count = 0;

class Address {
public:
    static int count;
    int aID;
    int streetNumber;
    string streetName;
    string city;
    string state;
    int zipCode;

    Address() : aID(0), streetNumber(0), streetName(""), city(""), state(""), zipCode(0) { count++; }
    Address(int id, int streetNum, string street, string cty, string st, int zip) : aID(id), streetNumber(streetNum), streetName(street), city(cty), state(st), zipCode(zip) { count++; }
    ~Address() { count--; }

    void display() const {
        cout << left << setw(10) << aID << setw(15) << streetNumber << setw(20) << streetName << setw(15) << city << setw(10) << state << setw(10) << zipCode << endl;
    }

    bool operator < (const Address& addr) const {
        return aID < addr.aID;
    }
};

int Address::count = 0;

bool fileExists(const string& filename) {
    ifstream file(filename);
    return file.good();
}

vector<Employee> readEmployees(const string& filename) {
    ifstream file(filename);
    if (!file.good()) {
        cerr << "Error opening file: " << filename << endl;
        exit(EXIT_FAILURE);
    }
    vector<Employee> employees;
    string line, name;
    int id, dept;
    while (getline(file, line)) {
        stringstream ss(line);
        if (!(ss >> id)) {
            cerr << "Error reading ID from line: " << line << endl;
            continue; // Skip invalid lines
        }
        getline(ss, name, ' '); // Skip the space after id
        getline(ss, name, '\n'); // Read the full name
        name = name.substr(0, name.find_last_not_of("0123456789 \t") + 1);
        if (!(stringstream(name.substr(name.find_last_of(" ") + 1)) >> dept)) {
            cerr << "Error reading department from line: " << line << endl;
            continue; // Skip invalid lines
        }
        name = name.substr(0, name.find_last_of(" "));
        employees.emplace_back(id, name, static_cast<Department>(dept));
    }
    return employees;
}

vector<Address> readAddresses(const string& filename) {
    ifstream file(filename);
    if (!file.good()) {
        cerr << "Error opening file: " << filename << endl;
        exit(EXIT_FAILURE);
    }
    vector<Address> addresses;
    string line, streetName, city, state;
    int id, streetNumber, zipCode;
    while (getline(file, line)) {
        stringstream ss(line);
        if (!(ss >> id >> streetNumber)) {
            cerr << "Error reading address from line: " << line << endl;
            continue; // Skip invalid lines
        }
        getline(ss, streetName, '\n'); // Read until newline
        streetName = streetName.substr(1, streetName.find_last_of(',') - 1); // Extract streetName
        zipCode = stoi(streetName.substr(streetName.find_last_of(' ') + 1));
        state = streetName.substr(streetName.find_last_of(',', streetName.find_last_of(',') - 1) + 2, 2);
        city = streetName.substr(streetName.find_first_of(',') + 2, streetName.find_last_of(',') - streetName.find_first_of(',') - 3);
        streetName = streetName.substr(0, streetName.find_first_of(','));
        addresses.emplace_back(id, streetNumber, streetName, city, state, zipCode);
    }
    return addresses;
}

void displayMenu() {
    cout << "Menu:\n";
    cout << "1. Display All Records\n";
    cout << "2. Search by Address ID\n";
    cout << "3. Search by ZIP Code\n";
    cout << "4. Sort by Address ID\n";
    cout << "5. Sort by ZIP Code\n";
    cout << "6. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    if (!fileExists("employees.txt") || !fileExists("addresses.txt")) {
        cerr << "Error: One or more required files are missing." << endl;
        return EXIT_FAILURE;
    }

    vector<Employee> employees = readEmployees("employees.txt");
    vector<Address> addresses = readAddresses("addresses.txt");

    int choice;
    do {
        displayMenu();
        while (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 6." << endl;
            displayMenu();
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

        switch (choice) {
            case 1: // Display All Records
                for (const auto& emp : employees) {
                    emp.display();
                }
                for (const auto& addr : addresses) {
                    addr.display();
                }
                break;
            case 2: { // Search by Address ID
                int searchId;
                cout << "Enter Address ID to search: ";
                while (!(cin >> searchId)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a valid number." << endl;
                    cout << "Enter Address ID to search: ";
                }
                auto it = find_if(addresses.begin(), addresses.end(), [&searchId](const Address& addr) { return addr.aID == searchId; });
                if (it != addresses.end()) {
                    it->display();
                } else {
                    cout << "No address found with ID " << searchId << endl;
                }
                break;
            }
            case 3: { // Search by ZIP Code
                int searchZip;
                cout << "Enter ZIP Code to search: ";
                while (!(cin >> searchZip)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a valid number." << endl;
                    cout << "Enter ZIP Code to search: ";
                }
                bool found = false;
                for (const auto& addr : addresses) {
                    if (addr.zipCode == searchZip) {
                        addr.display();
                        found = true;
                    }
                }
                if (!found) {
                    cout << "No addresses found with ZIP Code " << searchZip << endl;
                }
                break;
            }
            case 4: { // Sort by Address ID
                sort(addresses.begin(), addresses.end());
                cout << "Addresses sorted by ID:" << endl;
                for (const auto& addr : addresses) {
                    addr.display();
                }
                break;
            }
            case 5: { // Sort by ZIP Code
                sort(addresses.begin(), addresses.end(), [](const Address& a, const Address& b) { return a.zipCode < b.zipCode; });
                cout << "Addresses sorted by ZIP Code:" << endl;
                for (const auto& addr : addresses) {
                    addr.display();
                }
                break;
            }
            case 6: // Exit
                cout << "Exiting program." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
        cout << "Press enter to continue..." << endl;
        cin.get(); // Pause after each action
    } while (choice != 6);

    return 0;
}
