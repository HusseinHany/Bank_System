#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

class Person {
protected:
    string name;
    int id = 0;
    string password;

public:
    void setName() {
        cout << "Enter name: ";
        getline(cin, name);
    }

    string getName() const {
        return name;
    }

    void setId() {
        cout << "Enter ID: ";
        cin >> id;
        cin.ignore();
    }

    int getId() const {
        return id;
    }

    void setPassword() {
        cout << "Enter password (8-20 characters): ";
        getline(cin, password);
    }

    string getPassword() const {
        return password;
    }
};

class Client : public Person {
private:
    double balance;
    const double MIN_BALANCE = 1500;

public:
    Client() {
        setId();
        setName();
        setPassword();
        setBalance();
    }

    Client(int id, const string& name, const string& password, double balance) {
        this->id = id;
        this->name = name;
        this->password = password;
        this->balance = balance;
    }

    void setBalance() {
        cout << "Enter balance (minimum " << MIN_BALANCE << "): ";
        cin >> balance;
        cin.ignore();
    }

    double getBalance() const {
        return balance;
    }

    void saveToFile() {
        ofstream file("Client.txt", ios::app);
        file << id << "-" << name << "-" << password << "-" << balance << "\n";
        file.close();
    }

    void display() const {
        cout << "ID: " << id << "\nName: " << name << "\nBalance: " << balance << endl;
    }
};

class Employee : public Person {
protected:
    double salary;

public:
    void setSalary() {
        cout << "Enter salary: ";
        cin >> salary;
        cin.ignore();
    }

    double getSalary() const {
        return salary;
    }

    void saveToFile() {
        ofstream file("Employee.txt", ios::app);
        file << id << "-" << name << "-" << password << "-" << salary << "\n";
        file.close();
    }
};

class Admin : public Employee {
public:
    void saveToFile() {
        ofstream file("Admin.txt", ios::app);
        file << id << "-" << name << "-" << password << "-" << salary << "\n";
        file.close();
    }
};

class FileManager {
public:
    void getAllClients() {
        ifstream file("Client.txt");
        if (!file) {
            cout << "No clients found.\n";
            return;
        }
        string line;
        cout << "\n--- Clients List ---\n";
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }

    void getAllEmployees() {
        ifstream file("Employee.txt");
        if (!file) {
            cout << "No employees found.\n";
            return;
        }
        string line;
        cout << "\n--- Employees List ---\n";
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }

    void getAllAdmins() {
        ifstream file("Admin.txt");
        if (!file) {
            cout << "No admins found.\n";
            return;
        }
        string line;
        cout << "\n--- Admins List ---\n";
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }

    static vector<string> split(const string& line) {
        vector<string> tokens;
        stringstream ss(line);
        string token;
        while (getline(ss, token, '-')) {
            tokens.push_back(token);
        }
        return tokens;
    }

    void deposit(int id, double amount) {
        ifstream file("Client.txt");
        ofstream temp("temp.txt");
        string line;
        bool found = false;

        while (getline(file, line)) {
            vector<string> tokens = split(line);
            if (tokens.size() < 4) continue;

            if (stoi(tokens[0]) == id) {
                double newBalance = stod(tokens[3]) + amount;
                line = tokens[0] + "-" + tokens[1] + "-" + tokens[2] + "-" + to_string(newBalance);
                found = true;
                cout << "Deposit successful! New Balance: " << newBalance << endl;
            }
            temp << line << "\n";
        }

        file.close();
        temp.close();
        remove("Client.txt");
        if (rename("temp.txt", "Client.txt") != 0) {
            perror("Error renaming file");
        }
        if (!found) {
            cout << "Client ID not found.\n";
        }
    }

    void withdraw(int id, double amount) {
        ifstream file("Client.txt");
        ofstream temp("temp.txt");
        string line;
        bool found = false;

        while (getline(file, line)) {
            vector<string> tokens = split(line);
            if (tokens.size() < 4) continue;

            if (stoi(tokens[0]) == id) {
                double currentBalance = stod(tokens[3]);
                if (currentBalance - amount >= 1500) {
                    double newBalance = currentBalance - amount;
                    line = tokens[0] + "-" + tokens[1] + "-" + tokens[2] + "-" + to_string(newBalance);
                    found = true;
                    cout << "Withdrawal successful! New Balance: " << newBalance << endl;
                } else {
                    cout << "Withdrawal failed! Balance must remain at least 1500 after withdrawal.\n";
                }
            }
            temp << line << "\n";
        }

        file.close();
        temp.close();
        remove("Client.txt");
        rename("temp.txt", "Client.txt");

        if (!found) {
            cout << "Client ID not found.\n";
        }
    }

    void transfer(int senderId, int receiverId, double amount) {
        ifstream file("Client.txt");
        ofstream temp("temp.txt");
        string line;
        bool senderFound = false, receiverFound = false;
        double senderBalance = 0.0, receiverBalance = 0.0;

        vector<string> senderTokens, receiverTokens;

        while (getline(file, line)) {
            vector<string> tokens = split(line);
            if (tokens.size() < 4) continue;

            if (stoi(tokens[0]) == senderId) {
                senderFound = true;
                senderBalance = stod(tokens[3]);
                senderTokens = tokens;
            } else if (stoi(tokens[0]) == receiverId) {
                receiverFound = true;
                receiverBalance = stod(tokens[3]);
                receiverTokens = tokens;
            } else {
                temp << line << "\n";
            }
        }

        if (senderFound && receiverFound) {
            if (senderBalance - amount >= 1500) {
                senderBalance -= amount;
                receiverBalance += amount;

                temp << senderTokens[0] << "-" << senderTokens[1] << "-" << senderTokens[2] << "-" << to_string(senderBalance) << "\n";
                temp << receiverTokens[0] << "-" << receiverTokens[1] << "-" << receiverTokens[2] << "-" << to_string(receiverBalance) << "\n";

                cout << "Transfer successful! New Sender Balance: " << senderBalance << ", New Receiver Balance: " << receiverBalance << endl;
            } else {
                cout << "Transfer failed! Sender's balance must remain at least 1500 after transfer.\n";
            }
        } else {
            if (!senderFound) cout << "Sender ID not found.\n";
            if (!receiverFound) cout << "Receiver ID not found.\n";
        }

        file.close();
        temp.close();
        remove("Client.txt");
        rename("temp.txt", "Client.txt");
    }
    void editClient(int id, double newBalance) {
        ifstream file("Client.txt");
        ofstream temp("temp.txt");
        string line;
        bool found = false;

        while (getline(file, line)) {
            vector<string> tokens = split(line);
            if (tokens.size() < 4) continue;

            if (stoi(tokens[0]) == id) {
                line = tokens[0] + "-" + tokens[1] + "-" + tokens[2] + "-" + to_string(newBalance);
                found = true;
                cout << "Client balance updated successfully! New Balance: " << newBalance << endl;
            }
            temp << line << "\n";
        }

        file.close();
        temp.close();
        remove("Client.txt");
        if (rename("temp.txt", "Client.txt") != 0) {
            perror("Error renaming file");
        }
        if (!found) {
            cout << "Client ID not found.\n";
        }
    }

    void editEmployee(int id, double newSalary) {
        ifstream file("Employee.txt");
        ofstream temp("temp.txt");
        string line;
        bool found = false;

        while (getline(file, line)) {
            vector<string> tokens = split(line);
            if (tokens.size() < 4) continue;

            if (stoi(tokens[0]) == id) {
                line = tokens[0] + "-" + tokens[1] + "-" + tokens[2] + "-" + to_string(newSalary);
                found = true;
                cout << "Employee salary updated successfully! New Salary: " << newSalary << endl;
            }
            temp << line << "\n";
        }

        file.close();
        temp.close();
        remove("Employee.txt");
        rename("temp.txt", "Employee.txt");

        if (!found) {
            cout << "Employee ID not found.\n";
        }
    }
        void edit(int id, double amount) {
        editClient(id, amount);
        editEmployee(id, amount);
    }
};

class DataSourceInterface {
public:
    virtual void addClients(Client) = 0;
    virtual void addEmployee(Employee) = 0;
    virtual void addAdmin(Admin) = 0;
    virtual void getAllClients() = 0;
    virtual void getAllEmployees() = 0;
    virtual void getAllAdmins() = 0;
    virtual void removeAllClients() = 0;
    virtual void removeAllEmployees() = 0;
    virtual void removeAllAdmins() = 0;
};


class Parser {
public:
    static vector<string> split(const string& line) {
        vector<string> parts;
        stringstream ss(line);
        string part;
        while (getline(ss, part, '-')) 
            parts.push_back(part);
        return parts;
    }

    template <typename T>
    static T parse(const string& line);

    template <typename T>
    static vector<T> loadFromFile(const string& filename) {
        vector<T> objects;
        ifstream file(filename);
        string line;
        while (getline(file, line)) {
            try {
                objects.push_back(parse<T>(line));
            } catch (const exception& e) {
                cout << "Error parsing line: " << e.what() << endl;
            }
        }
        return objects;
    }
    template <typename T>
    static void saveToFile(const string& filename, const T& obj) {
        ofstream file(filename, ios::app);
        file << obj.toString() << "\n";
        file.close();
    }
};


int main() {
    cout << "-------------------------------------------------------Developed by Code Hunters-------------------------------------\n\n";
    cout << "                                      Welcome to Bank Management System                          \n";

    int choice;
    FileManager fileManager;

    while (true) {
        cout << "\n--- Bank Management System ---\n";
        cout << "1. Add Client\n";
        cout << "2. View All Clients\n";
        cout << "3. Edit Client\n";
        cout << "4. Add Employee\n";
        cout << "5. View All Employees\n";
        cout << "6. Edit Employee\n";
        cout << "7. Add Admin\n";
        cout << "8. View All Admins\n";
        cout << "9. Deposit Money\n";
        cout << "10. Withdraw Money\n";
        cout << "11. Transfer Money\n";
        cout << "12. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: {
                Client c;
                c.saveToFile();
                break;
            }
            case 2:{
                fileManager.getAllClients();
                break;
            }
            case 3: {
                int id;
                double newBalance;
                cout << "Enter Client ID to edit: ";
                cin >> id;
                cout << "Enter new balance: ";
                cin >> newBalance;
                fileManager.editClient(id, newBalance);
                break;
            }

            case 4: {
                Employee e;
                e.setId();
                e.setName();
                e.setPassword();
                e.setSalary();
                e.saveToFile();
                break;
            }
            case 5:{
                fileManager.getAllEmployees();
                break;
        }
            case 6:{
				int id;
				double amount;
				cout << "Enter Employee ID to edit: ";
				cin >> id;
				cout << "Enter amount to edit: ";
				cin >> amount;
				fileManager.edit(id, amount);
				break;
			}
                case 7: {
                Admin a;
                a.setId();
                a.setName();
                a.setPassword();
                a.setSalary();
                a.saveToFile();
                break;
            }
            case 8:
                fileManager.getAllAdmins();
                break;
            case 9: {
                int id;
                double amount;
                cout << "Enter Client ID to deposit: ";
                cin >> id;
                cout << "Enter amount to deposit: ";
                cin >> amount;
                fileManager.deposit(id, amount);
                break;
            }
            case 10:
				int id;
				double amount;
				cout << "Enter Client ID to withdraw: ";
				cin >> id;
				cout << "Enter amount to withdraw: ";
				cin >> amount;
				fileManager.withdraw(id, amount);
				break;
             case 11: {
                int senderId, receiverId;
                double amount;
                cout << "Enter Sender ID: ";
                cin >> senderId;
                cout << "Enter Receiver ID: ";
                cin >> receiverId;
                cout << "Enter amount to transfer: ";
                cin >> amount;
                fileManager.transfer(senderId, receiverId, amount);
                break;
            }
            case 12:
                cout << "Exiting...\n";
                return 0;
            default:
                cout << "Invalid choice! Try again.\n";
        }
    }


    return 0;
}
