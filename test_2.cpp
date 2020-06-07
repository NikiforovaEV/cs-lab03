#include "svg_histogram.h"
#include <cassert>

void test_1()
{
    double max_count = 0;
    poisk_max_count({1, 2, 3}, max_count);
    assert(max_count == 3);
}
void test_2()
{
    double max_count = 0;
    poisk_max_count({3, 3, 3}, max_count);
    assert(max_count == 3);
}

int main()
{
    test_1();
    test_2();
}
