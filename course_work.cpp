#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <memory>
#include <unordered_map>


class Employee;
class Order;
class Pizza;
class PizzaMaker;
class DeliveryMan;
class Checkout;

class Order {
private:
	std::vector<Pizza> pizzas;
	std::string deliveryAddress;
	std::string clientName;
public:
	std::string getAddress() const;
	std::string getClientName() const;
	void setAddress(std::string);
	void setClientName(std::string);
	double getOrderPrice() const;
	void addPizza(const std::string& name, double price, int amount = 1);
};

class Checkout {
public:
	Checkout() = default;
	void processPayment(const Order& o);
};

class PizzeriaDB {
private:
	std::string AdminKey = "superadmin";
	std::vector<Pizza> availablePizzas;
	std::vector<PizzaMaker> pizzaMakers;
	std::vector<DeliveryMan> couriers;
	std::unordered_map<std::string, std::string> clientData;
	std::queue<Order> current_orders;
	Checkout kassa;
public:
	std::vector<Pizza> getPizzasAvailable() const;
	void addClient(std::string, std::string);
	std::vector<PizzaMaker> getPizzaMakers();
	std::vector<DeliveryMan> getCouriers();
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
	void paymentProcess(const Order&);
};

class Employee {
protected:
	std::string name;
	bool free = 1;
public:
	Employee() = default;
	Employee(const std::string& name, bool free = true);
	bool isFree() const;
	void setName(const std::string& n);
	const std::string& getName() const;
};

class PizzaMaker : public Employee {
public:
	void makePizza(const Order&);
};

class DeliveryMan : public Employee {
public:
	void deliver(const Order&);
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

std::string Order::getAddress() const {
	return deliveryAddress;
}

std::string Order::getClientName() const {
	return clientName;
}

void Order::setAddress(std::string s) {
	deliveryAddress = s;
}

void Order::setClientName(std::string s) {
	clientName = s;
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

std::vector<PizzaMaker> PizzeriaDB::getPizzaMakers() {
	return pizzaMakers;
}

std::vector<DeliveryMan> PizzeriaDB::getCouriers() {
	return couriers;
}

void PizzeriaDB::addEmployee(const std::string& name) {
	std::cout << "1 - Add pizza maker\n2 - Add deliveryman\n";
	int n = inputInt("Choose number", 1, 2);
	switch (n) {
	case 1:
		pizzaMakers.push_back(PizzaMaker(name));
		break;
	case 2:
		couriers.push_back(DeliveryMan(name));
		break;
	}
}

void PizzeriaDB::deleteEmployee(const std::string& s) {
	std::cout << "1 - Delete pizza maker\n2 - Delete deliveryman\n";
	int n = inputInt("Choose number", 1, 2);
	int origSize;
	switch (n) {
	case 1:
		origSize = pizzaMakers.size();
		std::erase_if(pizzaMakers, [&s](PizzaMaker& e) { return e.getName() == s;  });\
		if (origSize != pizzaMakers.size()) {
			std::cout << "Successfully deleted employee: " << s << std::endl;
		}
		else std::cout << "No employee with such name was found" << std::endl;
		break;
	case 2:
		origSize = couriers.size();
		std::erase_if(couriers, [&s](DeliveryMan& e) { return e.getName() == s;  });
		if (origSize != couriers.size()) {
			std::cout << "Successfully deleted employee: " << s << std::endl;
		}
		else std::cout << "No employee with such name was found" << std::endl;
		break;
	}
	
}

void PizzeriaDB::addPizza(const std::string& name, double price) {
	Pizza temp(name, price);
	availablePizzas.push_back(temp);
}

void PizzeriaDB::deletePizza(const std::string& s) {
	int origSize = availablePizzas.size();
	std::erase_if(availablePizzas, [&s](Pizza& p) { return p.getPizzaType() == s; });
	if (origSize != availablePizzas.size()) {
		std::cout << "Successfully deleted pizza: " << s << std::endl;
	}
	else std::cout << "No pizza with such name was found" << std::endl;
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
		for (PizzaMaker maker : pizzaMakers) {
			if (maker.isFree()) maker.makePizza(current_orders.front());
			for (DeliveryMan cour : couriers) {
				if (cour.isFree()) cour.deliver(current_orders.front());
			}
			current_orders.pop();
			break;
		}
	}
}

void PizzeriaDB::saveDB() {
	// ������ AdminKey
	std::ofstream out("AdminKey.bin", std::ios::binary | std::ios::out);
	if (!out) {
		std::cout << "Failed to open AdminKey.bin for writing." << std::endl;
		return;
	}
	size_t len = AdminKey.size();
	out.write((char*)&len, sizeof(size_t));
	out.write(AdminKey.c_str(), len);
	out.close();

	// ������ PizzaCatalogue
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

	// ������ PizzaMakersDB
	out.open("WorkersDB.bin", std::ios::binary | std::ios::out);
	if (!out) {
		std::cout << "Failed to open WorkersDB.bin for writing." << std::endl;
		return;
	}
	size_t PizzaMakerCount = pizzaMakers.size();
	out.write((char*)&PizzaMakerCount, sizeof(size_t));
	for (const auto& worker : pizzaMakers) {
		size_t nameLen = worker.getName().size();
		out.write((char*)&nameLen, sizeof(size_t));
		out.write(worker.getName().c_str(), nameLen);
	}
	out.close();

	// ������ DeliveryManDB
	out.open("DeliveryManDB.bin", std::ios::binary | std::ios::out);
	if (!out) {
		std::cout << "Failed to open DeliveryManDB.bin for writing." << std::endl;
		return;
	}
	size_t workerCount = couriers.size();
	out.write((char*)&workerCount, sizeof(size_t));
	for (const auto& worker : couriers) {
		size_t nameLen = worker.getName().size();
		out.write((char*)&nameLen, sizeof(size_t));
		out.write(worker.getName().c_str(), nameLen);
	}
	out.close();

	// ������ ClientData
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
	// ������ AdminKey
	std::ifstream in("AdminKey.bin", std::ios::binary);
	if (!in) {
		std::cout << "Failed to open AdminKey.bin for reading." << std::endl;
		return;
	}
	if (in.peek() == std::ifstream::traits_type::eof()) {
		return;
	}
	size_t len;
	in.read((char*)&len, sizeof(size_t));
	AdminKey.resize(len);
	in.read(&AdminKey[0], len);
	in.close();

	// ������ PizzaCatalogue
	in.open("PizzaCatalogue.bin", std::ios::binary);
	if (!in) {
		std::cout << "Failed to open PizzaCatalogue.bin for reading." << std::endl;
		return;
	}
	if (in.peek() == std::ifstream::traits_type::eof()) {
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

	// ������ PizzaMakerDB
	in.open("WorkersDB.bin", std::ios::binary);
	if (!in) {
		std::cout << "Failed to open WorkersDB.bin for reading." << std::endl;
		return;
	}
	if (in.peek() == std::ifstream::traits_type::eof()) {
		return;
	}
	size_t PizzaMakerCount;
	in.read((char*)&PizzaMakerCount, sizeof(size_t));
	pizzaMakers.resize(PizzaMakerCount);
	for (size_t i = 0; i < PizzaMakerCount; ++i) {
		PizzaMaker employee;
		size_t nameLen = 20;
		in.read((char*)&nameLen, sizeof(size_t));
		std::string name(nameLen, '\0');
		in.read(&name[0], nameLen);
		employee.setName(name);
		pizzaMakers[i] = employee;
	}
	in.close();

	// ������ DeliveryManDB
	in.open("DeliveryManDB.bin", std::ios::binary);
	if (!in) {
		std::cout << "Failed to open WorkersDB.bin for reading." << std::endl;
		return;
	}
	if (in.peek() == std::ifstream::traits_type::eof()) {
		return;
	}
	size_t workerCount;
	in.read((char*)&workerCount, sizeof(size_t));
	couriers.resize(workerCount);
	for (size_t i = 0; i < workerCount; ++i) {
		DeliveryMan employee;
		size_t nameLen = 20;
		in.read((char*)&nameLen, sizeof(size_t));
		std::string name(nameLen, '\0');
		in.read(&name[0], nameLen);
		employee.setName(name);
		couriers[i] = employee;
	}
	in.close();

	// ������ ClientData
	in.open("ClientData.bin", std::ios::binary);
	if (!in) {
		std::cout << "Failed to open ClientData.bin for reading." << std::endl;
		return;
	}
	if (in.peek() == std::ifstream::traits_type::eof()) {
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

void PizzeriaDB::paymentProcess(const Order& o) {
	kassa.processPayment(o);
}

Employee::Employee(const std::string& name, bool free) : name(name), free(free) {}

void PizzaMaker::makePizza(const Order& o) {
	free = false;
	std::cout << "Worker " << this->name << " is working" << std::endl;
	std::cout << "Worker has finished" << std::endl;
	free = true;
}

void DeliveryMan::deliver(const Order& o) {
	free = false;
	std::cout << "Courier " << this->name << " is delireving to " << o.getClientName() << " on address: " << o.getAddress() << std::endl;
	std::cout << "Delivery man has delivered" << std::endl;
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
	std::string temp;
	Order this_order;
	std::cout << "Enter your name: ";
	std::cin >> temp;
	this_order.setClientName(temp);
	std::cout << "Enter your address: ";
	std::cin >> temp;
	this_order.setAddress(temp);
	std::cout << "Choose your pizza: " << std::endl;
	std::vector<Pizza> menu = std::move(p->getPizzasAvailable());
	char c = 'y';
	while (c == 'y') {
		for (int i = 0; i < menu.size(); i++) {
			std::cout << i + 1 << " " << menu[i].getPizzaType() << " " << menu[i].getPrice() << std::endl;
		}
		int pizza_number = inputInt("Enter number of your pizza", 1, menu.size());
		int amount = inputInt("Enter amount", 1);
		this_order.addPizza(menu[pizza_number - 1].getPizzaType(), menu[pizza_number - 1].getPrice(), amount);
		std::cout << "Wanna add more? y/n" << std::endl;
		std::cin >> c;
	}
	p->newOrder(this_order);
	std::cout << "Your total is: " << this_order.getOrderPrice() << std::endl;
	p->paymentProcess(this_order);
	std::cout << "Your order is now in work" << std::endl;
	p->complete_order();
}

void Client::MainMenu(std::shared_ptr<PizzeriaDB> db) {
	while (true) {
		std::cout << "1 - Make order\n2 - List of Pizzas\n3 - Exit\n";
		int num = inputInt("Choose number", 1, 3);
		std::string s;
		switch (num) {
		case 1:
			this->makeOrder(db);
			break;
		case 2: {
			std::vector<Pizza> menu = std::move(db->getPizzasAvailable());
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
		std::cout << "1 - List of pizzas\n2 - Add pizza\n3 - Delete pizza\n4 - List of pizzamakers\n5 - List of couriers\n6 - Add employee\n7 - Delete employee\n8 - Change admin key\n9 - Exit\n";
		int num = inputInt("Choose number", 1, 9);
		std::string s;
		switch (num) {
		case 1: {
			std::vector<Pizza> menu = std::move(db->getPizzasAvailable());
			for (int i = 0; i < menu.size(); i++) {
				std::cout << i + 1 << " " << menu[i].getPizzaType() << " " << menu[i].getPrice() << std::endl;
			}
			break;
		}
		case 2:
			double p;
			std::cout << "Enter pizza name: ";
			std::cin.ignore();
			std::getline(std::cin, s);
			std::cout << "Enter pizza price: ";
			std::cin >> p;
			db->addPizza(s, p);
			break;
		case 3:
			std::cout << "Enter pizza name: ";
			std::cin.ignore();
			std::getline(std::cin, s);
			db->deletePizza(s);
			break;
		case 4: {
			std::vector<PizzaMaker> workers = std::move(db->getPizzaMakers());
			for (PizzaMaker e : workers) {
				std::cout << e.getName() << std::endl;
			}
			break;
		}
		case 5: {
			std::vector<DeliveryMan> workers = std::move(db->getCouriers());
			for (DeliveryMan e : workers) {
				std::cout << e.getName() << std::endl;
			}
			break;
		}
		case 6:
			std::cout << "Enter employee name: ";
			std::cin >> s;
			db->addEmployee(s);
			break;
		case 7:
			std::cout << "Enter employee name: ";
			std::cin >> s;
			db->deleteEmployee(s);
			break;
		case 8:
			std::cout << "Enter new admin key: ";
			std::cin >> s;
			db->setAdminKey(s);
			break;
		case 9:
			return;
			break;
		}
	}
}

void Checkout::processPayment(const Order& o) {
	std::cout << "You need to pay: " << o.getOrderPrice() << std::endl;
	std::cout << "1 - Cash\n2 - Credit card\n3 - Credit card online\n";
	int n = inputInt("Choose number", 1, 3);
	switch (n) {
	case 1:
		return;
	case 2:
		return;
	case 3:
		std::cout << "Enter your card: ";
		std::string card_number;
		std::cin >> card_number;
		std::cout << "Payment successful" << std::endl;
		return;
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