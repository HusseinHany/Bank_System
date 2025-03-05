#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <functional>
#include <limits>

using namespace std;

class Validation {
public:
    static void validateName(const string& name) {
        if (name.length() < 5 || name.length() > 20) {
            throw invalid_argument("Name must be between 5 and 20 characters.");
        }
        for (char c : name) {
            if (!isalpha(c) && c != ' ' && c != '-') {
                throw invalid_argument("Name must contain only letters, spaces, and hyphens.");
            }
        }
    }

    static void validateId(int id) {
        if (id <= 0) {
            throw invalid_argument("ID must be a positive integer.");
        }
    }

    static void validatePassword(const string& password) {
        if (password.length() < 8 || password.length() > 20) {
            throw invalid_argument("Password must be between 8 and 20 characters.");
        }

        bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
        for (char c : password) {
            if (isupper(c)) hasUpper = true;
            if (islower(c)) hasLower = true;
            if (isdigit(c)) hasDigit = true;
            if (ispunct(c)) hasSpecial = true;
            if (isspace(c)) {
                throw invalid_argument("Password must not contain spaces.");
            }
        }

        if (!hasUpper || !hasLower || !hasDigit || !hasSpecial) {
            throw invalid_argument("Password must contain at least one uppercase letter, one lowercase letter, one digit, and one special character.");
        }
    }

    static void validateBalance(double balance) {
        if (balance < 1500) {
            throw invalid_argument("Balance must be at least 1500.");
        }
    }

    static void validateSalary(double salary) {
        if (salary < 5000) {
            throw invalid_argument("Salary must be at least 5000.");
        }
    }
};

class Person {
protected:
    string name;
    string password;
    int id;

public:
    Person(int id, const string& name, const string& password) : id(id), name(name), password(password) {
        Validation::validateId(id);
        Validation::validateName(name);
        Validation::validatePassword(password);
    }

    string getName() const { return name; }
    int getId() const { return id; }
    string getPassword() const { return password; }
};

class Client : public Person {
private:
    double balance;

public:
    Client(int id, const string& name, const string& password, double balance)
        : Person(id, name, password), balance(balance) {
        Validation::validateBalance(balance);
    }

    void deposit(double amount) { balance += amount; }

    void withdraw(double amount) {
        if (amount > balance) {
            throw invalid_argument("Insufficient balance.");
        }
        balance -= amount;
    }

    void transferTo(double amount, Client& recipient) {
        withdraw(amount);
        recipient.deposit(amount);
    }

    void setName(const string& newName) {
        try {
            Validation::validateName(newName);
            name = newName;
            cout << "Name updated successfully to: " << name << endl;
        } catch (const invalid_argument& e) {
            cout << "Error updating name: " << e.what() << endl;
        }
    }

    void setPassword(const string& newPassword) {
        try {
            Validation::validatePassword(newPassword);
            password = newPassword;
            cout << "Password updated successfully." << endl;
        } catch (const invalid_argument& e) {
            cout << "Error updating password: " << e.what() << endl;
        }
    }

    void setBalance(double newBalance) {
        try {
            Validation::validateBalance(newBalance);
            balance = newBalance;
            cout << "Balance updated successfully to: " << balance << endl;
        } catch (const invalid_argument& e) {
            cout << "Error updating balance: " << e.what() << endl;
        }
    }

    void checkBalance() const {
        cout << "Current balance: " << balance << endl;
    }

    double getBalance() const { return balance; }

    void display() const {
        cout << "Client ID: " << id << ", Name: " << name << ", Balance: " << balance << endl;
    }
};

class Employee : public Person {
protected:
    double salary;
    vector<Client> clients;

public:
    Employee(int id, const string& name, const string& password, double salary)
        : Person(id, name, password), salary(salary) {
        Validation::validateSalary(salary);
    }

    void addClient(const Client& client) {
        clients.push_back(client);
        cout << "Client " << client.getName() << " added successfully.\n";
    }

    int searchClient(int clientId) {
        for (size_t i = 0; i < clients.size(); i++) {
            if (clients[i].getId() == clientId) {
                return i; // Return the index of the client
            }
        }
        return -1; // Return -1 if not found
    }

    void listClients() const {
        if (clients.empty()) {
            cout << "No clients found.\n";
            return;
        }
        cout << "\n--- Client List ---\n";
        for (const Client& client : clients) {
            client.display();
        }
    }

    void editClient(int clientId, const string& newName, const string& newPassword, double newBalance) {
        int index = searchClient(clientId);
        if (index != -1) {
            clients[index].setName(newName);
            clients[index].setPassword(newPassword);
            clients[index].setBalance(newBalance);
            cout << "Client updated successfully.\n";
        } else {
            cout << "Client with ID " << clientId << " not found.\n";
        }
    }

    double getSalary() const { return salary; }

    void display() const {
        cout << "Employee ID: " << id << ", Name: " << name << ", Salary: " << salary << endl;
    }
};

class Admin : public Employee {
public:
    Admin(int id, const string& name, const string& password, double salary)
        : Employee(id, name, password, salary) {}

    void display() const {
        cout << "Admin ID: " << id << ", Name: " << name << ", Salary: " << salary << endl;
    }
};

class DatasourceInterface {
public:
    virtual void addClient(const Client& c) = 0;
    virtual void addEmployee(const Employee& e) = 0;
    virtual void addAdmin(const Admin& a) = 0;

    virtual vector<Client> getAllClients() = 0;
    virtual vector<Employee> getAllEmployees() = 0;
    virtual vector<Admin> getAllAdmins() = 0;

    virtual void removeAllClients() = 0;
    virtual void removeAllEmployees() = 0;
    virtual void removeAllAdmins() = 0;
};

class Parser {
public:
    static vector<string> split(const string& line) {
        vector<string> result;
        stringstream ss(line);
        string word;
        while (getline(ss, word, ',')) {
            result.push_back(word);
        }
        return result;
    }

    static Client parseToClient(const string& line) {
        vector<string> parts = split(line);
        if (parts.size() != 4) {
            throw invalid_argument("Invalid client data: " + line + " (expected 4 fields)");
        }
        try {
            int id = stoi(parts[0]);
            string name = parts[1];
            string password = parts[2];
            double balance = stod(parts[3]);

            return Client(id, name, password, balance);
        } catch (const invalid_argument& e) {
            throw invalid_argument("Invalid client data: " + string(e.what()) + " in line: " + line);
        } catch (const out_of_range& e) {
            throw invalid_argument("Out of range error: " + string(e.what()) + " in line: " + line);
        }
    }

    static Employee parseToEmployee(const string& line) {
        vector<string> parts = split(line);
        if (parts.size() != 4) {
            throw invalid_argument("Invalid employee data: " + line + " (expected 4 fields)");
        }
        try {
            int id = stoi(parts[0]);
            string name = parts[1];
            string password = parts[2];
            double salary = stod(parts[3]);

            return Employee(id, name, password, salary);
        } catch (const invalid_argument& e) {
            throw invalid_argument("Invalid employee data: " + string(e.what()) + " in line: " + line);
        } catch (const out_of_range& e) {
            throw invalid_argument("Out of range error: " + string(e.what()) + " in line: " + line);
        }
    }

    static Admin parseToAdmin(const string& line) {
        vector<string> parts = split(line);
        if (parts.size() != 4) {
            throw invalid_argument("Invalid admin data: " + line + " (expected 4 fields)");
        }
        try {
            int id = stoi(parts[0]);
            string name = parts[1];
            string password = parts[2];
            double salary = stod(parts[3]);

            return Admin(id, name, password, salary);
        } catch (const invalid_argument& e) {
            throw invalid_argument("Invalid admin data: " + string(e.what()) + " in line: " + line);
        } catch (const out_of_range& e) {
            throw invalid_argument("Out of range error: " + string(e.what()) + " in line: " + line);
        }
    }
};

class FileHelper {
public:
    static void saveToFile(const string& fileName, const string& data) {
        ofstream file(fileName, ios::app);
        if (file.is_open()) {
            file << data << "\n";
            file.close();
        } else {
            cout << "Error: Unable to open " << fileName << " for saving.\n";
        }
    }

    static void readFromFile(const string& fileName, const function<void(const string&)>& parser) {
        ifstream file(fileName);
        if (!file) {
            cout << "Error: File " << fileName << " does not exist or cannot be opened.\n";
            return;
        }
        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                parser(line);
            }
        }
        file.close();
    }

    static void clearFile(const string& fileName) {
        ofstream file(fileName, ios::trunc);
        if (file.is_open()) {
            file.close();
            cout << fileName << " has been cleared.\n";
        } else {
            cout << "Error: Unable to open " << fileName << " for clearing.\n";
        }
    }
};

class FileManager : public DatasourceInterface {
public:
    void addClient(const Client& c) override {
        if (!isClientIdUnique(c.getId())) {
            cout << "Error: Client ID " << c.getId() << " already exists.\n";
            return;
        }
        FileHelper::saveToFile("clients.txt", to_string(c.getId()) + "," + c.getName() + "," + c.getPassword() + "," + to_string(c.getBalance()));
    }

    void addEmployee(const Employee& e) override {
        if (!isEmployeeIdUnique(e.getId())) {
            cout << "Error: Employee ID " << e.getId() << " already exists.\n";
            return;
        }
        FileHelper::saveToFile("employees.txt", to_string(e.getId()) + "," + e.getName() + "," + e.getPassword() + "," + to_string(e.getSalary()));
    }

    void addAdmin(const Admin& a) override {
        if (!isAdminIdUnique(a.getId())) {
            cout << "Error: Admin ID " << a.getId() << " already exists.\n";
            return;
        }
        FileHelper::saveToFile("admins.txt", to_string(a.getId()) + "," + a.getName() + "," + a.getPassword() + "," + to_string(a.getSalary()));
    }

    vector<Client> getAllClients() override {
        vector<Client> clients;
        FileHelper::readFromFile("clients.txt", [&](const string& line) {
            clients.push_back(Parser::parseToClient(line));
        });
        return clients;
    }

    vector<Employee> getAllEmployees() override {
        vector<Employee> employees;
        FileHelper::readFromFile("employees.txt", [&](const string& line) {
            employees.push_back(Parser::parseToEmployee(line));
        });
        return employees;
    }

    vector<Admin> getAllAdmins() override {
        vector<Admin> admins;
        FileHelper::readFromFile("admins.txt", [&](const string& line) {
            admins.push_back(Parser::parseToAdmin(line));
        });
        return admins;
    }

    void deleteClient(int clientId) {
        vector<Client> clients = getAllClients();
        vector<Client> remainingClients;
        int count = 0;

        for (const Client& client : clients) {
            if (client.getId() == clientId) {
                count++;
            } else {
                remainingClients.push_back(client);
            }
        }

        if (count == 0) {
            cout << "Error: No client found with ID " << clientId << ".\n";
            return;
        }

        if (count > 1) {
            char choice;
            cout << "Warning: Multiple clients (" << count << ") have the same ID: " << clientId << ".\n";
            cout << "Do you still want to delete all of them? (y/n): ";
            cin >> choice;
            if (choice != 'y' && choice != 'Y') {
                cout << "Deletion canceled.\n";
                return;
            }
        }

        ofstream file("clients.txt", ios::trunc);
        if (file.is_open()) {
            for (const Client& client : remainingClients) {
                file << client.getId() << "," << client.getName() << "," << client.getPassword() << "," << client.getBalance() << "\n";
            }
            file.close();
            cout << "Client(s) with ID " << clientId << " have been deleted.\n";
        } else {
            cout << "Error: Unable to open clients.txt for deleting client.\n";
        }
    }

    void removeAllClients() override {
        FileHelper::clearFile("clients.txt");
    }

    void removeAllEmployees() override {
        FileHelper::clearFile("employees.txt");
    }

    void removeAllAdmins() override {
        FileHelper::clearFile("admins.txt");
    }

private:
    bool isClientIdUnique(int id) {
        vector<Client> clients = getAllClients();
        for (const Client& client : clients) {
            if (client.getId() == id) {
                return false;
            }
        }
        return true;
    }

    bool isEmployeeIdUnique(int id) {
        vector<Employee> employees = getAllEmployees();
        for (const Employee& employee : employees) {
            if (employee.getId() == id) {
                return false;
            }
        }
        return true;
    }

    bool isAdminIdUnique(int id) {
        vector<Admin> admins = getAllAdmins();
        for (const Admin& admin : admins) {
            if (admin.getId() == id) {
                return false;
            }
        }
        return true;
    }
};

int getValidIntInput(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value > 0) {
            break;
        } else {
            cout << "Invalid input. Please enter a positive integer.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    cin.ignore(); // Clear the newline character
    return value;
}

double getValidDoubleInput(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value && value > 0) {
            break;
        } else {
            cout << "Invalid input. Please enter a positive number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    cin.ignore(); // Clear the newline character
    return value;
}
string getValidName(const string& prompt) {
    string name;
    while (true) {
        cout << prompt;
        getline(cin, name);
        try {
            Validation::validateName(name);
            break;
        } catch (const invalid_argument& e) {
            cout << "Error: " << e.what() << " Please try again.\n";
        }
    }
    return name;
}

string getValidPassword(const string& prompt) {
    string password;
    while (true) {
        cout << prompt;
        getline(cin, password);
        try {
            Validation::validatePassword(password);
            break;
        } catch (const invalid_argument& e) {
            cout << "Error: " << e.what() << " Please try again.\n";
        }
    }
    return password;
}

void displayMenu() {
    cout << "\n--- Menu ---\n";
    cout << "1. Add Client\n";
    cout << "2. Add Employee\n";
    cout << "3. Add Admin\n";
    cout << "4. View Clients\n";
    cout << "5. View Employees\n";
    cout << "6. View Admins\n";
    cout << "7. Delete Client\n";
    cout << "8. Remove All Clients\n";
    cout << "9. Remove All Employees\n";
    cout << "10. Remove All Admins\n";
    cout << "11. Exit\n";
    cout << "Choose an option: ";
}

int main() {
    FileManager fileManager;
    int choice;

    while (true) {
        displayMenu();
        choice = getValidIntInput("");

        switch (choice) {
            case 1: {
                int id = getValidIntInput("Enter Client ID: ");
                string name = getValidName("Enter Client Name: ");
                string password = getValidPassword("Enter Client Password: ");
               
            
                double balance = getValidDoubleInput("Enter Client Balance: ");

                try {
                    Client newClient(id, name, password, balance);
                    fileManager.addClient(newClient);
                } catch (const invalid_argument& e) {
                    cout << e.what() << endl;
                }
                break;
            }

            case 2: {
                int id = getValidIntInput("Enter Employee ID: ");
               string name = getValidName("Enter Employee Name: ");

               string password = getValidPassword("Enter Employee Password: ");
               
               double salary = getValidDoubleInput("Enter Employee Salary: ");

                try {
                    Employee newEmployee(id, name, password, salary);
                    fileManager.addEmployee(newEmployee);
                } catch (const invalid_argument& e) {
                    cout << e.what() << endl;
                }
                break;
            }

            case 3: {
                int id = getValidIntInput("Enter Admin ID: ");
                string name = getValidName("Enter Admin Name: ");

               string password = getValidPassword("Enter Admin Password: ");
               
                double salary = getValidDoubleInput("Enter Admin Salary: ");

                try {
                    Admin newAdmin(id, name, password, salary);
                    fileManager.addAdmin(newAdmin);
                } catch (const invalid_argument& e) {
                    cout << e.what() << endl;
                }
                break;
            }

            case 4: {
                cout << "\n--- Clients ---\n";
                vector<Client> clients = fileManager.getAllClients();
                for (const Client& client : clients) {
                    client.display();
                }
                break;
            }

            case 5: {
                cout << "\n--- Employees ---\n";
                vector<Employee> employees = fileManager.getAllEmployees();
                for (const Employee& employee : employees) {
                    employee.display();
                }
                break;
            }

            case 6: {
                cout << "\n--- Admins ---\n";
                vector<Admin> admins = fileManager.getAllAdmins();
                for (const Admin& admin : admins) {
                    admin.display();
                }
                break;
            }

            case 7: {
                int clientId = getValidIntInput("Enter Client ID to delete: ");
                fileManager.deleteClient(clientId);
                break;
            }

            case 8:
                fileManager.removeAllClients();
                break;

            case 9:
                fileManager.removeAllEmployees();
                break;

            case 10:
                fileManager.removeAllAdmins();
                break;

            case 11:
                cout << "Exiting...\n";
                return 0;

            default:
                cout << "Invalid option. Please try again.\n";
        }
    }

    return 0;
}
