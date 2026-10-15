#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <mutex>
#include <chrono>
#include <string>

using namespace std;

// === Завдання 1.2.1 ===
namespace Task_1_2_1 {
    void Thread1() {
        cout << 1 << endl;
    }

    void Thread2() {
        cout << 2 << endl;
    }

    int run() {
        thread t1(Thread1);
        thread t2(Thread2);
        return 0;
    }
}

// === Завдання 1.2.2 ===
namespace Task_1_2_2 {
    void Thread1() {
        cout << 1 << endl;
    }

    void Thread2() {
        cout << 2 << endl;
    }

    int run() {
        thread t1(Thread1);
        thread t2(Thread2);
        t1.detach();
        t2.detach();
        return 0;
    }
}

// === Завдання 1.2.3 ===
namespace Task_1_2_3 {
    list<int> l;

    void AddToList(int val) {
        for (int i = 0; i < 10; i++) {
            l.push_back(val + i);
            cout << "Додано " << val + i << endl;
        }
    }

    void ListContains(int val) {
        for (int i = 0; i < 10; i++) {
            if (find(l.begin(), l.end(), val) != l.end()) {
                cout << "Входить" << endl;
            } else {
                cout << "Не входить" << endl;
            }
        }
    }

    int run() {
        int x = 5;
        thread t1(AddToList, x);
        thread t2(ListContains, x);
        t1.join();
        t2.join();
        return 0;
    }
}

// === Завдання 1.2.4 ===
namespace Task_1_2_4 {
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

    int run() {
        int x = 5;
        thread t1(AddToList, x);
        thread t2(ListContains, x);
        t1.join();
        t2.join();
        return 0;
    }
}

// === Завдання 1.2.5 ===
namespace Task_1_2_5 {
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

    int run() {
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
}

// === Завдання 1.2.6 ===
namespace Task_1_2_6 {
    class someData {
    public:
        string name;       
        string surname;    
        string address;    
        int age;           

        void print() {
            cout << name << " " << surname << ", " << address << ", " << age << endl;
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

            lock(a.m, b.m);
            lock_guard<mutex> lockA(a.m, adopt_lock);
            lock_guard<mutex> lockB(b.m, adopt_lock);

            cout << "До обміну:" << endl;
            cout << "Об'єкт 1: "; a.data.print();
            cout << "Об'єкт 2: "; b.data.print();

            swap(a.data, b.data);

            cout << "Після обміну:" << endl;
            cout << "Об'єкт 1: "; a.data.print();
            cout << "Об'єкт 2: "; b.data.print();
        }
    };

    int run() {
        exchangePerson p1, p2;

        thread t1(exchangePerson::JohnDoe, ref(p1));
        thread t2(exchangePerson::JacobSmith, ref(p2));
        t1.detach();
        t2.detach();

        thread t3(exchangePerson::Swap, ref(p1), ref(p2));
        t3.join();  
        return 0;
    }
}

// === Завдання 1.2.7 ===
namespace Task_1_2_7 {
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

    int run() {
        exchangePerson p1, p2;

        thread t1(exchangePerson::JohnDoe, ref(p1));
        thread t2(exchangePerson::JacobSmith, ref(p2));

        t1.detach();
        t2.detach();

        thread t3(exchangePerson::Swap, ref(p1), ref(p2));
        t3.join();

        return 0;
    }
}

int main() {

    // Task_1_2_1::run();
    // Task_1_2_2::run();
    // Task_1_2_3::run();
    // Task_1_2_4::run();
    // Task_1_2_5::run();
    // Task_1_2_6::run();
    // Task_1_2_7::run();

    return 0;
}