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

    string getName() const {
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

    int getId() const {
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

    void saveToFile() {
        ofstream file("Client.txt", ios::app);
        file << id << "-" << name << "-" << password << "-" << balance << "\n";
        file.close();
    }

    void loadFromFile() {
        ifstream file("Client.txt");
        if (file.is_open()) {
            while (file >> id >> name >> password >> balance) {
                // Load all clients
            }
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

    double getBalance() const {
        return balance;
    }

    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
        } else {
            cout << "Deposit amount must be greater than zero.\n";
        }
    }

    void withdraw(double amount) {
        if (amount > 0 && (balance - amount) >= MIN_BALANCE) {
            balance -= amount;
        } else {
            cout << "Invalid withdrawal amount.\n";
        }
    }

    void transferTo(Client& recipient, double amount) {
        if (amount > 0 && (balance - amount) >= MIN_BALANCE) {
            balance -= amount;
            recipient.deposit(amount);
        } else {
            cout << "Invalid transfer amount.\n";
        }
    }

    void checkBalance() const {
        cout << "Current Balance: " << getBalance() << endl;
    }

    void display() const {
        cout << "ID: " << getId() << endl;
        cout << "Name: " << getName() << endl;
        cout << "Balance: " << getBalance() << endl;
    }
};

class Employee : public Person {
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

    Employee(int id, const string& name, const string& password, double salary) {
        this->id = id;
        this->name = name;
        this->password = password;
        this->salary = salary;
    }

    void saveToFile() {
        ofstream file("Employee.txt", ios::app);
        file << id << "-" << name << "-" << password << "-" << salary << "\n";
        file.close();
    }

    void loadFromFile() {
        ifstream file("Employee.txt");
        if (file.is_open()) {
            while (file >> id >> name >> password >> salary) {
                // Load all employees
            }
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

    double getSalary() const {
        return salary;
    }
    
    void addClient(Client& client) {
        client.saveToFile();
    }

    void listClient() {
        ifstream file("Client.txt");
        if (!file) {
            cout << "No clients found.\n";
        } else {
            string line;
            cout << "\n--- Clients List ---\n";
            while (getline(file, line)) {
                cout << line << endl;
            }
            file.close();
        }
    }

	Client* searchClient(int id) {
		ifstream file("Client.txt");
		if (!file) {
			cout << "No clients found.\n";
		}
		else {
			string line;
			while (getline(file, line)) {
				stringstream ss(line);
				string token;
				getline(ss, token, '-');
				int clientId = stoi(token);
				if (clientId == id) {
					string name, password;
					double balance;
					getline(ss, name, '-');
					getline(ss, password, '-');
					getline(ss, token, '-');
					balance = stod(token);
					return new Client(clientId, name, password, balance);
				}
			}
			file.close();
		}
		return nullptr;
	}

    void editClient(int id, string name, string password, double balance){
    
		ifstream file("Client.txt");
		ofstream temp("temp.txt");
		if (!file) {
			cout << "No clients found.\n";
		}
		else {
			string line;
			while (getline(file, line)) {
				stringstream ss(line);
				string token;
				getline(ss, token, '-');
				int clientId = stoi(token);
				if (clientId == id) {
					temp << id << "-" << name << "-" << password << "-" << balance << "\n";
				}
				else {
					temp << line << "\n";
				}
			}
			file.close();
			temp.close();
			remove("Client.txt");
			if (rename("temp.txt", "Client.txt") != 0) {
				perror("Error renaming file");
			}

		}

    }

	void deleteClient(int id) {
		ifstream file("Client.txt");
		ofstream temp("temp.txt");
		if (!file) {
			cout << "No clients found.\n";
		}
		else {
			string line;
			while (getline(file, line)) {
				stringstream ss(line);
				string token;
				getline(ss, token, '-');
				int clientId = stoi(token);
				if (clientId != id) {
					temp << line << "\n";
				}
			}
			file.close();
			temp.close();
			remove("Client.txt");
			if (rename("temp.txt", "Client.txt") != 0) {
               perror("Error renaming file");
             }

		}
	}

    void display() const {
        cout << "Employee ID: " << getId() << endl;
        cout << "Name: " << getName() << endl;
        cout << "Salary: " << getSalary() << endl;
    }
};

class Admin : public Employee {
public:
    Admin() : Employee() {}

    Admin(int id, const string& name, const string& password, double salary) : Employee(id, name, password, salary) {}

    void saveToFile() {
        ofstream file("Admin.txt", ios::app);
        file << id << " " << name << " " << password << " " << salary << "\n";
        file.close();
    }

    void loadFromFile() {
        ifstream file("Admin.txt");
        if (file.is_open()) {
            while (file >> id >> name >> password >> salary) {
				
            }
            file.close();
        }
    }

	void addEmployee(Employee& employee) {
		employee.saveToFile();
	}

	void listEmployee() {
		ifstream file("Employee.txt");
		if (!file) {
			cout << "No employees found.\n";
		}
		else {
			string line;
			cout << "\n--- Employees List ---\n";
			while (getline(file, line)) {
				cout << line << endl;
			}
			file.close();
		}
	}

	Employee* searchEmployee(int id) {
		ifstream file("Employee.txt");

		if (!file) {
			cout << "No employees found.\n";
		}
		else {
			string line;
			while (getline(file, line)) {
				stringstream ss(line);
				string token;
				getline(ss, token, '-');
				int employeeId = stoi(token);
				if (employeeId == id) {
					string name, password;
					double salary;
					getline(ss, name, '-');
					getline(ss, password, '-');
					getline(ss, token, '-');
					salary = stod(token);
					return new Employee(employeeId, name, password, salary);
				}
			}
			file.close();
		}
	}

	void editEmployee(int id, string name, string password, double salary) {
		ifstream file("Employee.txt");
		ofstream temp("temp.txt");
		if (!file) {
			cout << "No employees found.\n";
		}
		else {
			string line;
			while (getline(file, line)) {
				stringstream ss(line);
				string token;
				getline(ss, token, '-');
				int employeeId = stoi(token);
				if (employeeId == id) {
					temp << id << "-" << name << "-" << password << "-" << salary << "\n";
				}
				else {
					temp << line << "\n";
				}
			}
			file.close();
			temp.close();
			remove("Employee.txt");
			rename("temp.txt", "Employee.txt");
		}
	}

    void display() const {
        cout << "Admin ID: " << getId() << "\nName: " << getName() << "\nSalary: " << getSalary() << "\n";
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

	static vector <string> split(string line) {
		vector <string> tokens;
		stringstream ss(line);
		string token;
		while (getline(ss, token, '-')) {
			tokens.push_back(token);
		}
		return tokens;
	}

	static Client ParseToClient(string line){
		vector <string> tokens = split(line);
		int id = stoi(tokens[0]);
		string name = tokens[1];
		string password = tokens[2];
		double balance = stod(tokens[3]);
		return Client(id, name, password, balance);
	}

	static Employee ParseToEmployee(string line) {
		vector <string> tokens = split(line);
		int id = stoi(tokens[0]);
		string name = tokens[1];
		string password = tokens[2];
		double salary = stod(tokens[3]);
		return Employee(id, name, password, salary);
	}

	static Admin ParseToAdmin(string line) {
		vector <string> tokens = split(line);
		int id = stoi(tokens[0]);
		string name = tokens[1];
		string password = tokens[2];
		double salary = stod(tokens[3]);
		return Admin(id, name, password, salary);
	}

};

class FileManager : public DataSourceInterface {
public:
	void addClients(Client c) {
		c.saveToFile();
	}

	void addEmployee(Employee e) {
		e.saveToFile();
	}

	void addAdmin(Admin a) {
		a.saveToFile();
	}

	void getAllClients() {
		ifstream file("Client.txt");
		if (!file) {
			cout << "No clients found.\n";
		}
		else {
			string line;
			cout << "\n--- Clients List ---\n";
			while (getline(file, line)) {
				cout << line << endl;
			}
			file.close();
		}
	}

	void getAllEmployees() {
		ifstream file("Employee.txt");
		if (!file) {
			cout << "No employees found.\n";
		}
		else {
			string line;
			cout << "\n--- Employees List ---\n";
			while (getline(file, line)) {
				cout << line << endl;
			}
			file.close();
		}
	}

	void getAllAdmins() {
		ifstream file("Admin.txt");
		if (!file) {
			cout << "No admins found.\n";
		}
		else {
			string line;
			cout << "\n--- Admins List ---\n";
			while (getline(file, line)) {
				cout << line << endl;
			}
			file.close();
		}
	}

    void editClient(int id, const string& name, const string& password, double balance) {
        ifstream file("Client.txt");
        ofstream temp("temp.txt");
        if (!file) {
            cout << "No clients found.\n";
        } else {
            string line;
            while (getline(file, line)) {
                stringstream ss(line);
                string token;
                getline(ss, token, '-');
                int clientId = stoi(token);
                if (clientId == id) {
                    temp << id << "-" << name << "-" << password << "-" << balance << "\n";
                } else {
                    temp << line << "\n";
                }
            }
            file.close();
            temp.close();
            remove("Client.txt");
            if (rename("temp.txt", "Client.txt") != 0) {
                perror("Error renaming file");
            }
        }
    }

	void editEmployee(int id, const string& name, const string& password, double salary) {
		ifstream file("Employee.txt");
		ofstream temp("temp.txt");
		if (!file) {
			cout << "No employees found.\n";
		}
		else {
			string line;
			while (getline(file, line)) {
				stringstream ss(line);
				string token;
				getline(ss, token, '-');
				int employeeId = stoi(token);
				if (employeeId == id) {
					temp << id << "-" << name << "-" << password << "-" << salary << "\n";
				}
				else {
					temp << line << "\n";
				}
			}
			file.close();
			temp.close();
			remove("Employee.txt");
			if (rename("temp.txt", "Employee.txt") != 0) {
				perror("Error renaming file");
			}
		}
	}

	void deposit(int id, double amount) {
		ifstream file("Client.txt");
		ofstream temp("temp.txt");
		string line;
		bool found = false;
		while (getline(file, line)) {
			vector<string> tokens = Parser::split(line);
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
			vector<string> tokens = Parser::split(line);
			if (tokens.size() < 4) continue;
			if (stoi(tokens[0]) == id) {
				double newBalance = stod(tokens[3]) - amount;
				if (newBalance >= 1500) {
					line = tokens[0] + "-" + tokens[1] + "-" + tokens[2] + "-" + to_string(newBalance);
					found = true;
					cout << "Withdrawal successful! New Balance: " << newBalance << endl;
				}
				else {
					cout << "Withdrawal failed! Minimum balance of 1500 must be maintained.\n";
				}
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

    void transfer(int senderId, int receiverId, double amount) {
        ifstream file("Client.txt");
        ofstream temp("temp.txt");
        string line;
        bool senderFound = false, receiverFound = false;
        double senderBalance = 0.0, receiverBalance = 0.0;

        vector<string> senderTokens, receiverTokens;

        while (getline(file, line)) {
            vector<string> tokens = Parser::split(line);
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

	void deletClient(int id) {
		ifstream file("Client.txt");
		ofstream temp("temp.txt");
		string line;
		bool found = false;
		while (getline(file, line)) {
			vector<string> tokens = Parser::split(line);
			if (tokens.size() < 4) continue;
			if (stoi(tokens[0]) == id) {
				found = true;
			}
			else {
				temp << line << "\n";
			}
		}
		file.close();
		temp.close();
		remove("Client.txt");
		rename("temp.txt", "Client.txt");
		if (!found) {
			cout << "Client ID not found.\n";
		}
	}

	void deleteEmployee(int id) {
		ifstream file("Employee.txt");
		ofstream temp("temp.txt");
		string line;
		bool found = false;
		while (getline(file, line)) {
			vector<string> tokens = Parser::split(line);
			if (tokens.size() < 4) continue;
			if (stoi(tokens[0]) == id) {
				found = true;
			}
			else {
				temp << line << "\n";
			}
		}
		file.close();
		temp.close();
		remove("Employee.txt");
		rename("temp.txt", "Employee.txt");
		if (!found) {
			cout << "Employee ID not found.\n";
		}
	}

	void deleteAdmin(int id) {
		ifstream file("Admin.txt");
		ofstream temp("temp.txt");
		string line;
		bool found = false;
		while (getline(file, line)) {
			vector<string> tokens = Parser::split(line);
			if (tokens.size() < 4) continue;
			if (stoi(tokens[0]) == id) {
				found = true;
			}
			else {
				temp << line << "\n";
			}
		}
		file.close();
		temp.close();
		remove("Admin.txt");
		rename("temp.txt", "Admin.txt");
		if (!found) {
			cout << "Admin ID not found.\n";
		}
	}

	void removeAllClients() {
		ofstream file("Client.txt");
		file.close();
	}

	void removeAllEmployees() {
		ofstream file("Employee.txt");
		file.close();
	}

	void removeAllAdmins() {
		ofstream file("Admin.txt");
		file.close();
	}


};

class FileHelper {
public:
	static void saveLast(string fileName, int id) {
		ofstream file(fileName);
		file << id;
		file.close();
	}

	static int getLast(string fileName) {
		ifstream file(fileName);
		int id;
		file >> id;
		file.close();
		return id;
	}

	static void saveLast(Client c) {
		ofstream file("ClientLast.txt");
		file << c.getId();
		file.close();
	}

	static void saveEmployee(string fileName, string lastIdFile, Employee e) {
		ofstream file(fileName, ios::app);
		file << e.getId() << "-" << e.getName() << "-" << e.getPassword() << "-" << e.getSalary() << "\n";
		file.close();
		saveLast(lastIdFile, e.getId());
	}

    static void getClients(){
    
		ifstream file("Client.txt");
		if (!file) {
			cout << "No clients found.\n";
		}
		else {
			string line;
			cout << "\n--- Clients List ---\n";
			while (getline(file, line)) {
				cout << line << endl;
			}
			file.close();
		}
    }

	static void getEmployees() {
		ifstream file("Employee.txt");
		if (!file) {
			cout << "No employees found.\n";
		}
		else {
			string line;
			cout << "\n--- Employees List ---\n";
			while (getline(file, line)) {
				cout << line << endl;
			}
			file.close();
		}
    }

    static void getAdmins() {
    
		ifstream file("Admin.txt");
		if (!file) {
			cout << "No admins found.\n";
        }
		else {
			string line;
			cout << "\n--- Admins List ---\n";
			while (getline(file, line)) {
				cout << line << endl;
            }
			file.close();
        }
    
    }

	static void clearFile(string fileName, string lastIdFile) {
		ofstream file(fileName);
		file.close();
		ofstream lastId(lastIdFile);
		lastId << 0;
		lastId.close();
    }

	
};


void displayMenu() {

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

    cout << "12. Remove Client\n";

    cout << "13. Remove Employee\n";

    cout << "14. Remove Admin\n";

    cout << "15. Exit\n";

    cout << "Enter your choice: ";

}

void addClient(FileManager& fileManager) {

    Client c; 

    c.saveToFile(); 

}

void viewAllClients(FileManager& fileManager) {

    fileManager.getAllClients();

}

void editClient(FileManager& fileManager) {

    int id;

    string name, password;

    double balance;

    cout << "Enter Client ID to edit: ";

    cin >> id;

    cin.ignore(); 

    cout << "Enter new name: ";

    getline(cin, name);

    cout << "Enter new password: ";

    getline(cin, password);

    cout << "Enter new balance: ";

    cin >> balance;

    fileManager.editClient(id, name, password, balance);

}

void addEmployee(FileManager& fileManager) {

    Employee e; 

    e.saveToFile(); 

}

void viewAllEmployees(FileManager& fileManager) {

    fileManager.getAllEmployees();

}

void editEmployee(FileManager& fileManager) {

    int id;

    string name, password;

    double salary;

    cout << "Enter Employee ID to edit: ";

    cin >> id;

    cin.ignore(); 

    cout << "Enter new name: ";

    getline(cin, name);

    cout << "Enter new password: ";

    getline(cin, password);

    cout << "Enter new salary: ";

    cin >> salary;

    fileManager.editEmployee(id, name, password, salary);

}

void addAdmin(FileManager& fileManager) {

    Admin a; 

    a.saveToFile(); 

}

void viewAllAdmins(FileManager& fileManager) {

    fileManager.getAllAdmins();

}

void depositMoney(FileManager& fileManager) {

    int id;

    double amount;

    cout << "Enter Client ID to deposit: ";

    cin >> id;

    cout << "Enter amount to deposit: ";

    cin >> amount;

    fileManager.deposit(id, amount);

}

void withdrawMoney(FileManager& fileManager) {

    int id;

    double amount;

    cout << "Enter Client ID to withdraw: ";

    cin >> id;

    cout << "Enter amount to withdraw: ";

    cin >> amount;

    fileManager.withdraw(id, amount);

}

void transferMoney(FileManager& fileManager) {

    int senderId, receiverId;

    double amount;

    cout << "Enter Sender ID: ";

    cin >> senderId;

    cout << "Enter Receiver ID: ";

    cin >> receiverId;

    cout << "Enter amount to transfer: ";

    cin >> amount;

    fileManager.transfer(senderId, receiverId, amount);

}

void removeClient(FileManager& fileManager) {

    int id;

    cout << "Enter Client ID to remove: ";

    cin >> id;

    fileManager.deletClient(id);

}

void removeEmployee(FileManager& fileManager) {

    int id;

    cout << "Enter Employee ID to remove: ";

    cin >> id;

    fileManager.deleteEmployee(id);

}

void removeAdmin(FileManager& fileManager) {

    int id;

    cout << "Enter Admin ID to remove: ";

    cin >> id;

    fileManager.deleteAdmin(id);
}

int main() {
  cout << "-------------------------------------------------------Developed by Code Hunters-------------------------------------\n\n";

    cout << "                                      Welcome to Bank Management System                          \n";


    int choice;

    FileManager fileManager;


    while (true) {

        displayMenu();

        cin >> choice;

        cin.ignore(); 


        switch (choice) {

            case 1: addClient(fileManager); break;

            case 2: viewAllClients(fileManager); break;

            case 3: editClient(fileManager); break;

            case 4: addEmployee(fileManager); break;

            case 5: viewAllEmployees(fileManager); break;

            case 6: editEmployee(fileManager); break;

            case 7: addAdmin(fileManager); break;

            case 8: viewAllAdmins(fileManager); break;

            case 9: depositMoney(fileManager); break;

            case 10: withdrawMoney(fileManager); break;

            case 11: transferMoney(fileManager); break;

            case 12: removeClient(fileManager); break;

            case 13: removeEmployee(fileManager); break;

            case 14: removeAdmin(fileManager); break;

            case 15: 

                cout << "Exiting...\n"; 

                return 0;

            default: 

                cout << "Invalid choice! Try again.\n";

        }

    }


    return 0;

}
