#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <chrono>
#include <cmath>
#include <queue>
#include <deque>
#include <vector>
#include <cstdlib>

using namespace std;

// === Завдання 2.1 ===
namespace Task_2_1 {
    mutex m;
    queue<int> numbers;
    bool ready = false;

    bool isPrime(int n) {
        if (n < 2)
            return false;
        for (int i = 2; i * i <= n; i++)
            if (n % i == 0)
                return false;
        return true;
    }

    void DataPreparation() {
        cout << "Введіть цілі числа (для завершення введіть будь-який нецифровий символ):" << endl;
        int x;
        while (cin >> x) {
            lock_guard<mutex> lk(m);
            numbers.push(x);
        }
        cin.clear();
        string dummy;
        cin >> dummy;

        lock_guard<mutex> lk(m);
        ready = true;
    }

    void DataProcessing() {
        unique_lock<mutex> lk(m);
        while (!ready) {
            lk.unlock();
            this_thread::sleep_for(chrono::milliseconds(100));
            lk.lock();
        }
        cout << "Прості числа:" << endl;
        while (!numbers.empty()) {
            int x = numbers.front();
            numbers.pop();
            if (isPrime(x))
                cout << x << " ";
        }
        cout << endl;
    }

    int run() {
        ready = false;
        while (!numbers.empty()) numbers.pop();

        thread preparation(DataPreparation);
        preparation.detach();
        thread processing(DataProcessing);
        processing.join();
        return 0;
    }
}

// === Завдання 2.2 ===
namespace Task_2_2 {
    mutex m;
    condition_variable cv;
    int i = 0;

    void Waits(int id) {
        unique_lock<mutex> lk(m);
        cout << "Потік " << id << " входить у стан очікування" << endl;
        cv.wait(lk, [] { return i == 1; });
        cout << "Очікування потоку " << id << " завершене" << endl;
    }

    void Awake() {
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

    int run() {
        i = 0;
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
}

// === Завдання 2.3 ===
namespace Task_2_3 {
    mutex m;
    condition_variable cv;
    int i = 0;

    void Waiter(int id) {
        unique_lock<mutex> lk(m);
        cv.wait(lk, [] { return i == 1; });
        cout << "Повідомлення з потоку " << id << endl;
    }

    void Notify() {
        this_thread::sleep_for(chrono::milliseconds(100));
        {
            lock_guard<mutex> lk(m);
            i = 1;
        }
        cv.notify_one();
    }

    int run() {
        i = 0;
        thread thread1(Waiter, 1);
        thread thread2(Waiter, 2);
        thread thread3(Waiter, 3);
        thread1.detach();
        thread2.detach();
        thread3.detach();
        thread notify(Notify);
        notify.join();
        this_thread::sleep_for(chrono::milliseconds(200));
        return 0;
    }
}

// === Завдання 2.4 ===
namespace Task_2_4 {
    mutex m;
    condition_variable cv;
    queue<int> numbers;

    bool isPrime(int n) {
        if (n < 2)
            return false;
        for (int i = 2; i * i <= n; i++)
            if (n % i == 0)
                return false;
        return true;
    }

    void DataPreparation() {
        cout << "Введіть цілі числа (для завершення введіть будь-який нецифровий символ):" << endl;
        int x;
        while (cin >> x) {
            lock_guard<mutex> lk(m);
            numbers.push(x);
        }
        cin.clear();
        string dummy;
        cin >> dummy;
        cv.notify_one();
    }

    void DataProcessing() {
        unique_lock<mutex> lk(m);
        cv.wait(lk, [] { return !numbers.empty(); });
        cout << "Прості числа:" << endl;
        while (!numbers.empty()) {
            int x = numbers.front();
            numbers.pop();
            if (isPrime(x))
                cout << x << " ";
        }
        cout << endl;
    }

    int run() {
        while (!numbers.empty()) numbers.pop();

        thread preparation(DataPreparation);
        preparation.detach();
        thread processing(DataProcessing);
        processing.join();
        return 0;
    }
}

// === Завдання 2.5 ===
namespace Task_2_5 {
    long long nthPrime(int n) {
        int count = 0;
        long long candidate = 1;
        while (count < n) {
            candidate++;
            bool prime = true;
            for (long long d = 2; d * d <= candidate; d++) {
                if (candidate % d == 0) {
                    prime = false;
                    break;
                }
            }
            if (prime)
                count++;
        }
        return candidate;
    }

    int run() {
        int mode, n, choice;
        cout << "Режим запуску (1 - deferred, 2 - async): ";
        cin >> mode;
        cout << "Введіть номер простого числа n: ";
        cin >> n;

        launch policy = (mode == 1) ? launch::deferred : launch::async;
        future<long long> f = async(policy, nthPrime, n);

        cout << "Оберіть функцію від n (1 - корінь квадратний, 2 - синус, 3 - натуральний логарифм): ";
        cin >> choice;
        if (choice == 1)
            cout << "sqrt(" << n << ") = " << sqrt(n) << endl;
        else if (choice == 2)
            cout << "sin(" << n << ") = " << sin(n) << endl;
        else
            cout << "ln(" << n << ") = " << log(n) << endl;

        auto start = chrono::steady_clock::now();
        long long result = f.get();
        auto end = chrono::steady_clock::now();

        cout << n << "-те просте число: " << result << endl;
        cout << "Час очікування результату: "
             << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " мс" << endl;
        return 0;
    }
}

// === Завдання 2.6 ===
namespace Task_2_6 {
    mutex m;
    condition_variable cv;
    deque<packaged_task<long long(int)>> tasks;
    deque<int> numbers;
    bool finished = false;

    long long nthPrime(int n) {
        int count = 0;
        long long candidate = 1;
        while (count < n) {
            candidate++;
            bool prime = true;
            for (long long d = 2; d * d <= candidate; d++) {
                if (candidate % d == 0) {
                    prime = false;
                    break;
                }
            }
            if (prime)
                count++;
        }
        return candidate;
    }

    void Worker() {
        while (true) {
            unique_lock<mutex> lk(m);
            cv.wait(lk, [] { return !tasks.empty() || finished; });
            if (tasks.empty())
                return;
            packaged_task<long long(int)> task = move(tasks.front());
            int n = numbers.front();
            tasks.pop_front();
            numbers.pop_front();
            lk.unlock();
            task(n);
        }
    }

    int run() {
        finished = false;
        tasks.clear();
        numbers.clear();

        thread worker(Worker);
        vector<int> requested;
        vector<future<long long>> results;
        int n;
        cout << "Введіть n (для завершення введіть будь-який нецифровий символ): ";
        while (true) {
            if (!(cin >> n) || n < 1)
                break;
            packaged_task<long long(int)> task(nthPrime);
            results.push_back(task.get_future());
            requested.push_back(n);
            {
                lock_guard<mutex> lk(m);
                tasks.push_back(move(task));
                numbers.push_back(n);
            }
            cv.notify_one();
        }
        cin.clear();
        string dummy;
        cin >> dummy;

        {
            lock_guard<mutex> lk(m);
            finished = true;
        }
        cv.notify_one();

        for (size_t i = 0; i < results.size(); i++)
            cout << requested[i] << "-те просте число: " << results[i].get() << endl;

        worker.join();
        return 0;
    }
}

// === Завдання 2.7 ===
namespace Task_2_7 {
    long long nthPrime(int n) {
        int count = 0;
        long long candidate = 1;
        while (count < n) {
            candidate++;
            bool prime = true;
            for (long long d = 2; d * d <= candidate; d++) {
                if (candidate % d == 0) {
                    prime = false;
                    break;
                }
            }
            if (prime)
                count++;
        }
        return candidate;
    }

    void Producer(int n, promise<long long> first, promise<bool> started, promise<long long> second) {
        first.set_value(nthPrime(n));
        started.set_value(true);
        second.set_value(nthPrime(n * 10));
    }

    void Consumer(int n, future<bool> started) {
        started.get();
        this_thread::sleep_for(chrono::seconds(2));
        cout << "sqrt(" << n << ") = " << sqrt(n) << endl;
    }

    int run() {
        int n;
        cout << "Введіть n: ";
        cin >> n;

        promise<long long> firstPromise;
        promise<bool> startedPromise;
        promise<long long> secondPromise;
        future<long long> firstFuture = firstPromise.get_future();
        future<bool> startedFuture = startedPromise.get_future();
        future<long long> secondFuture = secondPromise.get_future();

        thread producer(Producer, n, move(firstPromise), move(startedPromise), move(secondPromise));
        thread consumer(Consumer, n, move(startedFuture));

        cout << n << "-те просте число: " << firstFuture.get() << endl;
        cout << n * 10 << "-те просте число: " << secondFuture.get() << endl;

        producer.join();
        consumer.join();
        return 0;
    }
}

int main() {
    // Розкоментуйте потрібне завдання для запуску:

    // Task_2_1::run();
    // Task_2_2::run();
    // Task_2_3::run();
    // Task_2_4::run();
    // Task_2_5::run();
    // Task_2_6::run();
    // Task_2_7::run();

    return 0;
}