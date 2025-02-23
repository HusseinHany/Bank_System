#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class person {
protected:
    string name;
    int id = 0;
    string password;

public:
    void setName() {
        string inputName;
        while (true) {
            cout << "Enter name: ";
            getline(cin, inputName);
            bool valid = true;
            for (char c : inputName) {
                if (!isalpha(c) && c != ' ') {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                name = inputName;
                break;
            } else {
                cout << "Name must contain only alphabetic characters and spaces. Please try again.\n";
            }
        }
    }

    string getName() {
        return name;
    }

    void setId() {
        int inputId;
        while (true) {
            cout << "Enter ID: ";
            cin >> inputId;
            if (cin.fail() || inputId < 0) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "ID must be a positive integer. Please try again.\n";
            } else {
                id = inputId;
                cin.ignore(10000, '\n');
                break;
            }
        }
    }

    int getId() {
        return id;
    }

    void setPassword() {
        string inputPassword;
        while (true) {
            cout << "Enter password (8-20 characters): ";
            getline(cin, inputPassword);
            if (inputPassword.length() >= 8 && inputPassword.length() <= 20) {
                password = inputPassword;
                break;
            } else {
                cout << "Password must be between 8 and 20 characters. Please try again.\n";
            }
        }
    }

    string getPassword() {
        return password;
    }
};

class Client : public person {
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


    void saveToFile() {
    ofstream file("Client.txt", ios::app);  
    file << id << " " << name << " " << password << " " << balance << "\n";
    file.close();
}

void loadFromFile() {
    ifstream file("Client.txt");
    if (file.is_open()) {
        file >> id >> name >> password >> balance;
        file.close();
    }
}

    void setBalance() {
        double inputBalance;
        while (true) {
            cout << "Enter balance (minimum " << MIN_BALANCE << "): ";
            cin >> inputBalance;
            if (cin.fail() || inputBalance < MIN_BALANCE) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Balance must be at least " << MIN_BALANCE << ". Please try again.\n";
            } else {
                balance = inputBalance;
                cin.ignore(10000, '\n');
                break;
            }
        }
    }

    double getBalance() {
        return balance;
    }

    void deposit( double amount) {
        
        while (true) {
            cout << "Enter deposit amount (greater than 0): ";
            cin >> amount;
            if (cin.fail() || amount <= 0) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Deposit amount must be greater than zero. Please try again.\n";
            } else {
                balance += amount;
                cin.ignore(10000, '\n');
                break;
            }
        }
    }

    void withdraw() {
        double amount;
        while (true) {
            cout << "Enter withdrawal amount (greater than 0): ";
            cin >> amount;
            if (cin.fail() || amount <= 0 || (balance - amount) < MIN_BALANCE) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Withdrawal amount must be greater than zero and must not drop balance below " << MIN_BALANCE << ". Please try again.\n";
            } else {
                balance -= amount;
                cin.ignore(10000, '\n');
                break;
            }
        }
    }

    void transferTo(Client& recipient) {
        double amount;
        while (true) {
            cout << "Enter transfer amount (greater than 0): ";
            cin >> amount;
            if (cin.fail() || amount <= 0 || (balance - amount) < MIN_BALANCE) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Transfer amount must be greater than zero and must not drop balance below " << MIN_BALANCE << ". Please try again.\n";
            } else {
                balance -= amount;
                recipient.deposit(amount);
                cin.ignore(10000, '\n');
                break;
            }
        }
    }

    void checkBalance() {
        cout << "Current Balance: " << getBalance() << endl;
    }

    void display() {
        cout << "ID: " << getId() << endl;
        cout << "Name: " << getName() << endl;
        cout << "Balance: " << getBalance() << endl;
    }
};

class Employee : public person {
protected:
    double salary;
    const double MIN_SALARY = 5000;

public:
    Employee() {
        setId();
        setName();
        setPassword();
        setSalary();
    }

    void saveToFile() {
    ofstream file("Employee.txt", ios::app);
    file << id << " " << name << " " << password << " " << salary << "\n";
    file.close();
}

void loadFromFile() {
    ifstream file("Employee.txt");
    if (file.is_open()) {
        file >> id >> name >> password >> salary;
        file.close();
    }
}


    void setSalary() {
        double inputSalary;
        while (true) {
            cout << "Enter salary (minimum " << MIN_SALARY << "): ";
            cin >> inputSalary;
            if (cin.fail() || inputSalary < MIN_SALARY) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Salary must be at least " << MIN_SALARY << ". Please try again.\n";
            } else {
                salary = inputSalary;
                cin.ignore(10000, '\n');
                break;
            }
        }
    }

    double getSalary() {
        return salary;
    }

    void display() {
        cout << "Employee ID: " << getId() << endl;
        cout << "Name: " << getName() << endl;
        cout << "Salary: " << getSalary() << endl;
    }
};

class Admin : public Employee {
public:
    Admin() : Employee() {}

    void saveToFile() {
    ofstream file("Admin.txt", ios::app);
    file << id << " " << name << " " << password << " " << salary << "\n";
    file.close();
}

void loadFromFile() {
    ifstream file("Admin.txt");
    if (file.is_open()) {
        file >> id >> name >> password >> salary;
        file.close();
    }
}

    void display() {
        cout << "Admin ID: " << getId() << "\nName: " << getName() << "\nSalary: " << getSalary() << "\n";
    }
};

int main() {
    Client c1;
    c1.loadFromFile(); // Load data if available
    c1.display();
    c1.deposit(1000);
    c1.checkBalance();
    c1.withdraw();
    c1.checkBalance();
    c1.saveToFile(); // Save updated data

    Employee e1;
    e1.loadFromFile();
    e1.display();
    e1.saveToFile();

    Admin a1;
    a1.loadFromFile();
    a1.display();
    a1.saveToFile();

    return 0;
}
   