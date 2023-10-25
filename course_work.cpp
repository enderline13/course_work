#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <fstream>
#include <memory>

class Employee;
class Order;
class Pizza;


class Order {
private:
	std::vector<Pizza> pizzas;
public:
	double getOrderPrice() const;
	void addPizza(const std::string& name, double price, int amount = 1);
};

class PizzeriaDB {
private:
	std::string AdminKey = "superadmin";
	std::vector<Pizza> availablePizzas;
	std::vector<Employee> workers;
	std::map<std::string, std::string> clientData;
	std::queue<Order> current_orders;
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
	void getDB();
	void saveDB();
};

class Employee {
private:
	std::string name;
	bool free = 1;
public:
	Employee() = default;
	Employee(const std::string& name, bool free = true);
	void doWork(const Order& o);
	bool isFree() const;
	void setName(const std::string& n);
	const std::string& getName() const;
};

class Pizza {
private:
	std::string pizza_type;
	double price = 1;
	unsigned int amount = 1;
public:
	Pizza() = default;
	Pizza(const std::string& type, double price, int amount = 1);
	void setPizzaType(const std::string& type);
	void setPrice(double p);
	void setAmount(unsigned int a);
	const std::string& getPizzaType() const;
	double getPrice() const;
	unsigned int getAmount() const;
};

class User {
public:
	virtual void MainMenu(std::shared_ptr<PizzeriaDB> db) = 0;
};

class Client : public User {
public:
	void makeOrder(std::shared_ptr<PizzeriaDB> p);
	virtual void MainMenu(std::shared_ptr<PizzeriaDB> db) override;
};

class Admin : public User {
public:
	virtual void MainMenu(std::shared_ptr<PizzeriaDB> db) override;
};

int inputInt(const std::string& prompt, int m = 1, int M = 1000);

std::shared_ptr<User> authorisation(std::shared_ptr<PizzeriaDB>);



int main()
{
	std::shared_ptr<PizzeriaDB> dodo = std::make_shared<PizzeriaDB>();
	dodo->getDB();
	std::shared_ptr<User> current_user = authorisation(dodo);
	current_user->MainMenu(dodo);
	dodo->saveDB();
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
	Employee employee(name);
	workers.push_back(employee);
}

void PizzeriaDB::deleteEmployee(const std::string& s) {
	std::erase_if(workers, [&s](Employee& e) { return e.getName() == s; });
}

void PizzeriaDB::addPizza(const std::string& name, double price) {
	Pizza temp(name, price);
	availablePizzas.push_back(temp);
}

void PizzeriaDB::deletePizza(const std::string& s) {
	std::erase_if(availablePizzas, [&s](Pizza& p) { return p.getPizzaType() == s; });
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

void PizzeriaDB::saveDB() {
	// Запись AdminKey
	std::ofstream out("AdminKey.bin", std::ios::binary | std::ios::out);
	if (!out) {
		std::cout << "Failed to open AdminKey.bin for writing." << std::endl;
		return;
	}
	size_t len = AdminKey.size();
	out.write((char*)&len, sizeof(size_t));
	out.write(AdminKey.c_str(), len);
	out.close();

	// Запись PizzaCatalogue
	out.open("PizzaCatalogue.bin", std::ios::binary | std::ios::out);
	if (!out) {
		std::cout << "Failed to open PizzaCatalogue.bin for writing." << std::endl;
		return;
	}
	size_t pizzaCount = availablePizzas.size();
	out.write((char*)&pizzaCount, sizeof(size_t));
	for (const auto& pizza : availablePizzas) {
		size_t typeLen = pizza.getPizzaType().size();
		out.write((char*)&typeLen, sizeof(size_t));
		const std::string& pizzaType = pizza.getPizzaType();
		out.write(pizzaType.c_str(), typeLen);
		double price = pizza.getPrice();
		out.write((char*)&price, sizeof(double));
	}
	out.close();

	// Запись WorkersDB
	out.open("WorkersDB.bin", std::ios::binary | std::ios::out);
	if (!out) {
		std::cout << "Failed to open WorkersDB.bin for writing." << std::endl;
		return;
	}
	size_t workerCount = workers.size();
	out.write((char*)&workerCount, sizeof(size_t));
	for (const auto& worker : workers) {
		size_t nameLen = worker.getName().size();
		out.write((char*)&nameLen, sizeof(size_t));
		out.write(worker.getName().c_str(), nameLen);
	}
	out.close();

	// Запись ClientData
	out.open("ClientData.bin", std::ios::binary | std::ios::out);
	if (!out) {
		std::cout << "Failed to open ClientData.bin for writing." << std::endl;
		return;
	}
	size_t clientDataSize = clientData.size();
	out.write((char*)&clientDataSize, sizeof(size_t));
	for (const auto& entry : clientData) {
		size_t keyLen = entry.first.size();
		out.write((char*)&keyLen, sizeof(size_t));
		out.write(entry.first.c_str(), keyLen);

		size_t valueLen = entry.second.size();
		out.write((char*)&valueLen, sizeof(size_t));
		out.write(entry.second.c_str(), valueLen);
	}
	out.close();
}

void PizzeriaDB::getDB() {
	// Чтение AdminKey
	std::ifstream in("AdminKey.bin", std::ios::binary);
	if (!in) {
		std::cout << "Failed to open AdminKey.bin for reading." << std::endl;
		return;
	}
	if (in.peek() == std::ifstream::traits_type::eof()) {
		// Файл пуст, пропускаем чтение
		return;
	}
	size_t len;
	in.read((char*)&len, sizeof(size_t));
	AdminKey.resize(len);
	in.read(&AdminKey[0], len);
	in.close();

	// Чтение PizzaCatalogue
	in.open("PizzaCatalogue.bin", std::ios::binary);
	if (!in) {
		std::cout << "Failed to open PizzaCatalogue.bin for reading." << std::endl;
		return;
	}
	if (in.peek() == std::ifstream::traits_type::eof()) {
		// Файл пуст, пропускаем чтение
		return;
	}
	size_t pizzaCount;
	in.read((char*)&pizzaCount, sizeof(size_t));
	availablePizzas.resize(pizzaCount);
	for (size_t i = 0; i < pizzaCount; ++i) {
		Pizza pizza;
		size_t typeLen;
		in.read((char*)&typeLen, sizeof(size_t));
		std::string pizzaType(typeLen, '\0');
		in.read(&pizzaType[0], typeLen);
		pizza.setPizzaType(pizzaType);
		double price;
		in.read((char*)&price, sizeof(double));
		pizza.setPrice(price);
		availablePizzas[i] = pizza;
	}
	in.close();

	// Чтение WorkersDB
	in.open("WorkersDB.bin", std::ios::binary);
	if (!in) {
		std::cout << "Failed to open WorkersDB.bin for reading." << std::endl;
		return;
	}
	if (in.peek() == std::ifstream::traits_type::eof()) {
		// Файл пуст, пропускаем чтение
		return;
	}
	size_t workerCount;
	in.read((char*)&workerCount, sizeof(size_t));
	workers.resize(workerCount);
	for (size_t i = 0; i < workerCount; ++i) {
		Employee employee;
		size_t nameLen = 20;
		in.read((char*)&nameLen, sizeof(size_t));
		std::string name(nameLen, '\0');
		in.read(&name[0], nameLen);
		employee.setName(name);
		workers[i] = employee;
	}
	in.close();

	// Чтение ClientData
	in.open("ClientData.bin", std::ios::binary);
	if (!in) {
		std::cout << "Failed to open ClientData.bin for reading." << std::endl;
		return;
	}
	if (in.peek() == std::ifstream::traits_type::eof()) {
		// Файл пуст, пропускаем чтение
		return;
	}
	size_t clientDataSize;
	in.read((char*)&clientDataSize, sizeof(size_t));
	clientData.clear();
	for (size_t i = 0; i < clientDataSize; ++i) {
		size_t keyLen, valueLen;
		in.read((char*)&keyLen, sizeof(size_t));
		std::string key(keyLen, '\0');
		in.read(&key[0], keyLen);

		in.read((char*)&valueLen, sizeof(size_t));
		std::string value(valueLen, '\0');
		in.read(&value[0], valueLen);

		clientData[key] = value;
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

void Employee::setName(const std::string& n) {
	name = n;
}

const std::string& Employee::getName() const {
	return name;
}

Pizza::Pizza(const std::string& type, double price, int amount) : pizza_type(type), price(price), amount(amount) {}

void Pizza::setPizzaType(const std::string& type) {
	pizza_type = type;
}

void Pizza::setPrice(double p) {
	price = p;
}

void Pizza::setAmount(unsigned int a) {
	amount = a;
}

const std::string& Pizza::getPizzaType() const {
	return pizza_type;
}

double Pizza::getPrice() const {
	return price;
}

unsigned int Pizza::getAmount() const {
	return amount;
}

void Client::makeOrder(std::shared_ptr<PizzeriaDB> p) {
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
			std::cout << i + 1 << " " << menu[i].getPizzaType() << " " << menu[i].getPrice() << std::endl;
		}
		try {
			int pizza_number = inputInt("Enter number of your pizza", 1, menu.size());
			int amount = inputInt("Enter amount", 1);
			this_order.addPizza(menu[pizza_number - 1].getPizzaType(), menu[pizza_number - 1].getPrice(), amount);
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

void Client::MainMenu(std::shared_ptr<PizzeriaDB> db) {
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
				std::cout << i + 1 << " " << menu[i].getPizzaType() << " " << menu[i].getPrice() << std::endl;
			}
			break;
		}
		case 3:
			return;
			break;
		}
	}
}

void Admin::MainMenu(std::shared_ptr<PizzeriaDB> db) {
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
			std::cin.ignore();
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
				std::cout << i + 1 << " " << menu[i].getPizzaType() << " " << menu[i].getPrice() << std::endl;
			}
			break;
		}
		case 6:
			std::cout << "Enter new admin key: ";
			std::cin >> s;
			db->setAdminKey(s);
			break;
		case 7:
			return;
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

std::shared_ptr<User> authorisation(std::shared_ptr<PizzeriaDB> db) {
	int num;
	while (true) {
		num = inputInt("1 - I'm admin\n2 - I'm user\n3 - Exit\n", 1, 3);
		if (num == 1) {
			std::string s;
			std::cout << "Enter admin access key: ";
			std::cin >> s;
			if (db->AdminIsValid(s)) return std::make_shared<Admin>();
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
				return std::make_shared<Client>();
			}
			else {
				std::string l, p;
				std::cout << "Enter login: ";
				std::cin >> l;
				std::cout << "Enter password: ";
				std::cin >> p;
				if (db->ClientIsValid(l, p)) return std::make_shared<Client>();
				else std::cout << "Wrong login or password" << std::endl;
			}
		}
		else if (num == 3) exit(0);
	}
}