#include <iostream>
#include <functional>
#include <sstream>
#include <string>

template<typename Item>
struct Node {
	Item i;
	Node<Item>* next;
};

template<typename Item>
class Stack {
public:
	Stack(): head(nullptr) {}
	void push(Item i) {
		Node<Item>* node = new Node<Item>;
		node->i = i;
		node->next = head;
		head = node;
	}
	Item pop() {
		if (is_empty()) {
			std::cerr << "explode!" << std::endl;
		}
		Item i = head->i;
		Node<Item>* next = head->next;
		delete head;
		head = next;
		return i;
	}
	Item peak() {
		Item i = pop();
		push(i);
		return i;
	}
	bool is_empty() {
		return head == nullptr;
	}
private:
	Node<Item>* head;
};

/*
template<typename Item>
class List {
public:
	List(): head(nullptr) {}
	void add_item(Item i) {
		Node<Item>* node = new Node<Item>;
		node->i = i;
		node->next = head;
		head = node;
	}
	bool is_empty() {
		return head == nullptr;
	}
	Node<Item>* head;
};
*/

struct Operator {
	std::function<double(double, double)> do_op;
	std::string str_op;
};

const bool is_number(const std::string& str) {
	bool ret = false; 
	for (char c: str) {
		if (!isdigit(c)) {
			ret = false;
			break;
		} else {
			ret = true;
		}
	}
	return ret;
}

class Token {
public:
	Token(const std::string& str_token = ""): is_num(false), is_op(false), val(0.0), str(str_token) {
		if (is_number(str_token)) {
			is_num = true;
			val = std::stod(str_token);
		} else {
			if (str_token == "+") {
				is_num = false;
				is_op = true;
				op.do_op = std::plus<double>();
				op.str_op = str_token;
			} else if (str_token == "-") {
				is_num = false;
				is_op = true;
				op.do_op = std::minus<double>();
				op.str_op = str_token;
			} else if (str_token == "*") {
				is_num = false;
				is_op = true;
				op.do_op = std::multiplies<double>();
				op.str_op = str_token;
			} else if (str_token == "/") {
				is_num = false;
				is_op = true;
				op.do_op = std::divides<double>();
				op.str_op = str_token;
			}
		}
	}
	Token(const double d): is_num(true), is_op(false), val(d), str(std::to_string(d)) {}
	Token(const Operator o): is_num(false), is_op(true), str(o.str_op), op(o) {}
	bool is_valid() {
		return is_num || is_op;
	}
	bool is_num;
	bool is_op;
	double val;
	std::string str;
	Operator op;
};

bool is_num_tok_node(Node<Token>* n) {
	return (n != nullptr && n->i.is_num);
}


/*
Create single-linked-list of tokens called token_list
loop until token_list.head.next is a null pointer
current_token = item stored in token_list.head;
if (current_token is an operator and current_token.next is a number and current_token.next.next is a number):
    apply operator at current_token to current_token.next and current_token.next
    replace current_token.data with the results
    set current_token.next = current_token.next.next.next
else:
    current_token = current_token.next
*/
double parse_polish_ver1(std::string expression) {
	Node<Token> *head = new Node<Token>;
	std::istringstream iss(expression);
	Token tok;
	std::string str_tok;
	auto x = head;
	while (iss >> str_tok && (tok = Token(str_tok)).is_valid()) {
		Node<Token> *n = new Node<Token>;
		n->i = tok;
		x->next = n;
		x = n;
	}
	auto current_node = head->next;
	while(head->next->next != nullptr) {
		tok = current_node->i;
		if (tok.is_op && is_num_tok_node(current_node->next) && is_num_tok_node(current_node->next->next)) {
			Operator op = tok.op;
			double first_operand = current_node->next->i.val;
			double second_operand = current_node->next->next->i.val;
			double result = op.do_op(first_operand, second_operand);
			std::cout << first_operand << op.str_op << second_operand << "=" << result << std::endl;
			Token result_tok(result);
			current_node->i = result_tok;
			current_node->next = current_node->next->next->next;
			current_node = head->next;
		} else {
			current_node = current_node->next;
		}
	}
	return head->next->i.val;
}

/*
reverse tokens by pushing each to a stack called token_stack
create number_stack
while token_stack is not empty
    token = token_stack.pop()
    if token is an operator:
        first operand = number_stack.pop()
        second operand = number_stack.pop()
        apply the operator to the two operands
        push the result to number_stack
    else if token is a number:
        push the number to number_stack
    else:
        //explode!!
result is the only item in number stack
*/
double parse_polish_ver2(std::string expression) {
	Stack<Token> token_stack;
	Stack<double> number_stack;
	std::istringstream iss(expression);
	Token tok;
	std::string str_tok;
	while (iss >> str_tok && (tok = Token(str_tok)).is_valid()) {
		token_stack.push(tok);
	}
	while (token_stack.is_empty() == false) {
		auto token = token_stack.pop();
		if (token.is_op) {
			Operator op = token.op;
			double first_operand = number_stack.pop();
			double second_operand = number_stack.pop();
			double result = op.do_op(first_operand, second_operand);
			std::cout << first_operand << op.str_op << second_operand << "=" << result << std::endl;
			number_stack.push(result);
		} else if (token.is_num) {
			number_stack.push(token.val);
		}
	}
	return number_stack.pop();
}

int main() {
	std::string expression = "- * / 15 - 7 + 1 1 3 + 2 + 1 1";
	std::cout << "version 1" << std::endl;
	double result = parse_polish_ver1(expression);
	std::cout << result << std::endl << std::endl;
	std::cout << "version 2" << std::endl;
	result = parse_polish_ver2(expression);
	std::cout << result << std::endl;
	return 0;
}
