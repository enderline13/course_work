#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <map>

class Employee;
class Order;
class Pizza;

int inputInt(std::string prompt, int m = 1, int M = 1000);

class Order {
public:
	double getOrderPrice() const;
	void addPizza(std::string name, double price, int amount = 1);
private:
	std::vector<Pizza> pizzas;
};

class PizzeriaDB {
public:
	void addEmployee(std::string name);
	void addPizza(std::string name, double price);
	void newOrder(const Order & o);
	std::vector<Pizza> getPizzasAvailable() const;
	void complete_order();
private:
	std::vector<Pizza> availablePizzas;
	std::vector<Employee> workers;
	std::queue<Order> current_orders;
};

class Employee {
public:
	Employee(std::string name, bool free = true);
	void doWork(const Order& o);
	bool isFree() const;
private:
	std::string name;
	bool free;
};

class Pizza {
public:
	Pizza(std::string type, double price, int amount = 1);
	std::string getType() const;
	double getPrice() const;
	int getAmount() const;
private:
	std::string pizza_type;
	double price;
	unsigned int amount;
};

class User {
public:
	std::string GetLogin();
	std::string GetPassword();
	User(std::string l, std::string p);
	virtual void MainMenu() = 0;
private:
	std::string login;
	std::string password;
};

class Client : public User {
public:
	Client(std::string n, std::string ad);
	void makeOrder(PizzeriaDB& p) const;
	void MainMenu();
private:
	std::string name;
	std::string address;
};

class Admin : public User {
public:
	void MainMenu();
	void addEmployee(PizzeriaDB db, std::string name);
	void addPizza(PizzeriaDB db, std::string name, double price);
}; 

class UsersDB {
public:
	void setAdminData();
	void newClient(std::string l, std::string p);
	bool is_valid(User& u);
private:
	std::pair<std::string, std::string> adminData;
	std::map<std::string, std::string> usersData;
};

void authorisation(UsersDB);

int main()
{
	PizzeriaDB dodo;
	
}

void authorisation(UsersDB db) {
	int num;
	num = inputInt("1 - I'm admin\n2 - I'm user", 1, 2);
	
	num = inputInt("1 - sign up\n2 - log in", 1, 2);
	if (num == 1) {
		std::string l, p, n, a;;
		std::cout << "Enter login: ";
		std::cin >> l;
		std::cout << "Enter password: ";
		std::cin >> p;
		std::cout << "Enter your name: ";
		std::cin >> n;
		std::cout << "Enter address: ";
		std::cin >> a;




	}
}

std::string User::GetLogin() {
	return login;
}
std::string User::GetPassword() {
	return password;
}

bool UsersDB::is_valid(User& u) {
	if (usersData[u.GetLogin()] == u.GetPassword()) return true;
	return false;
}

void Admin::addEmployee(PizzeriaDB db, std::string name) {
	db.addEmployee(name);
}

void Admin::addPizza(PizzeriaDB db, std::string name, double price) {
	db.addPizza(name, price);
}


User::User(std::string l, std::string p) : login(l), password(p) {}

void PizzeriaDB::addEmployee(std::string name) {
	workers.emplace_back(name);
}
void PizzeriaDB::addPizza(std::string name, double price) {
	availablePizzas.emplace_back(name, price);
}
void PizzeriaDB::newOrder(const Order& o) {
	current_orders.push(o);
}
std::vector<Pizza> PizzeriaDB::getPizzasAvailable() const {
	return availablePizzas;
}
void PizzeriaDB::complete_order() {
	while (!current_orders.empty()) {
		for (Employee worker : workers) {
			if (worker.isFree()) worker.doWork(current_orders.front());
			current_orders.pop();
			break;
		}
	}
}

Employee::Employee(std::string name, bool free) : name(name), free(free) {}

void Employee::doWork(const Order& o) {
	free = false;
	std::cout << "Worker " << this->name << " is working" << std::endl;
	std::cout << "Worker has finished" << std::endl;
	free = true;
}

bool Employee::isFree() const {
	return free;
}

Client::Client(std::string n, std::string ad) : name(n), address(ad) {}

int inputInt(std::string prompt, int m, int M) {
	int N;
	std::cout << prompt << " (From " << m << " to " << M << "): ";
	if (!(std::cin >> N) || !(m <= N) || !(N <= M)) {
		std::cin.clear();
		std::cin.ignore(100, '\n');
		throw std::invalid_argument("Invalid number");
	}
	return N;
}

void Client::makeOrder(PizzeriaDB& p) const {
	Order this_order;
	std::cout << "Choose your pizza: " << std::endl;
	std::vector<Pizza> menu = p.getPizzasAvailable();
	char c = 'y';
	while (c == 'y') {
		for (int i = 0; i < menu.size(); i++) {
			std::cout << i + 1 << " " << menu[i].getType() << " " << menu[i].getPrice() << std::endl;
		}
		try {
			int pizza_number = inputInt("Enter number of your pizza", 1, menu.size());
			int amount = inputInt("Enter amount", 1);
			this_order.addPizza(menu[pizza_number - 1].getType(), menu[pizza_number - 1].getPrice(), amount);
		}
		catch (const std::invalid_argument& e) {
			std::cout << e.what() << ". Enter correct number.\n";
			continue;
		}
		std::cout << "Wanna add more? y/n" << std::endl;
		std::cin >> c;
	}
	p.newOrder(this_order);
	std::cout << "Your total is: " << this_order.getOrderPrice() << std::endl;
	std::cout << "Your order is now in work" << std::endl;
	p.complete_order();
	std::cout << "Your order is ready" << std::endl;
	std::cout << "It will be delivered to: " << this->name << ". Address: " << this->address << std::endl;
}


Pizza::Pizza(std::string type, double price, int amount) : pizza_type(type), price(price), amount(amount) {}

std::string Pizza::getType() const {
	return pizza_type;
}
double Pizza::getPrice() const {
	return price;
}
int Pizza::getAmount() const {
	return amount;
}

double Order::getOrderPrice() const {
	double sum = 0;
	for (auto now : pizzas) {
		sum += now.getPrice() * now.getAmount();
	}
	return sum;
}

void Order::addPizza(std::string name, double price, int amount) {
	pizzas.emplace_back(name, price, amount);
}
