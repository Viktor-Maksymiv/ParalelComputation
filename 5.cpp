#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <mutex>
#include <chrono>

using namespace std;

list<int> l;
mutex m;

void AddToList(int val) {
    lock_guard<mutex> lg(m);
    l.push_back(val);
    cout << "Додано " << val << endl;
}

void ListContains(int val) {
    lock_guard<mutex> lg(m);
    if (find(l.begin(), l.end(), val) != l.end()) {
        cout << "Входить" << endl;
    } else {
        cout << "Не входить" << endl;
    }
}

int main() {
    int x = 5;
    for (int i = 0; i < 10; i++) {
        thread t1(AddToList, x + i);
        thread t2(ListContains, x);
        t1.detach();
        t2.detach();
    }
    this_thread::sleep_for(chrono::seconds(1));
    return 0;
}