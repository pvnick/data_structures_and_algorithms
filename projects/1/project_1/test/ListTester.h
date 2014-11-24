#ifndef _LISTTESTER_H
#define _LISTTESTER_H

namespace cop3530 {
    //the following class allows us to access protected class members in the underlying lists
    template <typename List>
    class ListTester: public List {
    public:
        ListTester() = default;
        ListTester(const List& other): List(other) {}
        bool validate_internal_integrity() {
            return List::validate_internal_integrity();
        }
    };
}

#endif
