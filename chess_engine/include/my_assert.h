#ifndef MY_ASSERT_H_2323
#define MY_ASSERT_H_2323

// From: https://stackoverflow.com/questions/3692954/add-custom-messages-in-assert
#ifndef NDEBUG
#define MY_ASSERT(Expr, Msg) __M_Assert(#Expr, Expr, __FILE__, __LINE__, Msg)
#else
#define MY_ASSERT(Expr, Msg) ;
#endif

constexpr void __M_Assert(char const* expr_str, bool expr, char const* file, int line, char const* msg)
{
  if (!expr)
  {
    std::cerr << "Assert failed:\t" << msg << "\n"
              << "Expected:\t" << expr_str << "\n"
              << "Source:\t\t" << file << ", line " << line << "\n";
    abort();
  }
}

#endif // MY_ASSERT_H_2323
