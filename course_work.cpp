#include <iostream>
#include <string>
#include <vector>
#include <stack>

class Pizzeria;
class Employee;
class Client;
class Order;
class Pizza;

class Pizzeria {
public:
	void addEmployee(std::string name) {
		workers.emplace_back(name);
	}
	void addPizza(std::string name, double price) {
		availablePizzas.emplace_back(name, price);
	}
private:
	std::vector<Pizza> availablePizzas;
	std::vector<Employee> workers;
	std::stack<Order> current_orders;
};

class Employee {
public:

private:
	std::string name;
};

class Client {
public:
	void makeOrder(const Pizzeria& p) {
		std::cout << "Enter your pizzas:\n 1 - pepperoni" << std::endl;

	}
private:
	std::string name;
	std::string address;
};

class Pizza {
public:
	std::string getType() {
		return pizza_type;
	}
	double getPrice() {
		return price;
	}
	int getAmount() {
		return amount;
	}
private:
	std::string pizza_type;û
	double price;
	int amount = 1;
};

class Order {
public:
	double getOrderPrice() {
		double sum = 0;
		for (auto now : pizzas) {
			sum += now.getPrice() * now.getAmount();
		}
		return sum;
	}
private:
	std::vector<Pizza> pizzas;
};







int main()
{
	Pizzeria dodo;

}

