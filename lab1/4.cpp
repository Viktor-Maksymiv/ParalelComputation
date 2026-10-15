#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <mutex>

using namespace std;

list<int> l;
mutex m;

void AddToList(int val) {
    for (int i = 0; i < 10; i++) {
        m.lock();
        l.push_back(val + i);
        cout << "Додано " << val + i << endl;
        m.unlock();
    }
}

void ListContains(int val) {
    for (int i = 0; i < 10; i++) {
        m.lock();
        if (find(l.begin(), l.end(), val) != l.end()) {
            cout << "Входить" << endl;
        } else {
            cout << "Не входить" << endl;
        }
        m.unlock();
    }
}

int main() {
    int x = 5;
    thread t1(AddToList, x);
    thread t2(ListContains, x);
    t1.join();
    t2.join();
    return 0;
}