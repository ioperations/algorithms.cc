#include <sstream>

#include "stack.h"

struct Postfix_expr {
    struct Entry;
    struct Operand;
    struct Multiplication;
    struct Addition;

    struct Entry_visitor {
        Stack<int> stack_;
        void visit_operand(Operand& operand);
        void visit_multiplication(Multiplication& multiplication);
        void visit_addition(Addition& addition);
    };
    struct Entry {
        Entry* next_ = nullptr;
        virtual void print(std::ostream& stream) {}
        virtual void visit(Entry_visitor&) = 0;
        virtual ~Entry() {}
    };
    struct Operand : public Entry {
        int value_ = 0;
        void print(std::ostream& stream) override {
            stream << value_;
        }
        void visit(Entry_visitor& visitor) override {
            visitor.visit_operand(*this);
        }
    };
    struct Operator : public Entry {
    };
    struct Addition : Operator {
        void print(std::ostream& stream) override {
            stream << '+';
        }
        void visit(Entry_visitor& visitor) override {
            visitor.visit_addition(*this);
        }
    };
    struct Multiplication : Operator {
        void print(std::ostream& stream) override {
            stream << '*';
        }
        void visit(Entry_visitor& visitor) override {
            visitor.visit_multiplication(*this);
        }
    };

    Entry* head_ = nullptr;
    Entry* tail_ = nullptr;
    Stack<Operator*> operator_stack_;
    Operand* current_operand_ = nullptr;

    ~Postfix_expr() {
        for (auto entry = head_; entry; ) {
            auto previous = entry;
            entry = entry->next_;
            delete previous;
        }
    }
    void add(Entry* entry) {
        if (head_) 
            tail_->next_ = entry;
        else
            head_ = entry;
        tail_ = entry;
    }
    void append_current_operand() {
        if (current_operand_) {
            add(current_operand_);
            current_operand_ = nullptr;
        }
    }
    void add(char c) {
        if (c == ')') {
            append_current_operand();
            add(operator_stack_.pop());
        } else if (c == '+') {
            append_current_operand();
            operator_stack_.push(new Addition);
        } else if (c == '*') {
            append_current_operand();
            operator_stack_.push(new Multiplication);
        } else if (c >= '0' && c <= '9') {
            int value = c - '0';
            if (current_operand_)
                current_operand_->value_ = 10 * current_operand_->value_ + value;
            else {
                current_operand_ = new Operand;
                current_operand_->value_ = value;
            }
        }
    }
    void post_construct() {
        while (!operator_stack_.empty())
            add(operator_stack_.pop());
    }
    int evaluate() {
        Entry_visitor entry_visitor;
        for (auto entry = head_; entry; entry = entry->next_)
            entry->visit(entry_visitor);
        return entry_visitor.stack_.pop();
    }
};

void Postfix_expr::Entry_visitor::visit_operand(Operand& operand) {
    stack_.push(operand.value_);
}

void Postfix_expr::Entry_visitor::visit_multiplication(Multiplication& multiplication) {
   stack_.push(stack_.pop() * stack_.pop()); 
}

void Postfix_expr::Entry_visitor::visit_addition(Addition& addition) {
   stack_.push(stack_.pop() + stack_.pop()); 
}

std::ostream& operator<<(std::ostream& stream, const Postfix_expr& expr) {
    for (auto entry = expr.head_; entry; entry = entry->next_) {
        entry->print(stream);
        stream << " ";
    }
    return stream;
}

int main() {
    const char* infix = "55 * ( ( ( 9 + 8 ) * ( 4 * 6 ) ) + 7 )";
    std::cout << "infix expression: " << infix << std::endl;
    Stack<char> stack;
    const char* p;
    char c;
    Postfix_expr postfix_expr;
    for (p = infix; ; ++p) {
        c = *p;
        if (c == '\0')
            break;
        postfix_expr.add(c);
    }
    postfix_expr.post_construct();
    std::cout << "postfix expression: " << postfix_expr << std::endl;
    std::cout << "result: " << postfix_expr.evaluate() << std::endl;

}
