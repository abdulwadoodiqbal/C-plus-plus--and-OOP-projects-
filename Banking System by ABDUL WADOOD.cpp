// TASK 2: Banking System

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <iomanip>
#include <limits>

using namespace std;

string getCurrentTimestamp() {
    time_t now = time(nullptr);
    tm ltm{};

#if defined(_WIN32) || defined(_MSC_VER)
    localtime_s(&ltm, &now);      
#else
    localtime_r(&now, &ltm);      
#endif

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &ltm);
    return string(buffer);
}

// ================= Transaction Class =================
class Transaction {
public:
    int transactionId;
    string type;          // "Deposit", "Withdrawal", "Transfer-Out", "Transfer-In"
    double amount;
    double balanceAfter;
    string timestamp;
    string details;      

    Transaction(int id, string type, double amount, double balanceAfter, string details = "")
        : transactionId(id), type(type), amount(amount),
        balanceAfter(balanceAfter), details(details) {
        timestamp = getCurrentTimestamp();
    }

    void display() const {
        cout << left << setw(6) << transactionId
            << setw(15) << type
            << setw(12) << fixed << setprecision(2) << amount
            << setw(14) << balanceAfter
            << setw(22) << timestamp
            << details << endl;
    }
};

// ================= Account Class =================
class Account {
public:
    int accountNumber;
    int customerId;
    string accountType;   
    double balance;
    vector<Transaction> history;

private:
    int nextTransactionId = 1;

public:
    Account(int accNum, int custId, string type, double initialDeposit)
        : accountNumber(accNum), customerId(custId),
        accountType(type), balance(initialDeposit) {
        if (initialDeposit > 0)
            recordTransaction("Deposit", initialDeposit, "Initial deposit");
    }

    void recordTransaction(string type, double amount, string details = "") {
        history.emplace_back(nextTransactionId++, type, amount, balance, details);
    }

    bool deposit(double amount) {
        if (amount <= 0) {
            cout << "Deposit amount must be positive.\n";
            return false;
        }
        balance += amount;
        recordTransaction("Deposit", amount);
        return true;
    }

    bool withdraw(double amount) {
        if (amount <= 0) {
            cout << "Withdrawal amount must be positive.\n";
            return false;
        }
        if (amount > balance) {
            cout << "Insufficient balance.\n";
            return false;
        }
        balance -= amount;
        recordTransaction("Withdrawal", amount);
        return true;
    }

    void displayInfo() const {
        cout << "\n----- Account Info -----\n";
        cout << "Account Number : " << accountNumber << endl;
        cout << "Account Type   : " << accountType << endl;
        cout << "Balance        : " << fixed << setprecision(2) << balance << endl;
    }

    void displayRecentTransactions(int count = 5) const {
        cout << "\n----- Recent Transactions (Account " << accountNumber << ") -----\n";
        cout << left << setw(6) << "ID" << setw(15) << "Type" << setw(12) << "Amount"
            << setw(14) << "Balance" << setw(22) << "Timestamp" << "Details" << endl;
        cout << string(85, '-') << endl;

        int start = max(0, (int)history.size() - count);
        for (int i = start; i < (int)history.size(); i++)
            history[i].display();

        if (history.empty())
            cout << "No transactions yet.\n";
    }
};

// ================= Customer Class =================
class Customer {
public:
    int customerId;
    string name;
    string email;
    string phone;
    vector<int> accountNumbers; // links to accounts owned by this customer

    Customer(int id, string name, string email, string phone)
        : customerId(id), name(name), email(email), phone(phone) {
    }

    void displayInfo() const {
        cout << "\n----- Customer Info -----\n";
        cout << "Customer ID : " << customerId << endl;
        cout << "Name        : " << name << endl;
        cout << "Email       : " << email << endl;
        cout << "Phone       : " << phone << endl;
        cout << "Accounts    : ";
        if (accountNumbers.empty()) cout << "None";
        for (size_t i = 0; i < accountNumbers.size(); i++)
            cout << accountNumbers[i] << (i + 1 < accountNumbers.size() ? ", " : "");
        cout << endl;
    }
};

// ================= Bank Class (manages everything) =================
class Bank {
private:
    map<int, Customer> customers;
    map<int, Account> accounts;
    int nextCustomerId = 1001;
    int nextAccountNumber = 5001;

public:
    // ---- Customer management ----
    int createCustomer(string name, string email, string phone) {
        int id = nextCustomerId++;
        customers.emplace(id, Customer(id, name, email, phone));
        cout << "Customer created successfully. Customer ID: " << id << endl;
        return id;
    }

    Customer* findCustomer(int customerId) {
        auto it = customers.find(customerId);
        return (it != customers.end()) ? &it->second : nullptr;
    }

    // ---- Account management ----
    int createAccount(int customerId, string accountType, double initialDeposit) {
        Customer* cust = findCustomer(customerId);
        if (!cust) {
            cout << "Customer not found.\n";
            return -1;
        }
        int accNum = nextAccountNumber++;
        accounts.emplace(accNum, Account(accNum, customerId, accountType, initialDeposit));
        cust->accountNumbers.push_back(accNum);
        cout << "Account created successfully. Account Number: " << accNum << endl;
        return accNum;
    }

    Account* findAccount(int accountNumber) {
        auto it = accounts.find(accountNumber);
        return (it != accounts.end()) ? &it->second : nullptr;
    }

    // ---- Transactions ----
    void deposit(int accountNumber, double amount) {
        Account* acc = findAccount(accountNumber);
        if (!acc) { cout << "Account not found.\n"; return; }
        if (acc->deposit(amount))
            cout << "Deposit successful. New balance: " << fixed << setprecision(2) << acc->balance << endl;
    }

    void withdraw(int accountNumber, double amount) {
        Account* acc = findAccount(accountNumber);
        if (!acc) { cout << "Account not found.\n"; return; }
        if (acc->withdraw(amount))
            cout << "Withdrawal successful. New balance: " << fixed << setprecision(2) << acc->balance << endl;
    }

    bool transferFunds(int fromAccNum, int toAccNum, double amount) {
        Account* fromAcc = findAccount(fromAccNum);
        Account* toAcc = findAccount(toAccNum);

        if (!fromAcc || !toAcc) {
            cout << "One or both accounts not found.\n";
            return false;
        }
        if (fromAccNum == toAccNum) {
            cout << "Cannot transfer to the same account.\n";
            return false;
        }
        if (amount <= 0) {
            cout << "Transfer amount must be positive.\n";
            return false;
        }
        if (amount > fromAcc->balance) {
            cout << "Insufficient balance for transfer.\n";
            return false;
        }

        fromAcc->balance -= amount;
        fromAcc->recordTransaction("Transfer-Out", amount, "To Acc#" + to_string(toAccNum));

        toAcc->balance += amount;
        toAcc->recordTransaction("Transfer-In", amount, "From Acc#" + to_string(fromAccNum));

        cout << "Transfer successful!\n";
        return true;
    }

    // ---- Display ----
    void showAccountInfo(int accountNumber) {
        Account* acc = findAccount(accountNumber);
        if (!acc) { cout << "Account not found.\n"; return; }
        acc->displayInfo();
    }

    void showTransactionHistory(int accountNumber, int count = 5) {
        Account* acc = findAccount(accountNumber);
        if (!acc) { cout << "Account not found.\n"; return; }
        acc->displayRecentTransactions(count);
    }

    void showCustomerInfo(int customerId) {
        Customer* cust = findCustomer(customerId);
        if (!cust) { cout << "Customer not found.\n"; return; }
        cust->displayInfo();
    }

    void listAllCustomers() {
        cout << "\n----- All Customers -----\n";
        if (customers.empty()) { cout << "No customers yet.\n"; return; }
        for (auto& pair : customers) {
            cout << "ID: " << pair.second.customerId << " | Name: " << pair.second.name << endl;
        }
    }
};

// ================= Utility input helpers =================
int readInt(const string& prompt) {
    int value;
    cout << prompt;
    while (!(cin >> value)) {
        cout << "Invalid input. Enter a number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return value;
}

double readDouble(const string& prompt) {
    double value;
    cout << prompt;
    while (!(cin >> value)) {
        cout << "Invalid input. Enter a number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return value;
}

string readLine(const string& prompt) {
    string value;
    cout << prompt;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, value);
    return value;
}

// ================= Main Menu =================
void printMenu() {
    cout << "\n========== BANKING SYSTEM ==========\n";
    cout << "1. Create Customer\n";
    cout << "2. Create Account\n";
    cout << "3. Deposit\n";
    cout << "4. Withdraw\n";
    cout << "5. Transfer Funds\n";
    cout << "6. View Account Info\n";
    cout << "7. View Transaction History\n";
    cout << "8. View Customer Info\n";
    cout << "9. List All Customers\n";
    cout << "0. Exit\n";
    cout << "=====================================\n";
}

int main() {
    Bank bank;

    // Seed some demo data 
    int c1 = bank.createCustomer("Ali Khan", "ali@example.com", "0300-1234567");
    int a1 = bank.createAccount(c1, "Savings", 5000);

    int choice;
    do {
        printMenu();
        choice = readInt("Enter choice: ");

        switch (choice) {
        case 1: {
            string name = readLine("Enter name: ");
            string email = readLine("Enter email: ");
            string phone = readLine("Enter phone: ");
            bank.createCustomer(name, email, phone);
            break;
        }
        case 2: {
            int custId = readInt("Enter customer ID: ");
            string type = readLine("Enter account type (Savings/Current): ");
            double deposit = readDouble("Enter initial deposit: ");
            bank.createAccount(custId, type, deposit);
            break;
        }
        case 3: {
            int accNum = readInt("Enter account number: ");
            double amount = readDouble("Enter deposit amount: ");
            bank.deposit(accNum, amount);
            break;
        }
        case 4: {
            int accNum = readInt("Enter account number: ");
            double amount = readDouble("Enter withdrawal amount: ");
            bank.withdraw(accNum, amount);
            break;
        }
        case 5: {
            int fromAcc = readInt("Enter FROM account number: ");
            int toAcc = readInt("Enter TO account number: ");
            double amount = readDouble("Enter transfer amount: ");
            bank.transferFunds(fromAcc, toAcc, amount);
            break;
        }
        case 6: {
            int accNum = readInt("Enter account number: ");
            bank.showAccountInfo(accNum);
            break;
        }
        case 7: {
            int accNum = readInt("Enter account number: ");
            int count = readInt("How many recent transactions to view? ");
            bank.showTransactionHistory(accNum, count);
            break;
        }
        case 8: {
            int custId = readInt("Enter customer ID: ");
            bank.showCustomerInfo(custId);
            break;
        }
        case 9:
            bank.listAllCustomers();
            break;
        case 0:
            cout << "Exiting... Thank you!\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}