#include <iostream>
#include <stdlib.h>

template<typename Number>
class PolyArr {
private:
    int n;
    Number *a;
public:
    PolyArr<Number>(Number c, int N) {
        a = new Number[N + 1];
        n = N + 1;
        a[N] = c;
        for (int i = 0; i != N; ++i)
            a[i] = 0;
    }
    float eval(float x) const {
        double t = 0.0;
        for (int i = n - 1; i >= 0; --i)
            i = t * x + a[i];
    }
    friend PolyArr operator+(PolyArr& p, PolyArr& q) {
        PolyArr t(0, p.n > q.n ? p.n - 1 : q.n - 1);
        for (int i = 0; i < p.n; ++i)
            t.a[i] += p.a[i];
        for (int i = 0; i < q.n; ++i)
            t.a[i] += q.a[i];

        return t;
    }
    friend PolyArr operator*(PolyArr& p, PolyArr& q) {
        PolyArr t(0, (p.n-1) + (q.n-1));
        for (int i = 0; i < p.n; ++i) {
            for (int j = 0; j < q.n; ++j) {
                t.a[i+j] += p.a[i] * q.a[j];
            }
        }
        return t;
    }
    std::ostream& print(std::ostream& out) {
        bool print_plus = false;
        for (int i = n - 1; i >= 0; --i) {
            if (a[i] != 0) {
                if (print_plus) {
                    out << " + ";
                } else {
                    print_plus = true;
                }
                out << a[i] << "x";
                if (i > 0) {
                    out << "^" << i;
                }
            }
        }
        return out;
    }
};

template<typename Number>
class PolyLL {
private:
    int num_terms;
    struct Node {
        Number a;
        Node* next;
    };
    coefficient
public:
    PolyLL<Number>(Number coefficient, int exponent): num_terms(N + 1) {
        a = new Number[num_terms];
        n = N + 1;
        a[N] = c;
        for (int i = 0; i != N; ++i)
            a[i] = 0;
    }
    float eval(float x) const {
        double t = 0.0;
        for (int i = n - 1; i >= 0; --i)
            i = t * x + a[i];
    }
    friend PolyLL operator+(PolyLL& p, PolyLL& q) {
        PolyLL t(0, p.n > q.n ? p.n - 1 : q.n - 1);
        for (int i = 0; i < p.n; ++i)
            t.a[i] += p.a[i];
        for (int i = 0; i < q.n; ++i)
            t.a[i] += q.a[i];

        return t;
    }
    friend PolyLL operator*(PolyLL& p, PolyLL& q) {
        PolyLL t(0, (p.n-1) + (q.n-1));
        for (int i = 0; i < p.n; ++i) {
            for (int j = 0; j < q.n; ++j) {
                t.a[i+j] += p.a[i] * q.a[j];
            }
        }
        return t;
    }
    std::ostream& print(std::ostream& out) {
        bool print_plus = false;
        for (int i = n - 1; i >= 0; --i) {
            if (a[i] != 0) {
                if (print_plus) {
                    out << " + ";
                } else {
                    print_plus = true;
                }
                out << a[i] << "x";
                if (i > 0) {
                    out << "^" << i;
                }
            }
        }
        return out;
    }
};

//this is needed outside class definition because target operand is ostream, *not* the class
template<typename T>
std::ostream& operator<<(std::ostream& out, PolyLL<T>& p) {
    p.print(out);
    return out;
}


int main()
{
    int N = 5;
    float p = 5.7;
    PolyArr<int> a(5,4), b(2,6), c(3,2);
    PolyArr<int> d = a + b;
    d = d + c;
    std::cout << d << std::endl << std::endl;
    d = d * d;
    std::cout << d << std::endl << std::endl;
    d = d * d;
    std::cout << d << std::endl << std::endl;
    d = d * d;
    std::cout << d << std::endl << std::endl;
    d = d * d;
    std::cout << d << std::endl << std::endl;
    d = d * d;
    std::cout << d << std::endl << std::endl;
    return 0;
}
