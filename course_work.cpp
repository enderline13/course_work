#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <fstream>

class Employee;
class Order;
class Pizza;


class Order {
public:
	double getOrderPrice() const;
	void addPizza(const std::string& name, double price, int amount = 1);
private:
	std::vector<Pizza> pizzas;
};

class PizzeriaDB {
public:
	std::vector<Pizza> getPizzasAvailable() const;
	void addClient(std::string, std::string);
	void addEmployee(const std::string& name);
	void deleteEmployee(const std::string& s);
	void addPizza(const std::string& name, double price);
	void deletePizza(const std::string& s);
	void setAdminKey(const std::string& s);
	bool AdminIsValid(const std::string&) const;
	bool ClientIsValid(const std::string&, const std::string&) const;
	void newOrder(const Order& o);
	void complete_order();
	void GetDB();
	void SaveDB();
private:
	std::string AdminKey = "superadmin";
	std::vector<Pizza> availablePizzas;
	std::vector<Employee> workers;
	std::map<std::string, std::string> clientData;
	std::queue<Order> current_orders;
};

class Employee {
public:
	Employee(const std::string& name, bool free = true);
	void doWork(const Order& o);
	bool isFree() const;
	std::string getName() const;
private:
	std::string name;
	bool free;
};

class Pizza {
public:
	Pizza(const std::string& type, double price, int amount = 1);
	std::string getType() const;
	double getPrice() const;
	int getAmount() const;
	std::string getName() const;
private:
	std::string pizza_type;
	double price;
	unsigned int amount;
};

class User {
public:
	User() = default;
	virtual void MainMenu(PizzeriaDB* db) = 0;
};

class Client : public User {
public:
	Client() = default;
	void makeOrder(PizzeriaDB* db);
	void MainMenu(PizzeriaDB* db) override;
};

class Admin : public User {
public:
	Admin() = default;
	void MainMenu(PizzeriaDB* db) override;
}; 

int inputInt(const std::string& prompt, int m = 1, int M = 1000);

User* authorisation(PizzeriaDB* db);



int main()
{
	PizzeriaDB* dodo = new PizzeriaDB;
	dodo->GetDB();
	User* current_user = authorisation(dodo);
	current_user->MainMenu(dodo);
	dodo->SaveDB();
	delete current_user;
	delete dodo;
}
/*
std::string AdminKey;
std::vector<Pizza> availablePizzas;
std::vector<Employee> workers;
std::map<std::string, std::string> clientData;
*/
void PizzeriaDB::GetDB() {
	std::fstream in("AdminKey.bin", std::ios::binary | std::ios::in | std::ios::out);
	if (!in) std::cout << "bad file";
	in.read((char*)&AdminKey, sizeof(std::string));
	in.close();
	in.open("PizzaCatalogue.bin", std::ios::binary | std::ios::in | std::ios::out);
	in.read((char*)&availablePizzas, sizeof(std::vector<Pizza>));
	in.close();
	in.open("WorkersDB.bin", std::ios::binary | std::ios::in | std::ios::out);
	in.read((char*)&workers, sizeof(std::vector<Employee>));
	in.close();
	in.open("ClientData.bin", std::ios::binary | std::ios::in | std::ios::out);
	in.read((char*)&clientData, sizeof(std::map<std::string, std::string>));
	in.close();
}

void PizzeriaDB::SaveDB() {
	std::ofstream out("AdminKey.bin", std::ios::binary | std::ios::out);
	if (!out) std::cout << "bad file";
	out.write((char*)&AdminKey, sizeof(std::string));
	out.close();
	out.open("PizzaCatalogue.bin", std::ios::binary | std::ios::out);
	out.write((char*)&availablePizzas, sizeof(std::vector<Pizza>));
	out.close();
	out.open("WorkersDB.bin", std::ios::binary | std::ios::out);
	out.write((char*)&workers, sizeof(std::vector<Employee>));
	out.close();
	out.open("ClientData.bin", std::ios::binary | std::ios::out);
	out.write((char*)&clientData, sizeof(std::map<std::string, std::string>));
	out.close();
}

double Order::getOrderPrice() const {
	double sum = 0;
	for (auto now : pizzas) {
		sum += now.getPrice() * now.getAmount();
	}
	return sum;
}

void Order::addPizza(const std::string& name, double price, int amount) {
	pizzas.emplace_back(name, price, amount);
}

std::vector<Pizza> PizzeriaDB::getPizzasAvailable() const {
	return availablePizzas;
}

void PizzeriaDB::addClient(std::string l, std::string p) {
	clientData[l] = p;
}

void PizzeriaDB::addEmployee(const std::string& name) {
	workers.emplace_back(name);
}

void PizzeriaDB::deleteEmployee(const std::string& s) {
	std::erase_if(workers, [&s](Employee& e) { return e.getName() == s; });
}

void PizzeriaDB::addPizza(const std::string& name, double price) {
	availablePizzas.emplace_back(name, price);
}

void PizzeriaDB::deletePizza(const std::string& s) {
	std::erase_if(availablePizzas, [&s](Pizza& p) { return p.getName() == s; });
}

void PizzeriaDB::setAdminKey(const std::string& s) {
	AdminKey = s;
}

bool PizzeriaDB::AdminIsValid(const std::string& s) const {
	return s == AdminKey;
}

bool PizzeriaDB::ClientIsValid(const std::string& l, const std::string& p) const {
	return clientData.contains(l) and clientData.at(l) == p;
}

void PizzeriaDB::newOrder(const Order& o) {
	current_orders.push(o);
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

Employee::Employee(const std::string& name, bool free) : name(name), free(free) {}

void Employee::doWork(const Order& o) {
	free = false;
	std::cout << "Worker " << this->name << " is working" << std::endl;
	std::cout << "Worker has finished" << std::endl;
	free = true;
}

bool Employee::isFree() const {
	return free;
}

std::string Employee::getName() const {
	return name;
}

Pizza::Pizza(const std::string& type, double price, int amount) : pizza_type(type), price(price), amount(amount) {}

std::string Pizza::getType() const {
	return pizza_type;
}

double Pizza::getPrice() const {
	return price;
}

int Pizza::getAmount() const {
	return amount;
}

std::string Pizza::getName() const {
	return pizza_type;
}

void Client::makeOrder(PizzeriaDB* p) {
	std::string name, address;
	std::cout << "Enter your name: ";
	std::cin >> name;
	std::cout << "Enter your address: ";
	std::cin >> address;
	Order this_order;
	std::cout << "Choose your pizza: " << std::endl;
	std::vector<Pizza> menu = p->getPizzasAvailable();
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
	p->newOrder(this_order);
	std::cout << "Your total is: " << this_order.getOrderPrice() << std::endl;
	std::cout << "Your order is now in work" << std::endl;
	p->complete_order();
	std::cout << "Your order is ready" << std::endl;
	std::cout << "It will be delivered to: " << name << ". Address: " << address << std::endl;
}

void Client::MainMenu(PizzeriaDB* db) {
	while (true) {
		std::cout << "1 - Make order\n2 - List of Pizzas\n3 - Exit\n";
		int num = inputInt("Choose number: ", 1, 3);
		std::string s;
		switch (num) {
		case 1:
			this->makeOrder(db);
			break;
		case 2: {
			std::vector<Pizza> menu = db->getPizzasAvailable();
			for (int i = 0; i < menu.size(); i++) {
				std::cout << i + 1 << " " << menu[i].getType() << " " << menu[i].getPrice() << std::endl;
			}
			break;
		}
		case 3:
			exit(0);
			break;
		}
	}
}

void Admin::MainMenu(PizzeriaDB* db) {
	while (true) {
		std::cout << "1 - Add pizza\n2 - Delete pizza\n3 - Add employee\n4 - Delete employee\n5 - List of pizzas\n6 - Change admin key\n7 - Exit\n";
		int num = inputInt("Choose number: ", 1, 7);
		std::string s;
		switch (num) {
		case 1:
			double p;
			std::cout << "Enter pizza name: ";
			std::cin.ignore();
			std::getline(std::cin, s);
			std::cout << "Enter pizza price: ";
			std::cin >> p;
			db->addPizza(s, p);
			break;
		case 2:
			std::cout << "Enter pizza name: ";
			std::getline(std::cin, s);
			db->deletePizza(s);
			break;
		case 3:
			std::cout << "Enter employee name: ";
			std::cin >> s;
			db->addEmployee(s);
			break;
		case 4:
			std::cout << "Enter employee name: ";
			std::cin >> s;
			db->deleteEmployee(s);
			break;
		case 5: {
			std::vector<Pizza> menu = db->getPizzasAvailable();
			for (int i = 0; i < menu.size(); i++) {
				std::cout << i + 1 << " " << menu[i].getType() << " " << menu[i].getPrice() << std::endl;
			}
			break;
		}
		case 6:
			std::cout << "Enter new admin key: ";
			std::cin >> s;
			db->setAdminKey(s);
			break;
		case 7:
			exit(0);
			break;
		}
	}
}

int inputInt(const std::string& prompt, int m, int M) {
	int N;
	while (true) {
		std::cout << prompt << " (From " << m << " to " << M << "): ";
		if (!(std::cin >> N) || !(m <= N) || !(N <= M)) {
			std::cin.clear();
			std::cin.ignore(100, '\n');
			std::cout << "Incorrect number" << std::endl;
		}
		else return N;
	}
}

User* authorisation(PizzeriaDB* db) {
	int num;
	while (true) {
		num = inputInt("1 - I'm admin\n2 - I'm user\n3 - Exit\n", 1, 3);
		if (num == 1) {
			std::string s;
			std::cout << "Enter admin access key: ";
			std::cin >> s;
			if (db->AdminIsValid(s)) return new Admin;
			else std::cout << "Wrong key" << std::endl;
		}
		else if (num == 2) {
			num = inputInt("1 - sign up\n2 - log in", 1, 2);
			if (num == 1) {
				std::string l, p;
				std::cout << "Enter login: ";
				std::cin >> l;
				std::cout << "Enter password: ";
				std::cin >> p;
				db->addClient(l, p);
				return new Client;
			}
			else {
				std::string l, p;
				std::cout << "Enter login: ";
				std::cin >> l;
				std::cout << "Enter password: ";
				std::cin >> p;
				if (db->ClientIsValid(l, p)) return new Client;
				else std::cout << "Wrong login or password" << std::endl;
			}
		}
		else if (num == 3) exit(0);
	}
}






