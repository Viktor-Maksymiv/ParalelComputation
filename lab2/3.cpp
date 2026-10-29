#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstdlib>
using namespace std;

mutex m;
condition_variable cv;
int i = 0;

void Waiter(int id)
{
    unique_lock<mutex> lk(m);
    cv.wait(lk, [] { return i == 1; });
    cout << "Повідомлення з потоку " << id << endl;
}

void Notify()
{
    this_thread::sleep_for(chrono::milliseconds(100));
    {
        lock_guard<mutex> lk(m);
        i = 1;
    }
    cv.notify_one();
}

int main()
{
    thread thread1(Waiter, 1);
    thread thread2(Waiter, 2);
    thread thread3(Waiter, 3);
    thread1.detach();
    thread2.detach();
    thread3.detach();
    thread notify(Notify);
    notify.join();
    this_thread::sleep_for(chrono::milliseconds(100));
    quick_exit(0);
}
