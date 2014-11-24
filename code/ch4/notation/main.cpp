#include <iostream>
#include <string>
#include <sstream>

using namespace std;

typedef double operand;
typedef std::string token;

template <typename T>
class Stack {
private:
    struct Node {
        T item;
        Node* next;
    };
    Node* head;
public:
    Stack() {
        head = new Node();
        head->next = nullptr;
    }
    ~Stack() {
        Node* n;
        while ((n = head->next) != nullptr) {
            head->next = n->next;
            delete n;
        }
    }
    void push(const T& item) {
        Node* n = new Node();
        n->item = item;
        n->next = head->next;
        head->next = n;
    }
    T pop() {
        Node* n = head->next;
        head->next = n->next;
        T item = n->item;
        delete n;
        return item;
    }
    bool is_empty() {
        return head->next == nullptr;
    }
};

/*
    PostFix notation, AKA Reverse Polish Notation
*/

operand eval_postfix(std::string expression) {
    Stack<operand> operand_stack;
    std::istringstream in(expression);
    token tok;
    while (in >> tok) {
        if (tok == "+") {
            operand op2 = operand_stack.pop();
            operand op1 = operand_stack.pop();
            operand result = op1 + op2;
            operand_stack.push(result);
        } else if (tok == "*") {
            operand op2 = operand_stack.pop();
            operand op1 = operand_stack.pop();
            operand result = op1 * op2;
            operand_stack.push(result);
        } else if (tok == "-") {
            operand op2 = operand_stack.pop();
            operand op1 = operand_stack.pop();
            operand result = op1 - op2;
            operand_stack.push(result);
        } else if (tok == "/") {
            operand op2 = operand_stack.pop();
            operand op1 = operand_stack.pop();
            operand result = op1 / op2;
            operand_stack.push(result);
        } else {
            operand_stack.push(std::stoi(tok));
        }
    }
    return operand_stack.pop();
}

/*
    Infix notation, ie traditional arithmetic expressions
    This function converts the parenthesized arithmetic expression to postfix, then evaluates that
*/
operand eval_infix(std::string expression) {
    Stack<token> operator_stack;
    std::istringstream in(expression);
    std::ostringstream postfix;
    token tok;
    while (in >> tok) {
        if (tok == "(") {
            //do nothing
        } else if (tok == "*" || tok == "/" || tok == "+" || tok == "-") {
            operator_stack.push(tok);
        } else if (tok == ")") {
            postfix << operator_stack.pop() + " ";
        } else {
            postfix << tok << " ";
        }
    }
    return eval_postfix(postfix.str());
}

int main()
{
    std::string rpn_expr = "5 9 8 + 4 6 * * 7 + *";
    std::cout << "Evaluating postfix expression: " << rpn_expr << std::endl;
    std::cout << "Result: " << eval_postfix(rpn_expr) << std::endl;

    std::string infix_expr = "( ( 5 + ( 3 - ( 2 * 10 ) ) ) / 5 )";
    std::cout << "Evaluating infix expression: " << infix_expr << std::endl;
    std::cout << "Result: " << eval_infix(infix_expr) << std::endl;


    return 0;
}
