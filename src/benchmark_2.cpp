#include <stdio.h>
#include <chrono>
#include <time.h>
#include <iostream>
#include <boost/thread.hpp>

using namespace std;

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;

#ifdef FALSE_SHARING

struct xy
{
    unsigned int x;
    unsigned int y;
};

#else

__declspec(align(64))struct xy
{
    unsigned int x;
    unsigned int y;
};

#endif

int thread1(xy& foo) {
    for (int i = 0; i < 100000; ++i)
        for (int j = 0; j < 100000; ++j) {
            foo.x++;
            foo.y++;
        }
    return 0;
}

int thread2(xy& foo) {
    for (int i = 0; i < 100000; ++i)
        for (int j = 0; j < 100000; ++j) {
            foo.x++;
            foo.y++;
        }
    return 0;
}

int main(void) {

#ifdef FALSE_SHARING
    std::cout << "INFO: processing false sharing example..." << std::endl;
#else
    std::cout << "INFO: processing fixed version" << std::endl;
#endif

    auto t0 = Time::now();

    boost::thread_group tg;

    xy foo[2];
    foo[0] = { 0, 0 };
    foo[1] = { 0, 0 };

    tg.create_thread([&foo]() {
        thread1(foo[0]);
        });

    tg.create_thread([&foo]() {
        thread2(foo[1]);
        });

    tg.join_all();

    auto t1 = Time::now();

    fsec fs = t1 - t0;
    ms d = std::chrono::duration_cast<ms>(fs);

    std::cout << "INFO: trace time " << d.count() << " ms" << std::endl;
    return 0;
}
