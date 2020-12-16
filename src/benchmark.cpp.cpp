#include <stdio.h>
#include <iostream>
#include <chrono>
#include <time.h>
#include <iostream>
#include <boost/thread.hpp>

using namespace std;

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;

#ifdef FALSE_SHARING
    
struct
{
    unsigned int x;
    unsigned int y;
} foo = { 0, 0 };

#else

struct
{
    unsigned int x;
    char make_structure_look_big[60];
    unsigned int y;
} foo = { 0, {0}, 0 };

#endif

int main(void) {
        auto t0 = Time::now();

        boost::thread_group tg;

        tg.create_thread([]() {
            for (int i = 0; i < 100000; ++i)
                for (int j = 0; j < 100000; ++j)
                    foo.x++;
            });
        tg.create_thread([]() {
            for (int k = 0; k < 100000; ++k)
                for (int m = 0; m < 100000; ++m)
                    foo.y++;
            });
        tg.join_all();
        std::cout << foo.x << ' ' << foo.y << std::endl;
        auto t1 = Time::now();
        fsec fs = t1 - t0;
        ms d = std::chrono::duration_cast<ms>(fs);
        std::cout << "Trace time " << d.count() << "ms\n";
        return 0;
}
