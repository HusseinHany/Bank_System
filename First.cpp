#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <climits>
#include <fstream>
#include <chrono>
#include <thread>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <regex>
#include <memory>
#include <sstream>

// Optional secure password input (Windows only)
#ifndef USE_SECURE_PASSWORD
#define USE_SECURE_PASSWORD 0
#endif

#if USE_SECURE_PASSWORD && defined(_WIN32)
#include <conio.h>
#endif

using namespace std;

// Forward declarations
class Person;
class Client;
class Employee;
class Admin;
class FileManager;
class FileHelper;
class Validation;
class Logger;
class Screens;
class TransactionStore;

// Function prototypes
int getValidIntInput(const string& prompt);
double getValidDoubleInput(const string& prompt, double min);
string getValidName(const string& prompt);
string getValidPassword(const string& prompt);
#if USE_SECURE_PASSWORD && defined(_WIN32)
string getSecurePassword(const string& prompt) {
    cout << prompt;
    string password;
    char ch;
    while ((ch = _getch()) != '\r') { // '\r' is the Enter key
        if (ch == '\b') { // Handle backspace
            if (!password.empty()) {
                cout << "\b \b";
                password.pop_back();
            }
        } else {
            password.push_back(ch);
            cout << '*';
        }
    }
    cout << endl;
    return password;
}
#else
string getSecurePassword(const string& prompt) {
    // Fallback to regular input on non-Windows platforms
    cout << prompt;
    string password;
    getline(cin, password);
    return password;
}
#endif

// Runtime configurable settings (replacing hardcoded constants)
double MIN_CLIENT_BALANCE = 1500.0;
double MIN_EMPLOYEE_SALARY = 5000.0;
const int MAX_LOGIN_ATTEMPTS = 10;

class Logger {
private:
    static void logToFile(const string& message) {
        ofstream logFile("bank_activity.log", ios::app);
        time_t now = time(nullptr);
        char timestamp[26];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
        
        if (logFile.is_open()) {
            logFile << "[" << timestamp << "] " << message << endl;
            logFile.close();
        }
    }

public:
    static void logActivity(const string& activity, int userId, const string& userType) {
        string message = userType + " (ID: " + to_string(userId) + ") " + activity;
        logToFile(message);
    }
    
    static void logError(const string& error) {
        logToFile("ERROR: " + error);
    }
    
    static void logSystemMessage(const string& message) {
        logToFile("SYSTEM: " + message);
    }
};

// Utility functions
string readLineTrimmed() {
    string line;
    if (getline(cin, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        return line;
    }
    return ""; // Return empty string if EOF or error
}

// TransactionStore class for persistent transaction history
class TransactionStore {
public:
    static void saveTransaction(int clientId, const string& details) {
        ofstream file("transactions.txt", ios::app);
        if (file.is_open()) {
            time_t now = time(nullptr);
            char timestamp[26];
            strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
            
            file << clientId << "|" << timestamp << "|" << details << endl;
            file.close();
        } else {
            Logger::logError("Unable to open transactions.txt for writing");
        }
    }
    
    static vector<string> getTransactionHistory(int clientId) {
        vector<string> history;
        ifstream file("transactions.txt");
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                size_t firstPipe = line.find('|');
                if (firstPipe != string::npos) {
                    string idStr = line.substr(0, firstPipe);
                    try {
                        int id = stoi(idStr);
                        if (id == clientId) {
                            size_t secondPipe = line.find('|', firstPipe + 1);
                            if (secondPipe != string::npos) {
                                string timestamp = line.substr(firstPipe + 1, secondPipe - firstPipe - 1);
                                string details = line.substr(secondPipe + 1);
                                history.push_back("[" + timestamp + "] " + details);
                            }
                        }
                    } catch (const exception& e) {
                        Logger::logError("Malformed transaction line: " + line);
                        continue;
                    }
                }
            }
            file.close();
        }
        return history;
    }
};

class Validation {
public:
    static void validateName(const string& name) {
        if (name.empty() || name.length() < 3) {
            throw invalid_argument("Name must be at least 3 characters long.");
        }
    }

    static void validatePassword(const string& password) {
        if (password.length() < 6) {
            throw invalid_argument("Password must be at least 6 characters long.");
        }
    }
    
    static void validateId(int id) {
        if (id <= 0) {
            throw invalid_argument("ID must be a positive number.");
        }
    }
    
    static void validateAmount(double amount, double minimum) {
        if (amount < minimum) {
            throw invalid_argument("Amount must be at least " + to_string(minimum) + ".");
        }
    }
    
    static bool isValidEmail(const string& email) {
        return email.find('@') != string::npos && email.find('.') != string::npos;
    }
};

// Base Person class
class Person {
protected:
    int id;
    string name;
    string password;
    string email;

public:
    Person() : id(0), name(""), password(""), email("") {}
    Person(int id, string name, string password, string email = "") : id(id), name(name), password(password), email(email) {}
    
    int getId() const { return id; }
    string getName() const { return name; }
    string getPassword() const { return password; }
    string getEmail() const { return email; }
    
    void setName(const string& newName) { name = newName; }
    void setPassword(const string& newPassword) { password = newPassword; }
    void setEmail(const string& newEmail) { email = newEmail; }
    
    virtual void display() const {
        cout << "ID: " << id << ", Name: " << name << ", Email: " << email << endl;
    }
    
    virtual ~Person() = default;
};

// Client class
class Client : public Person {
private:
    double balance;
    vector<string> transactions;
    
public:
    Client() : Person(), balance(0.0) {}
    Client(int id, string name, string password, double balance, string email = "") 
        : Person(id, name, password, email), balance(balance) {}
    
    double getBalance() const { return balance; }
    
    void setBalance(double newBalance) { 
        balance = newBalance; 
        recordTransaction("Balance updated to $" + to_string(balance));
    }
    
    void deposit(double amount) {
        balance += amount;
        recordTransaction("Deposited $" + to_string(amount) + ". New balance: $" + to_string(balance));
    }
    
    bool withdraw(double amount) {
        if (amount > balance || (balance - amount) < MIN_CLIENT_BALANCE) {
            return false;
        }
        balance -= amount;
        recordTransaction("Withdrew $" + to_string(amount) + ". New balance: $" + to_string(balance));
        return true;
    }
    
    void recordTransaction(const string& details) {
        // Record to persistent storage
        TransactionStore::saveTransaction(id, details);
        
        // Also log the activity
        Logger::logActivity(details, id, "Client");
    }
    
    vector<string> getTransactionHistory() const {
        // Load transaction history from persistent storage
        return TransactionStore::getTransactionHistory(id);
    }
    
    void display() const override {
        Person::display();
        cout << "Balance: $" << fixed << setprecision(2) << balance << endl;
    }
};

// Employee class
class Employee : public Person {
private:
    double salary;
    
public:
    Employee() : Person(), salary(0.0) {}
    Employee(int id, string name, string password, double salary, string email = "") : Person(id, name, password, email), salary(salary) {}
    
    double getSalary() const { return salary; }
    void setSalary(double newSalary) { salary = newSalary; }
    
    void display() const override {
        Person::display();
        cout << "Salary: $" << fixed << setprecision(2) << salary << endl;
    }
};

// Admin class
class Admin : public Person {
private:
    string position;
    
public:
    Admin() : Person(), position("") {}
    Admin(int id, string name, string password, string position = "The Owner", string email = "")  : Person(id, name, password, email), position(position) {}
    
    string getPosition() const { return position; }
    void setPosition(const string& newPosition) { position = newPosition; }
    
    void display() const override {
        Person::display();
        cout << "Position: " << position << endl;
    }
};

// FileHelper class
class FileHelper {
public:
    static void saveToFile(const string& filename, const string& data) {
        ofstream file(filename, ios::app);
        if (file.is_open()) {
            file << data << endl;
            file.close();
        } else {
            Logger::logError("Unable to open file: " + filename);
            throw runtime_error("Unable to open file: " + filename);
        }
    }
    
    static void clearFile(const string& filename) {
        ofstream file(filename, ios::trunc);
        file.close();
    }
    
    static void saveClient(const Client& client) {
        string data = to_string(client.getId()) + "," +  client.getName() + "," +  client.getPassword() + "," + to_string(client.getBalance()) + "," +client.getEmail();
        saveToFile("clients.txt", data);
    }
    
    static void saveEmployee(const Employee& employee) {
        string data = to_string(employee.getId()) + "," + employee.getName() + "," +  employee.getPassword() + "," + to_string(employee.getSalary()) + "," +employee.getEmail();
        saveToFile("employees.txt", data);
    }
    
    static vector<Client> parseCSVLine(const string& line, vector<string>& parts) {
        parts.clear();
        stringstream ss(line);
        string item;
        
        while (getline(ss, item, ',')) {
            // Trim whitespace
            item.erase(0, item.find_first_not_of(" \t\r\n"));
            item.erase(item.find_last_not_of(" \t\r\n") + 1);
            parts.push_back(item);
        }
        return {};
    }
    
    static vector<Client> getClients() {
        vector<Client> clients;
        ifstream file("clients.txt");
        if (file.is_open()) {
            string line;
            int lineNumber = 0;
            while (getline(file, line)) {
                lineNumber++;
                if (line.empty()) continue;
                
                vector<string> parts;
                parseCSVLine(line, parts);
                
                if (parts.size() < 4) {
                    Logger::logError("Malformed client line " + to_string(lineNumber) + ": " + line);
                    continue;
                }
                
                try {
                    string email = parts.size() > 4 ? parts[4] : "";
                    
                    // Guard against overflow
                    int id = stoi(parts[0]);
                    double balance = stod(parts[3]);
                    
                    if (id < 0 || balance < 0) {
                        Logger::logError("Invalid negative values in line " + to_string(lineNumber) + ": " + line);
                        continue;
                    }
                    
                    clients.emplace_back(id, parts[1], parts[2], balance, email);
                } catch (const exception& e) {
                    Logger::logError("Error parsing client line " + to_string(lineNumber) + ": " + line + " - " + e.what());
                    continue;
                }
            }
            file.close();
        }
        return clients;
    }
    
    static vector<Employee> getEmployees() {
        vector<Employee> employees;
        ifstream file("employees.txt");
        if (file.is_open()) {
            string line;
            int lineNumber = 0;
            while (getline(file, line)) {
                lineNumber++;
                if (line.empty()) continue;
                
                vector<string> parts;
                parseCSVLine(line, parts);
                
                if (parts.size() < 4) {
                    Logger::logError("Malformed employee line " + to_string(lineNumber) + ": " + line);
                    continue;
                }
                
                try {
                    string email = parts.size() > 4 ? parts[4] : "";
                    
                    int id = stoi(parts[0]);
                    double salary = stod(parts[3]);
                    
                    if (id < 0 || salary < 0) {
                        Logger::logError("Invalid negative values in employee line " + to_string(lineNumber) + ": " + line);
                        continue;
                    }
                    
                    employees.emplace_back(id, parts[1], parts[2], salary, email);
                } catch (const exception& e) {
                    Logger::logError("Error parsing employee line " + to_string(lineNumber) + ": " + line + " - " + e.what());
                    continue;
                }
            }
            file.close();
        }
        return employees;
    }
    
    static vector<Admin> getAdmins() {
        vector<Admin> admins;
        ifstream file("admins.txt");
        if (file.is_open()) {
            string line;
            int lineNumber = 0;
            while (getline(file, line)) {
                lineNumber++;
                if (line.empty()) continue;
                
                vector<string> parts;
                parseCSVLine(line, parts);
                
                if (parts.size() < 3) {
                    Logger::logError("Malformed admin line " + to_string(lineNumber) + ": " + line);
                    continue;
                }
                
                try {
                    string position = parts.size() > 3 ? parts[3] : "OWNER";
                    string email = parts.size() > 4 ? parts[4] : "";
                    
                    int id = stoi(parts[0]);
                    if (id < 0) {
                        Logger::logError("Invalid negative ID in admin line " + to_string(lineNumber) + ": " + line);
                        continue;
                    }
                    
                    admins.emplace_back(id, parts[1], parts[2], position, email);
                } catch (const exception& e) {
                    Logger::logError("Error parsing admin line " + to_string(lineNumber) + ": " + line + " - " + e.what());
                    continue;
                }
            }
            file.close();
        }
        return admins;
    }
};

// FileManager class
class FileManager {
public:
    bool isClientIdUnique(int id) const {
        vector<Client> clients = FileHelper::getClients();
        for (const Client& client : clients) {
            if (client.getId() == id) {
                return false;
            }
        }
        return true;
    }
    
    bool isEmployeeIdUnique(int id) const {
        vector<Employee> employees = FileHelper::getEmployees();
        for (const Employee& employee : employees) {
            if (employee.getId() == id) {
                return false;
            }
        }
        return true;
    }
    
    bool isAdminIdUnique(int id) const {
        vector<Admin> admins = FileHelper::getAdmins();
        for (const Admin& admin : admins) {
            if (admin.getId() == id) {
                return false;
            }
        }
        return true;
    }
    
    void addClient(const Client& client) {
        if (!isClientIdUnique(client.getId())) {
            throw invalid_argument("Client ID already exists.");
        }
        FileHelper::saveClient(client);
        Logger::logActivity("Added new client", client.getId(), "System");
    }
    
    void addEmployee(const Employee& employee) {
        if (!isEmployeeIdUnique(employee.getId())) {
            throw invalid_argument("Employee ID already exists.");
        }
        string data = to_string(employee.getId()) + "," + employee.getName() + "," +  employee.getPassword() + "," + to_string(employee.getSalary()) + "," +employee.getEmail();
        FileHelper::saveToFile("employees.txt", data);
        Logger::logActivity("Added new employee", employee.getId(), "System");
    }
    
    void addAdmin(const Admin& admin) {
        if (!isAdminIdUnique(admin.getId())) {
            throw invalid_argument("Admin ID already exists.");
        }
        string data = to_string(admin.getId()) + "," + admin.getName() + "," +  admin.getPassword() + "," +  admin.getPosition() + "," +admin.getEmail();
        FileHelper::saveToFile("admins.txt", data);
        Logger::logActivity("Added new admin", admin.getId(), "System");
    }
    
    // Centralized update methods to eliminate repeated clear/append cycles
    void updateClient(const Client& updatedClient) {
        vector<Client> clients = FileHelper::getClients();
        bool found = false;
        
        for (Client& client : clients) {
            if (client.getId() == updatedClient.getId()) {
                client = updatedClient;
                found = true;
                break;
            }
        }
        
        if (found) {
            FileHelper::clearFile("clients.txt");
            for (const Client& client : clients) {
                FileHelper::saveClient(client);
            }
            Logger::logActivity("Updated client", updatedClient.getId(), "System");
        } else {
            Logger::logError("Attempted to update non-existent client ID: " + to_string(updatedClient.getId()));
        }
    }
    
    void updateEmployee(const Employee& updatedEmployee) {
        vector<Employee> employees = FileHelper::getEmployees();
        bool found = false;
        
        for (Employee& employee : employees) {
            if (employee.getId() == updatedEmployee.getId()) {
                employee = updatedEmployee;
                found = true;
                break;
            }
        }
        
        if (found) {
            FileHelper::clearFile("employees.txt");
            for (const Employee& employee : employees) {
                FileHelper::saveEmployee(employee);
            }
            Logger::logActivity("Updated employee", updatedEmployee.getId(), "System");
        } else {
            Logger::logError("Attempted to update non-existent employee ID: " + to_string(updatedEmployee.getId()));
        }
    }
};

// ClientManager class
class ClientManager {
public:
    static void displayClientMenu() {
        cout << "\n--- Client Menu ---\n";
        cout << "1. Check Balance\n";
        cout << "2. Deposit\n";
        cout << "3. Withdraw\n";
        cout << "4. Transfer to Another Client\n";
        cout << "5. View Transaction History\n";
        cout << "6. Update Personal Information\n";
        cout << "7. Logout\n";
        cout << "Choose an option: ";
    }
    
    static Client login(int id, string password) {
        int attempts = 0;
        
        while (attempts < MAX_LOGIN_ATTEMPTS) {
            vector<Client> clients = FileHelper::getClients();
            for (const Client& client : clients) {
                if (client.getId() == id && client.getPassword() == password) {
                    Logger::logActivity("Logged in successfully", id, "Client");
                    cout << "\nWelcome, " << client.getName() << "!\n";
                    return client;
                }
            }
            
            attempts++;
            if (attempts < MAX_LOGIN_ATTEMPTS) {
                cout << "Invalid ID or password. You have " << (MAX_LOGIN_ATTEMPTS - attempts) << " attempts remaining.\n";
                cout << "Enter Password: ";
                cin >> password;
            }
        }
        
        Logger::logError("Failed login attempt for client ID: " + to_string(id));
        cout << "Maximum login attempts reached. Please try again later.\n";
        
        this_thread::sleep_for(chrono::seconds(2));
        
        return Client();
    }
    
    static void checkBalance(const Client& client) {
        cout << "Current Balance: $" << fixed << setprecision(2) << client.getBalance() << endl;
        Logger::logActivity("Checked balance", client.getId(), "Client");
    }
    
    static Client deposit(Client client) {
        double amount = getValidDoubleInput("Enter amount to deposit: $", 0.01);
        client.deposit(amount);
        
        vector<Client> clients = FileHelper::getClients();
        FileHelper::clearFile("clients.txt");
        for (const Client& c : clients) {
            if (c.getId() == client.getId()) {
                FileHelper::saveClient(client);
            } else {
                FileHelper::saveClient(c);
            }
        }
        
        cout << "Deposit successful. New balance: $" << fixed << setprecision(2) << client.getBalance() << endl;
        return client;
    }
    
    static Client withdraw(Client client) {
        double amount = getValidDoubleInput("Enter amount to withdraw: $", 0.01);
        
        if (client.getBalance() - amount < MIN_CLIENT_BALANCE) {
            cout << "Error: Withdrawal would drop balance below minimum requirement of $" 
                 << MIN_CLIENT_BALANCE << endl;
            return client;
        }
        
        if (client.withdraw(amount)) {
            vector<Client> clients = FileHelper::getClients();
            FileHelper::clearFile("clients.txt");
            for (const Client& c : clients) {
                if (c.getId() == client.getId()) {
                    FileHelper::saveClient(client);
                } else {
                    FileHelper::saveClient(c);
                }
            }
            
            cout << "Withdrawal successful. New balance: $" << fixed << setprecision(2) << client.getBalance() << endl;
        } else {
            cout << "Insufficient funds.\n";
        }
        
        return client;
    }
    
    static Client transfer(Client sender) {
        int recipientId = getValidIntInput("Enter recipient client ID: ");
        vector<Client> clients = FileHelper::getClients();
        
        Client* recipient = nullptr;
        for (Client& client : clients) {
            if (client.getId() == recipientId) {
                recipient = &client;
                break;
            }
        }
        
        if (!recipient) {
            cout << "Recipient client not found.\n";
            return sender;
        }
        
        if (sender.getId() == recipientId) {
            cout << "Cannot transfer to yourself.\n";
            return sender;
        }
        
        double amount = getValidDoubleInput("Enter amount to transfer: $", 0.01);
        
        if (sender.getBalance() - amount < MIN_CLIENT_BALANCE) {
            cout << "Error: Transfer would drop balance below minimum requirement of $" 
                 << MIN_CLIENT_BALANCE << endl;
            return sender;
        }
        
        if (sender.withdraw(amount)) {
            recipient->deposit(amount);
            
            // Record transfer transactions for both parties
            string senderDetails = "Transferred $" + to_string(amount) + " to " + recipient->getName() + " (ID: " + to_string(recipientId) + ")";
            string recipientDetails = "Received $" + to_string(amount) + " from " + sender.getName() + " (ID: " + to_string(sender.getId()) + ")";
            
            TransactionStore::saveTransaction(sender.getId(), senderDetails);
            TransactionStore::saveTransaction(recipientId, recipientDetails);
            
            // Use centralized update methods
            FileManager fileManager;
            fileManager.updateClient(sender);
            fileManager.updateClient(*recipient);
            
            cout << "Transfer successful. New balance: $" << fixed << setprecision(2) << sender.getBalance() << endl;
            Logger::logActivity("Transferred $" + to_string(amount) + " to client ID: " + to_string(recipientId), 
                               sender.getId(), "Client");
        } else {
            cout << "Insufficient funds.\n";
        }
        
        return sender;
    }
    
    static void viewTransactionHistory(const Client& client) {
        vector<string> history = client.getTransactionHistory();
        if (history.empty()) {
            cout << "No transaction history available.\n";
            return;
        }
        
        cout << "\n--- Transaction History ---\n";
        for (const string& transaction : history) {
            cout << transaction << endl;
        }
        
        Logger::logActivity("Viewed transaction history", client.getId(), "Client");
    }
    
    static Client updatePersonalInfo(Client client) {
        cout << "\n--- Update Personal Information ---\n";
        cout << "1. Update Name\n";
        cout << "2. Update Password\n";
        cout << "3. Update Email\n";
        cout << "4. Return to Main Menu\n";
        cout << "Choose an option: ";
        
        int choice = getValidIntInput("");
        
        switch (choice) {
            case 1: {
                string newName = getValidName("Enter new name: ");
                client.setName(newName);
                cout << "Name updated successfully.\n";
                Logger::logActivity("Updated name", client.getId(), "Client");
                break;
            }
            case 2: {
                string newPassword = getValidPassword("Enter new password: ");
                client.setPassword(newPassword);
                cout << "Password updated successfully.\n";
                Logger::logActivity("Updated password", client.getId(), "Client");
                break;
            }
            case 3: {
                string newEmail;
                while (true) {
                    cout << "Enter new email: ";
                    cin >> newEmail;
                    if (Validation::isValidEmail(newEmail)) {
                        client.setEmail(newEmail);
                        cout << "Email updated successfully.\n";
                        Logger::logActivity("Updated email", client.getId(), "Client");
                        break;
                    } else {
                        cout << "Invalid email format. Please try again.\n";
                    }
                }
                break;
            }
            case 4:
                return client;
            default:
                cout << "Invalid option. Please try again.\n";
        }
        vector<Client> clients = FileHelper::getClients();
        FileHelper::clearFile("clients.txt");
        for (const Client& c : clients) {
            if (c.getId() == client.getId()) {
                FileHelper::saveClient(client);
            } else {
                FileHelper::saveClient(c);
            }
        }
        
        return client;
    }
    
    static bool clientOptions(Client client) {
        int choice;
        while (true) {
            displayClientMenu();
            choice = getValidIntInput("");
            switch (choice) {
                case 1:
                    checkBalance(client);
                    break;
                case 2:
                    client = deposit(client);
                    break;
                case 3:
                    client = withdraw(client);
                    break;
                case 4:
                    client = transfer(client);
                    break;
                case 5:
                    viewTransactionHistory(client);
                    break;
                case 6:
                    client = updatePersonalInfo(client);
                    break;
                case 7:
                    Logger::logActivity("Logged out", client.getId(), "Client");
                    cout << "Logging out...\n";
                    return false;
                default:
                    cout << "Invalid option. Please try again.\n";
            }
        }
    }
};

// EmployeeManager class
class EmployeeManager {
public:
    static void displayEmployeeMenu() {
        cout << "\n--- Employee Menu ---\n";
        cout << "1. Add New Client\n";
        cout << "2. List All Clients\n";
        cout << "3. Search For Client\n";
        cout << "4. Edit Client Info\n";
        cout << "5. Process Client Transactions\n";
        cout << "6. View Client Transaction History\n";
        cout << "7. Logout\n";
        cout << "Choose an option: ";
    }

    static Employee login(int id, string password) {
        int attempts = 0;
        
        while (attempts < MAX_LOGIN_ATTEMPTS) {
            vector<Employee> employees = FileHelper::getEmployees();
            for (const Employee& employee : employees) {
                if (employee.getId() == id && employee.getPassword() == password) {
                    Logger::logActivity("Logged in successfully", id, "Employee");
                    cout << "\nWelcome, " << employee.getName() << "!\n";
                    return employee;
                }
            }
            
            attempts++;
            if (attempts < MAX_LOGIN_ATTEMPTS) {
                cout << "Invalid ID or password. You have " << (MAX_LOGIN_ATTEMPTS - attempts) << " attempts remaining.\n";
                cout << "Enter Password: ";
                cin >> password;
            }
        }
        
        Logger::logError("Failed login attempt for employee ID: " + to_string(id));
        cout << "Maximum login attempts reached. Please try again later.\n";
        
        this_thread::sleep_for(chrono::seconds(3));
        
        return Employee();
    }

    static void addNewClient(const Employee& employee) {
        try {
            int id = getValidIntInput("Enter Client ID: ");
            Validation::validateId(id);
            
            string name = getValidName("Enter Client Name: ");
            string password = getValidPassword("Enter Client Password: ");
            double balance = getValidDoubleInput("Enter Initial Balance: $", MIN_CLIENT_BALANCE);
            
            string email;
            cout << "Enter Client Email (optional, press Enter to skip): ";
            getline(cin, email);
            if (!email.empty() && !Validation::isValidEmail(email)) {
                cout << "Invalid email format. Email will not be saved.\n";
                email = "";
            }
            
            FileManager fileManager;
            if (!fileManager.isClientIdUnique(id)) {
                cout << "Error: Client ID " << id << " already exists.\n";
                return;
            }
            
            Client newClient(id, name, password, balance, email);
            fileManager.addClient(newClient);
            
            cout << "Client added successfully.\n";
            Logger::logActivity("Added new client with ID: " + to_string(id), employee.getId(), "Employee");
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            Logger::logError(string("Error adding client: ") + e.what());
        }
    }

    static void listAllClients() {
        vector<Client> clients = FileHelper::getClients();
        if (clients.empty()) {
            cout << "No clients found.\n";
            return;
        }
        
        cout << "\n--- Client List ---\n";
        cout << setw(5) << "ID" << setw(20) << "Name" << setw(30) << "Email" << setw(15) << "Balance" << endl;
        cout << string(70, '-') << endl;
        
        for (const Client& client : clients) {
            cout << setw(5) << client.getId() 
                 << setw(20) << client.getName()
                 << setw(30) << client.getEmail()
                 << setw(15) << fixed << setprecision(2) << client.getBalance() << endl;
        }
    }

    static void searchForClient(int clientId) {
        vector<Client> clients = FileHelper::getClients();
        for (const Client& client : clients) {
            if (client.getId() == clientId) {
                cout << "Client found:\n";
                client.display();
                return;
            }
        }
        cout << "Client not found.\n";
    }

    static void editClientInfo() {
        int clientId = getValidIntInput("Enter Client ID: ");
        vector<Client> clients = FileHelper::getClients();
        bool found = false;
        
        for (Client& client : clients) {
            if (client.getId() == clientId) {
                cout << "Current client information:\n";
                client.display();
                
                cout << "\n--- Edit Client Information ---\n";
                cout << "1. Edit Name\n";
                cout << "2. Edit Password\n";
                cout << "3. Edit Balance\n";
                cout << "4. Edit Email\n";
                cout << "5. Return to Employee Menu\n";
                cout << "Choose an option: ";
                
                int choice = getValidIntInput("");
                
                switch (choice) {
                    case 1: {
                        string newName = getValidName("Enter new name: ");
                        client.setName(newName);
                        cout << "Name updated successfully.\n";
                        break;
                    }
                    case 2: {
                        string newPassword = getValidPassword("Enter new password: ");
                        client.setPassword(newPassword);
                        cout << "Password updated successfully.\n";
                        break;
                    }
                    case 3: {
                        double newBalance = getValidDoubleInput("Enter new balance: $", MIN_CLIENT_BALANCE);
                        client.setBalance(newBalance);
                        cout << "Balance updated successfully.\n";
                        break;
                    }
                    case 4: {
                        string newEmail;
                        while (true) {
                            cout << "Enter new email: ";
                            cin >> newEmail;
                            if (Validation::isValidEmail(newEmail)) {
                                client.setEmail(newEmail);
                                cout << "Email updated successfully.\n";
                                break;
                            } else {
                                cout << "Invalid email format. Please try again.\n";
                            }
                        }
                        break;
                    }
                    case 5:
                        return;
                    default:
                        cout << "Invalid option. Please try again.\n";
                        return;
                }
                
                found = true;
                break;
            }
        }

        if (found) {
            FileHelper::clearFile("clients.txt");
            for (const Client& client : clients) {
                FileHelper::saveClient(client);
            }
            cout << "Client updated successfully.\n";
            Logger::logActivity("Updated client with ID: " + to_string(clientId), 0, "Employee");
        } else {
            cout << "Client not found.\n";
        }
    }
    
    static void processClientTransactions() {
        int clientId = getValidIntInput("Enter Client ID: ");
        vector<Client> clients = FileHelper::getClients();
        
        Client* targetClient = nullptr;
        for (Client& client : clients) {
            if (client.getId() == clientId) {
                targetClient = &client;
                break;
            }
        }
        
        if (!targetClient) {
            cout << "Client not found.\n";
            return;
        }
        
        cout << "Client found: " << targetClient->getName() << ", Current Balance: $" 
             << fixed << setprecision(2) << targetClient->getBalance() << endl;
        
        cout << "\n--- Process Transaction ---\n";
        cout << "1. Deposit\n";
        cout << "2. Withdraw\n";
        cout << "3. Return to Employee Menu\n";
        cout << "Choose an option: ";
        
        int choice = getValidIntInput("");
        
        switch (choice) {
            case 1: {
                double amount = getValidDoubleInput("Enter amount to deposit: $", 0.01);
                targetClient->deposit(amount);
                cout << "Deposit successful. New balance: $" << fixed << setprecision(2) << targetClient->getBalance() << endl;
                break;
            }
            case 2: {
                double amount = getValidDoubleInput("Enter amount to withdraw: $", 0.01);
                if (targetClient->getBalance() - amount < MIN_CLIENT_BALANCE) {
                    cout << "Error: Withdrawal would drop balance below minimum requirement of $" 
                         << MIN_CLIENT_BALANCE << endl;
                    return;
                }
                
                if (targetClient->withdraw(amount)) {
                    cout << "Withdrawal successful. New balance: $" << fixed << setprecision(2) << targetClient->getBalance() << endl;
                } else {
                    cout << "Insufficient funds.\n";
                }
                break;
            }
            case 3:
                return;
            default:
                cout << "Invalid option. Please try again.\n";
                return;
        }
        
        FileHelper::clearFile("clients.txt");
        for (const Client& client : clients) {
            FileHelper::saveClient(client);
        }
        
        Logger::logActivity("Processed transaction for client ID: " + to_string(clientId), 0, "Employee");
    }
    
    static void viewClientTransactionHistory() {
        int clientId = getValidIntInput("Enter Client ID: ");
        vector<Client> clients = FileHelper::getClients();
        
        for (const Client& client : clients) {
            if (client.getId() == clientId) {
                vector<string> history = client.getTransactionHistory();
                if (history.empty()) {
                    cout << "No transaction history available for this client.\n";
                } else {
                    cout << "\n--- Transaction History for " << client.getName() << " ---\n";
                    for (const string& transaction : history) {
                        cout << transaction << endl;
                    }
                }
                return;
            }
        }
        
        cout << "Client not found.\n";
    }

    static bool employeeOptions(Employee employee) {
        int choice;
        while (true) {
            displayEmployeeMenu();
            choice = getValidIntInput("");
            switch (choice) {
                case 1:
                    addNewClient(employee);
                    break;
                case 2:
                    listAllClients();
                    break;
                case 3: {
                    int clientId = getValidIntInput("Enter Client ID: ");
                    searchForClient(clientId);
                    break;
                }
                case 4:
                    editClientInfo();
                    break;
                case 5:
                    processClientTransactions();
                    break;
                case 6:
                    viewClientTransactionHistory();
                    break;
                case 7:
                    Logger::logActivity("Logged out", employee.getId(), "Employee");
                    cout << "Logging out...\n";
                    return false;
                default:
                    cout << "Invalid option. Please try again.\n";
            }
        }
    }
};

// AdminManager class
class AdminManager {
public:
    static void displayAdminMenu() {
        cout << "\n--- Admin Menu ---\n";
        cout << "1. Add New Employee\n";
        cout << "2. List All Employees\n";
        cout << "3. Search For Employee\n";
        cout << "4. Edit Employee Info\n";
        cout << "5. Add New Admin\n";
        cout << "6. System Settings\n";
        cout << "7. View System Logs\n";
        cout << "8. Logout\n";
        cout << "Choose an option: ";
    }

    static Admin login(int id, string password) {
        int attempts = 0;
        
        while (attempts < MAX_LOGIN_ATTEMPTS) {
            vector<Admin> admins = FileHelper::getAdmins();
            for (const Admin& admin : admins) {
                if (admin.getId() == id && admin.getPassword() == password) {
                    Logger::logActivity("Logged in successfully", id, "Admin");
                    cout << "\nWelcome, " << admin.getName() << " (" << admin.getPosition() << ")!\n";
                    return admin;
                }
            }
            
            attempts++;
            if (attempts < MAX_LOGIN_ATTEMPTS) {
                cout << "Invalid ID or password. You have " << (MAX_LOGIN_ATTEMPTS - attempts) << " attempts remaining.\n";
                cout << "Enter Password: ";
                cin >> password;
            }
                    }

        Logger::logError("Failed login attempt for admin ID: " + to_string(id));
        cout << "Maximum login attempts reached. Please try again later.\n";
        
        this_thread::sleep_for(chrono::seconds(3));
        
        return Admin();
    }

    static void addNewEmployee() {
        try {
            int id = getValidIntInput("Enter Employee ID: ");
            Validation::validateId(id);
            
            string name = getValidName("Enter Employee Name: ");
            string password = getValidPassword("Enter Employee Password: ");
            double salary = getValidDoubleInput("Enter Employee Salary: $", MIN_EMPLOYEE_SALARY);
            
            string email;
            cout << "Enter Employee Email (optional, press Enter to skip): ";
            getline(cin, email);
            if (!email.empty() && !Validation::isValidEmail(email)) {
                cout << "Invalid email format. Email will not be saved.\n";
                email = "";
            }
            
            FileManager fileManager;
            if (!fileManager.isEmployeeIdUnique(id)) {
                cout << "Error: Employee ID " << id << " already exists.\n";
                return;
            }
            
            Employee newEmployee(id, name, password, salary, email);
            fileManager.addEmployee(newEmployee);
            
            cout << "Employee added successfully.\n";
            Logger::logActivity("Added new employee with ID: " + to_string(id), 0, "Admin");
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            Logger::logError(string("Error adding employee: ") + e.what());
        }
    }

    static void listAllEmployees() {
        vector<Employee> employees = FileHelper::getEmployees();
        if (employees.empty()) {
            cout << "No employees found.\n";
            return;
        }
        
        cout << "\n--- Employee List ---\n";
        cout << setw(5) << "ID" << setw(20) << "Name" << setw(30) << "Email" << setw(15) << "Salary" << endl;
        cout << string(70, '-') << endl;
        
        for (const Employee& employee : employees) {
            cout << setw(5) << employee.getId() 
                 << setw(20) << employee.getName()
                 << setw(30) << employee.getEmail()
                 << setw(15) << fixed << setprecision(2) << employee.getSalary() << endl;
        }
    }

    static void searchForEmployee(int employeeId) {
        vector<Employee> employees = FileHelper::getEmployees();
        for (const Employee& employee : employees) {
            if (employee.getId() == employeeId) {
                cout << "Employee found:\n";
                employee.display();
                return;
            }
        }
        cout << "Employee not found.\n";
    }

    static void editEmployeeInfo() {
        int employeeId = getValidIntInput("Enter Employee ID: ");
        vector<Employee> employees = FileHelper::getEmployees();
        bool found = false;
        
        for (Employee& employee : employees) {
            if (employee.getId() == employeeId) {
                cout << "Current employee information:\n";
                employee.display();
                
                cout << "\n--- Edit Employee Information ---\n";
                cout << "1. Edit Name\n";
                cout << "2. Edit Password\n";
                cout << "3. Edit Salary\n";
                cout << "4. Edit Email\n";
                cout << "5. Return to Admin Menu\n";
                cout << "Choose an option: ";
                
                int choice = getValidIntInput("");
                
                switch (choice) {
                    case 1: {
                        string newName = getValidName("Enter new name: ");
                        employee.setName(newName);
                        cout << "Name updated successfully.\n";
                        break;
                    }
                    case 2: {
                        string newPassword = getValidPassword("Enter new password: ");
                        employee.setPassword(newPassword);
                        cout << "Password updated successfully.\n";
                        break;
                    }
                    case 3: {
                        double newSalary = getValidDoubleInput("Enter new salary: $", MIN_EMPLOYEE_SALARY);
                        employee.setSalary(newSalary);
                        cout << "Salary updated successfully.\n";
                        break;
                    }
                    case 4: {
                        string newEmail;
                        while (true) {
                            cout << "Enter new email: ";
                            cin >> newEmail;
                            if (Validation::isValidEmail(newEmail)) {
                                employee.setEmail(newEmail);
                                cout << "Email updated successfully.\n";
                                break;
                            } else {
                                cout << "Invalid email format. Please try again.\n";
                            }
                        }
                        break;
                    }
                    case 5:
                        return;
                    default:
                        cout << "Invalid option. Please try again.\n";
                        return;
                }
                
                found = true;
                break;
            }
        }

        if (found) {
            FileHelper::clearFile("employees.txt");
            for (const Employee& employee : employees) {
                FileHelper::saveEmployee(employee);
            }
            cout << "Employee updated successfully.\n";
            Logger::logActivity("Updated employee with ID: " + to_string(employeeId), 0, "Admin");
        } else {
            cout << "Employee not found.\n";
        }
    }

    static void addNewAdmin() {
        try {
            int id = getValidIntInput("Enter Admin ID: ");
            Validation::validateId(id);
            
            string name = getValidName("Enter Admin Name: ");
            string password = getValidPassword("Enter Admin Password: ");
            string position = getValidName("Enter Admin Position: ");
            
            string email;
            cout << "Enter Admin Email (optional, press Enter to skip): ";
            getline(cin, email);
            if (!email.empty() && !Validation::isValidEmail(email)) {
                cout << "Invalid email format. Email will not be saved.\n";
                email = "";
            }
            
            FileManager fileManager;
            if (!fileManager.isAdminIdUnique(id)) {
                cout << "Error: Admin ID " << id << " already exists.\n";
                return;
            }
            
            Admin newAdmin(id, name, password, position, email);
            fileManager.addAdmin(newAdmin);
            
            cout << "Admin added successfully.\n";
            Logger::logActivity("Added new admin with ID: " + to_string(id), 0, "Admin");
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            Logger::logError(string("Error adding admin: ") + e.what());
        }
    }

    static void viewSystemLogs() {
        ifstream logFile("bank_activity.log");
        if (!logFile.is_open()) {
            cout << "Unable to open log file.\n";
            return;
        }
        
        cout << "\n--- System Logs ---\n";
        string line;
        while (getline(logFile, line)) {
            cout << line << endl;
        }
        logFile.close();
    }

    static void systemSettings() {
        cout << "\n--- System Settings ---\n";
        cout << "Current Settings:\n";
        cout << "  Minimum Client Balance: $" << fixed << setprecision(2) << MIN_CLIENT_BALANCE << "\n";
        cout << "  Minimum Employee Salary: $" << fixed << setprecision(2) << MIN_EMPLOYEE_SALARY << "\n\n";
        cout << "1. Change Minimum Client Balance\n";
        cout << "2. Change Minimum Employee Salary\n";
        cout << "3. Return to Admin Menu\n";
        cout << "Choose an option: ";
        
        int choice = getValidIntInput("");
        
        switch (choice) {
            case 1: {
                double newMinBalance = getValidDoubleInput("Enter new minimum client balance: $", 0.0);
                double oldBalance = MIN_CLIENT_BALANCE;
                MIN_CLIENT_BALANCE = newMinBalance;
                cout << "Minimum client balance updated from $" << oldBalance << " to $" << newMinBalance << endl;
                Logger::logSystemMessage("Minimum client balance changed from $" + to_string(oldBalance) + " to $" + to_string(newMinBalance));
                break;
            }
            case 2: {
                double newMinSalary = getValidDoubleInput("Enter new minimum employee salary: $", 0.0);
                double oldSalary = MIN_EMPLOYEE_SALARY;
                MIN_EMPLOYEE_SALARY = newMinSalary;
                cout << "Minimum employee salary updated from $" << oldSalary << " to $" << newMinSalary << endl;
                Logger::logSystemMessage("Minimum employee salary changed from $" + to_string(oldSalary) + " to $" + to_string(newMinSalary));
                break;
            }
            case 3:
                return;
            default:
                cout << "Invalid option. Please try again.\n";
        }
    }

    static bool adminOptions(Admin admin) {
        int choice;
        while (true) {
            displayAdminMenu();
            choice = getValidIntInput("");
            switch (choice) {
                case 1:
                    addNewEmployee();
                    break;
                case 2:
                    listAllEmployees();
                    break;
                case 3: {
                    int employeeId = getValidIntInput("Enter Employee ID: ");
                    searchForEmployee(employeeId);
                    break;
                }
                case 4:
                    editEmployeeInfo();
                    break;
                case 5:
                    addNewAdmin();
                    break;
                case 6:
                    systemSettings();
                    break;
                case 7:
                    viewSystemLogs();
                    break;
                case 8:
                    Logger::logActivity("Logged out", admin.getId(), "Admin");
                    cout << "Logging out...\n";
                    return false;
                default:
                    cout << "Invalid option. Please try again.\n";
            }
        }
    }
};

// Function to get valid integer input with overflow protection
int getValidIntInput(const string& prompt) {
    while (true) {
        cout << prompt;
        string input = readLineTrimmed();
        
        // Handle EOF or empty input
        if (cin.eof()) {
            cout << "\nEnd of input detected. Exiting...\n";
            exit(0);
        }
        
        if (input.empty()) {
            cout << "Invalid input. Please enter a positive integer.\n";
            continue;
        }
        
        try {
            // Check for extremely large inputs to prevent overflow
            if (input.length() > 9) { // int max is ~2 billion, 10 digits
                cout << "Number too large. Please enter a smaller positive integer.\n";
                continue;
            }
            
            long long temp = stoll(input);
            if (temp < 0 || temp > INT_MAX) {
                cout << "Invalid input. Please enter a positive integer within valid range.\n";
                continue;
            }
            
            return static_cast<int>(temp);
        } catch (const exception& e) {
            cout << "Invalid input. Please enter a positive integer.\n";
        }
    }
}

// Function to get valid double input with overflow protection
double getValidDoubleInput(const string& prompt, double min) {
    while (true) {
        cout << prompt;
        string input = readLineTrimmed();
        
        // Handle EOF or empty input
        if (cin.eof()) {
            cout << "\nEnd of input detected. Exiting...\n";
            exit(0);
        }
        
        if (input.empty()) {
            cout << "Invalid input. Please enter a number greater than or equal to " << min << ".\n";
            continue;
        }
        
        try {
            // Check for extremely large inputs
            if (input.length() > 20) {
                cout << "Number too large. Please enter a smaller number.\n";
                continue;
            }
            
            double value = stod(input);
            if (value < min) {
                cout << "Invalid input. Please enter a number greater than or equal to " << min << ".\n";
                continue;
            }
            
            return value;
        } catch (const exception& e) {
            cout << "Invalid input. Please enter a valid number greater than or equal to " << min << ".\n";
        }
    }
}

// Function to get valid name input
string getValidName(const string& prompt) {
    string name;
    while (true) {
        cout << prompt;
        getline(cin, name);
        try {
            Validation::validateName(name);
            return name;
        } catch (const invalid_argument& e) {
            cout << e.what() << endl;
        }
    }
}

// Function to get valid password input
string getValidPassword(const string& prompt) {
    string password;
    while (true) {
        cout << prompt;
        password = readLineTrimmed();
        try {
            Validation::validatePassword(password);
            return password;
        } catch (const invalid_argument& e) {
            cout << e.what() << endl;
        }
    }
}

// Screens class to manage user interface
class Screens {
public:
    // Method to display the bank name
    static void bankName() {
        cout << "\n=====================\n";
        cout << "       Byte Masters   \n";
        cout << "=====================\n";
    }

    // Method to display a welcome message
    static void welcome() {
        cout << "\nWelcome to the Banking System!\n";
    }

    // Method to display login options
    static void loginOptions() {
        cout << "\n--- Login Options ---\n";
        cout << "1. Client Login\n";
        cout << "2. Employee Login\n";
        cout << "3. Admin Login\n";
        cout << "4. Exit\n";
        cout << "Choose an option: ";
    }

    // Method to get the user's choice for login type
    static int loginAs() {
        int choice = getValidIntInput("Choose your login type: ");
        return choice;
    }

    // Method to display an invalid option message
    static void invalid(int c) {
        cout << "Invalid option: " << c << ". Please try again.\n";
    }

    // Method to display a logout message
    static void logout() {
        cout << "Logging out...\n";
    }

    // Method to display the login screen for a specific user type
    static void loginScreen(int c) {
        switch (c) {
            case 1:
                cout << "\n--- Client Login ---\n";
                break;
            case 2:
                cout << "\n--- Employee Login ---\n";
                break;
            case 3:
                cout << "\n--- Admin Login ---\n";
                break;
            default:
                cout << "Unknown login type.\n";
                break;
        }
    }

    // Method to run the application
   static void runApp() {
    bankName();
    welcome();
    while (true) {
        loginOptions();
        int choice = loginAs();
        switch (choice) {
            case 1: {
                int clientId = getValidIntInput("Enter Client ID: ");
                string password;
                cout << "Enter Password: ";
                cin >> password;
                Client client = ClientManager::login(clientId, password);
                if (client.getId() != 0) {
                    ClientManager::clientOptions(client);
                }
                break;
            }
            case 2: {
                int employeeId = getValidIntInput("Enter Employee ID: ");
                string password;
                cout << "Enter Password: ";
                cin >> password;
                Employee employee = EmployeeManager::login(employeeId, password);
                if (employee.getId() != 0) {
                    EmployeeManager::employeeOptions(employee);
                }
                break;
            }
            case 3: {
                int adminId = getValidIntInput("Enter Admin ID: ");
                string password;
                cout << "Enter Password: ";
                cin >> password;
                Admin admin = AdminManager::login(adminId, password);
                if (admin.getId() != 0) {
                    AdminManager::adminOptions(admin);
                }
                break;
            }
            case 4:
                cout << "Exiting the system. Goodbye!\n";
                return;
            default:
                invalid(choice);
        }
    }
   }
};


int main() {
    try {
        Screens::runApp();
    } catch (const exception& e) {
        Logger::logError("Unhandled exception in main: " + string(e.what()));
        cout << "An unexpected error occurred. Please check the log file for details.\n";
        return 1;
    } catch (...) {
        Logger::logError("Unknown exception in main");
        cout << "An unknown error occurred. Please check the log file for details.\n";
        return 1;
    }
    return 0;
}
