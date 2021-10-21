#include <my_assert.h>

void __M_Assert(char const* expr_str, bool expr, char const* file, int line, char const* msg)
{
    if (!expr)
    {
        std::cerr << "Assert failed:\t" << msg << "\n"
            << "Expected:\t" << expr_str << "\n"
            << "Source:\t\t" << file << ", line " << line << "\n";
        abort();
    }
}
