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
	void addWorker(const Employee& e) {
		workers.push_back(e);
	}
private:
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
		std::cout << "Enter your pizzas" << std::endl;

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
private:
	std::string pizza_type;
	double price;
};

class Order{
public:
	double getOrderPrice() {
		double sum = 0;
		for (auto now : pizzas) {
			sum += now.getPrice();
		}
		return sum;
	}
private:
	std::vector<Pizza> pizzas;
}







int main()
{
	Pizzeria dodo;

}

