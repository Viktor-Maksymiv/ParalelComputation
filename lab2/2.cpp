#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
using namespace std;

mutex m;
condition_variable cv;
int i = 0;

void Waits(int id)
{
    unique_lock<mutex> lk(m);
    cout << "Потік " << id << " входить у стан очікування" << endl;
    cv.wait(lk, [] { return i == 1; });
    cout << "Очікування потоку " << id << " завершене" << endl;
}

void Awake()
{
    this_thread::sleep_for(chrono::milliseconds(100));
    {
        lock_guard<mutex> lk(m);
        cout << "Awake: починаю повідомляти решту" << endl;
    }
    cv.notify_all();
    this_thread::sleep_for(chrono::milliseconds(100));
    {
        lock_guard<mutex> lk(m);
        i = 1;
        cout << "Awake: повторне повідомлення" << endl;
    }
    cv.notify_all();
}

int main()
{
    thread w1(Waits, 1);
    thread w2(Waits, 2);
    thread w3(Waits, 3);
    thread awake(Awake);
    awake.join();
    w1.join();
    w2.join();
    w3.join();
    return 0;
}
