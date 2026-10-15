#include <iostream>
#include <thread>
#include <mutex>
#include <string>
using namespace std;

class someData {
public:
    string name;
    string surname;
    string address;
    int age = 0;

    void print() {
        cout << name << " " << surname << ", "
            << address << ", " << age << endl;
    }
};

class exchangePerson {
public:
    someData data;
    mutex m;

    static void JohnDoe(exchangePerson& p) {
        lock_guard<mutex> lock(p.m);

        p.data.name = "John";
        p.data.surname = "Doe";
        p.data.address = "Unknown";
        p.data.age = 120;
    }

    static void JacobSmith(exchangePerson& p) {
        lock_guard<mutex> lock(p.m);

        p.data.name = "Jacob";
        p.data.surname = "Smith";
        p.data.address = "Known";
        p.data.age = 1;
    }

    static void Swap(exchangePerson& a, exchangePerson& b) {
        if (&a == &b) return;

        unique_lock<mutex> lockA(a.m, defer_lock);
        unique_lock<mutex> lockB(b.m, defer_lock);

        std::lock(lockA, lockB);

        cout << "До обміну:" << endl;
        cout << "Об'єкт 1: ";
        a.data.print();
        cout << "Об'єкт 2: ";
        b.data.print();

        swap(a.data, b.data);

        cout << "Після обміну:" << endl;
        cout << "Об'єкт 1: ";
        a.data.print();
        cout << "Об'єкт 2: ";
        b.data.print();
    }
};

int main() {

    exchangePerson p1, p2;

    thread t1(exchangePerson::JohnDoe, ref(p1));
    thread t2(exchangePerson::JacobSmith, ref(p2));

    t1.detach();
    t2.detach();

    thread t3(exchangePerson::Swap, ref(p1), ref(p2));
    t3.join();

    return 0;
}