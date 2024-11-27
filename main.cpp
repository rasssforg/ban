#include <iostream>
#include <memory>
#include <vector>
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
        cout << "Расчетный счет, баланс: " << balance << ", Лимит овердрафта: " << overdraftLimit << endl;
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

    shared_ptr<Account> GetAccount(int index) const {
        if (index < 0 || index >= accounts.size()) {
            return nullptr; 
        }
                return accounts[index];
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

        int main() {
            Bank bank;
            bank.AddCustomer("Татьяна Сергеевна", "774831");
            bank.AddAccount(make_shared<SavingsAccount>(10680, 0.05));
            bank.AddAccount(make_shared<CheckingAccount>(5000, 1500));
            bank.AddAccount(make_shared<BusinessAccount>(20000));

            bank.ShowCustomers();
            bank.ShowAccounts();

            auto savings = dynamic_pointer_cast<SavingsAccount>(bank.GetAccount(0));
            if (savings) {
                savings->ApplyInterest();
            }

            bank.ShowAccounts();

            auto checking = bank.GetAccount(1);
            bank.Transfer(savings, checking, 500);

            bank.ShowAccounts();

            return 0;
        }


