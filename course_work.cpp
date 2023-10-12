#include <iostream>
#include <string>
#include <vector>
#include <stack>

class Employee;
class Order;
class Pizza;

class Order {
public:
	double getOrderPrice() const;
	void addPizza(std::string name, double price, int amount = 1);
private:
	std::vector<Pizza> pizzas;
};

class Pizzeria {
public:
	void addEmployee(std::string name);
	void addPizza(std::string name, double price);
	void newOrder(const Order & o);
	std::vector<Pizza> getPizzasAvailable() const;
	void complete_order();
private:
	std::vector<Pizza> availablePizzas;
	std::vector<Employee> workers;
	std::stack<Order> current_orders;
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

class Client {
public:
	Client(std::string n, std::string ad);
	void makeOrder(Pizzeria& p) const;
private:
	std::string name;
	std::string address;
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
	int amount ;
};









int main()
{
	Pizzeria dodo;
	dodo.addEmployee("Jack");
	dodo.addEmployee("Bill");
	dodo.addEmployee("Gustav");
	dodo.addPizza("Margarita", 12.5);
	dodo.addPizza("Pepperoni", 16);
	dodo.addPizza("Chicken barbecue", 18.2);
	Client william("William Smith", "Grey street 22");
	william.makeOrder(dodo);

}

void Pizzeria::addEmployee(std::string name) {
	workers.emplace_back(name);
}
void Pizzeria::addPizza(std::string name, double price) {
	availablePizzas.emplace_back(name, price);
}
void Pizzeria::newOrder(const Order& o) {
	current_orders.push(o);
}
std::vector<Pizza> Pizzeria::getPizzasAvailable() const {
	return availablePizzas;
}
void Pizzeria::complete_order() {
	for (Employee worker : workers) {
		if (worker.isFree()) worker.doWork(current_orders.top());
		current_orders.pop();
		break;
	}
}

Employee::Employee(std::string name, bool free) : name(name) {}

void Employee::doWork(const Order& o) {
	free = false;
	std::cout << "Worker is working" << std::endl;
	std::cout << "Worker has finished" << std::endl;
	free = true;
}
bool Employee::isFree() const {
	return free;
}

Client::Client(std::string n, std::string ad) : name(n), address(ad) {}
void Client::makeOrder(Pizzeria& p) const {
	Order this_order;
	std::cout << "Choose your pizza: " << std::endl;
	std::vector<Pizza> menu = p.getPizzasAvailable();
	char c = 'y';
	while (c == 'y') {
		for (int i = 0; i < menu.size(); i++) {
			std::cout << i + 1 << " " << menu[i].getType() << " " << menu[i].getPrice() << std::endl;
		}
		int pizza_number;
		int amount;
		std::cout << "Enter number of your pizza: ";
		std::cin >> pizza_number;
		std::cout << "Enter amount: ";
		std::cin >> amount;
		this_order.addPizza(menu[pizza_number - 1].getType(), menu[pizza_number - 1].getPrice(), amount);
		std::cout << "Wanna add more? y/n" << std::endl;
		std::cin >> c;
	}
	p.newOrder(this_order);
	std::cout << "Your total is: " << this_order.getOrderPrice() << std::endl;
	std::cout << "Your order is now in work" << std::endl;
	p.complete_order();
	std::cout << "Your order is ready" << std::endl;
	std::cout << "It will be delivered to: " << this->name << " on address: " << this->address << std::endl;
}

Pizza::Pizza(std::string type, double price, int amount) : pizza_type(type), price(price), amount(amount) 
	{}

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
