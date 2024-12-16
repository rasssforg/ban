#include <iostream>
#include <memory>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
using namespace std;


class Customer {
private:
    string name;
    string id;
public:
    Customer(const string& name, const string& id) : name(name), id(id) {}
    string GetName() const { return name; }
    string GetId() const { return id; }
};

class Account {
protected:
    double balance;
public:
    Account(double initialBalance) : balance(initialBalance) {}

    virtual void Deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "Депозит: " << amount << " выполнен." << endl;
        } else {
            cout << "Сумма депозита должна быть положительной." << endl;
        }
    }

    virtual void Withdraw(double amount) = 0;
    double GetBalance() const { return balance; }
    virtual void DisplayInfo() const = 0;
    virtual ~Account() = default; 
};

class SavingsAccount : public Account {
private:
    double interestRate;
public:
    SavingsAccount(double initialBalance, double interestRate = 0.01) 
        : Account(initialBalance), interestRate(interestRate) {}

    void Withdraw(double amount) override {
        if (amount <= balance) {
            balance -= amount;
            cout << "Снятие: " << amount << " выполнено." << endl;
        } else {
            cout << "Недостаточно средств для снятия!" << endl;
        }
    }

    void ApplyInterest() {
        balance += balance * interestRate;
        cout << "Проценты применены: " << (balance * interestRate) << endl;
    }

    void DisplayInfo() const override {
        cout << "Сберегательный счет, баланс: " << balance << ", Процентная ставка: " << interestRate << endl;
    }
};


class CheckingAccount : public Account {
private:
    double overdraftLimit;
public:
    CheckingAccount(double initialBalance, double overdraftLimit = 0) 
        : Account(initialBalance), overdraftLimit(overdraftLimit) {}

    void Withdraw(double amount) override {
        if (balance + overdraftLimit >= amount) {
            balance -= amount; 
            if (balance < 0) {
                cout << "Вы вышли в минус на " << -balance << "!" << endl;
            } else {
                cout << "Снятие: " << amount << " выполнено." << endl;
            }
        } else {
            cout << "Недостаточно средств для снятия!" << endl;
        }
    }

    void DisplayInfo() const override {
        cout << "Расчетный счет, баланс: " << balance 
             << " Лимит овердрафта: " << overdraftLimit << endl;
    }

    void SetOverdraftLimit(double newLimit) {
        overdraftLimit = newLimit;
        cout << "Лимит овердрафта обновлен до: " << overdraftLimit << endl;
    }
};

class BusinessAccount : public Account {
public:
    BusinessAccount(double initialBalance) : Account(initialBalance) {}

    void Withdraw(double amount) override {
        if (amount <= GetBalance()) {
            balance -= amount;
            cout << "Снятие: " << amount << " выполнено." << endl;
        } else {
            cout << "Недостаточно средств для снятия!" << endl;
        }
    }

    void DisplayInfo() const override {
        cout << "Бизнес-счет, баланс: " << balance << endl;
    }
};

class Bank {
private:
    vector<shared_ptr<Customer>> customers;
    vector<shared_ptr<Account>> accounts;

public:
    void AddCustomer(const string& name, const string& id) {
        customers.push_back(make_shared<Customer>(name, id));
    }

    void AddAccount(shared_ptr<Account> account) {
        accounts.push_back(account);
    }

    vector<shared_ptr<Account>> GetAccounts() const {
        return accounts;
    }


    void ShowAccounts() const {
        for (const auto& account : accounts) {
            account->DisplayInfo();
        }
    }

    void ShowCustomers() const {
        for (const auto& customer : customers) {
            cout << "Клиент: " << customer->GetName() << ", ID: " << customer->GetId() << endl;
        }
    }

    void Transfer(shared_ptr<Account> from, shared_ptr<Account> to, double amount) {
        if (from && to) {
            if (from->GetBalance() >= amount) {
                from->Withdraw(amount);
                to->Deposit(amount);
                cout << "Перевод: " << amount << " выполнен." << endl;
            } else {
                cout << "Недостаточно средств для перевода!" << endl;
            }
        } else {
            cout << "Некорректные счета для перевода!" << endl;
        }
    }
};

bool isValidId(const string& id) {
    return id.length() == 9 && all_of(id.begin(), id.end(), ::isdigit);
}

void LoadCustomers(Bank& bank, const string& fileName) {
    ifstream file(fileName);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string name, id;
            ss >> name >> id;
            bank.AddCustomer(name, id);
        }
        file.close();
    } else {
        cout << "Ошибка загрузки файла клиентов." << endl;
    }
}


void LoadAccounts(Bank& bank, const string& fileName) {
    ifstream file(fileName);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            int accountType;
            double balance, interestRate, overdraftLimit;
            ss >> accountType >> balance;

            switch (accountType) {
                case 1:  
                    ss >> interestRate;
                    bank.AddAccount(make_shared<SavingsAccount>(balance, interestRate));
                    break;
                case 2:  
                    ss >> overdraftLimit;
                    bank.AddAccount(make_shared<CheckingAccount>(balance, overdraftLimit));
                    break;
                case 3:  
                    bank.AddAccount(make_shared<BusinessAccount>(balance));
                    break;
                default:
                    cout << "Неизвестный тип аккаунта: " << accountType << endl;
                    break;
            }
        }
        file.close();
    } else {
        cout << "Ошибка загрузки файла аккаунтов." << endl;
    }
}

void SaveAccounts(const Bank& bank, const string& fileName) {
    ofstream file(fileName);
    if (file.is_open()) {
        for (const auto& account : bank.GetAccounts()) {
            file << account->GetType() << " " << account->GetBalance();
            if (auto savings = dynamic_pointer_cast<SavingsAccount>(account)) {
                file << " " << savings->GetInterestRate();
            } else if (auto checking = dynamic_pointer_cast<CheckingAccount>(account)) {
                file << " " << checking->GetOverdraftLimit();
            }
            file << endl;
        }
        file.close();
    } else {
        cout << "Ошибка сохранения файла аккаунтов." << endl;
    }
}

int main() {
    Bank bank;
    LoadCustomers(bank, "customers.txt");
    LoadAccounts(bank, "accounts.txt");

    int numAccounts;
    cout << "Введите количество аккаунтов: ";
    cin >> numAccounts;

    for (int i = 0; i < numAccounts; ++i) {
        double initialBalance;
        int accountType;

        cout << "Введите начальный баланс для аккаунта #" << (i + 1) << ": ";
        cin >> initialBalance;

        while (true) {
            cout << "Введите тип аккаунта (1 - Сберегательный, 2 - Расчетный, 3 - Бизнес): ";
            cin >> accountType;

            if (accountType >= 1 && accountType <= 3) {
                break; 
            } else {
                cout << "Такого варианта нет, попробуйте заново." << endl;
            }
        }

        if (accountType == 1) {
            double interestRate;
            cout << "Введите процентную ставку: ";
            cin >> interestRate;
            bank.AddAccount(make_shared<SavingsAccount>(initialBalance, interestRate));
        } else if (accountType == 2) {
            double overdraftLimit;
            cout << "Введите лимит овердрафта (максимальная сумма, на которую можно уходить в минус): ";
            cin >> overdraftLimit;
            bank.AddAccount(make_shared<CheckingAccount>(initialBalance, overdraftLimit));
        } else if (accountType == 3) {
            bank.AddAccount(make_shared<BusinessAccount>(initialBalance));
        }

        cout << "Аккаунт #" << (i + 1) << " создан с балансом: " << initialBalance 
             << " и типом: " << accountType << endl;
    }


    SaveCustomers(bank, "customers.txt");
    SaveAccounts(bank, "accounts.txt");


    bank.ShowCustomers();
    bank.ShowAccounts();

    return 0;
} }
