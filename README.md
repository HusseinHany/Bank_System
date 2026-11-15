# Bank_System
Banking System in C++

Overview

This project is a simple banking system implemented in C++. It defines three types of users: Clients, Employees, and Admins. The system allows Clients to manage their balance, Employees to store salary details, and Admins to oversee the system. The program also supports file storage to save user data persistently.

Features

Client Features:

Set and retrieve personal details (ID, name, password, balance)

Deposit and withdraw money

Transfer funds to another client

Display account details

Save and load client data from a file

Employee Features:

Set and retrieve personal details (ID, name, password, salary)

Display employee details

Save and load employee data from a file

Admin Features:

Inherits all Employee functionalities

Manage system-wide data

File Handling

The program uses file handling to store and retrieve user data:

Client.txt: Stores client details (ID, name, password, balance)

Employee.txt: Stores employee details (ID, name, password, salary)

Admin.txt: Stores admin details (ID, name, password, salary)

Class Structure

Person (Base Class)

Contains common attributes: name, id, password

Methods for setting and retrieving these attributes

Client (Derived from Person)

Additional attribute: balance

Methods for deposit, withdrawal, and transfer

Employee (Derived from Person)

Additional attribute: salary

Method to set and retrieve salary

Admin (Derived from Employee)

Inherits all functionalities of Employee

How to Use

Compile the Program

g++ -std=c++17 First.cpp -o bank_system

Run the Executable

./bank_system

First run

The program has no sign-up screen, so the first admin is created by hand. In the folder you run it from, create a file named admins.txt with one line:

1,Your Name,yourpassword,OWNER,you@example.com

Log in as Admin with ID 1 and that password, then add employees; employees add clients. Passwords of accounts created inside the program are stored hashed.

Follow the Prompts

Enter details when prompted

Perform deposit, withdrawal, or transfers

View account details

Possible Enhancements

Implement a proper authentication system for login

Add an interactive menu for user operations

Improve file handling by using structured formats (e.g., JSON or database integration)

Implement error handling for file operations

Author

This project was developed as part of a learning exercise in C++.

License

This project is open-source and can be modified for educational purposes.

