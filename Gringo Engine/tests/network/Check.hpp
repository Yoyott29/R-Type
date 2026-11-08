#pragma once

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

namespace check
{
struct Failure : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct Test {
    std::string name;
    std::function<void()> run;
};

inline std::vector<Test> &tests()
{
    static std::vector<Test> list;
    return list;
}
} // namespace check

#define CHECK_STR_(x) #x
#define CHECK_STR(x) CHECK_STR_(x)
#define CHECK_WHERE __FILE__ ":" CHECK_STR(__LINE__) ": "

#define TEST_CASE(name)                                                                            \
    static void name();                                                                            \
    static const bool name##_registered = (check::tests().push_back({#name, &name}), true);        \
    static void name()

#define CHECK(cond)                                                                                \
    do {                                                                                           \
        if (!(cond))                                                                               \
            throw check::Failure(CHECK_WHERE #cond);                                               \
    } while (false)

#define CHECK_THROWS(expr, Type)                                                                   \
    do {                                                                                           \
        try {                                                                                      \
            expr;                                                                                  \
        } catch (const Type &) {                                                                   \
            break;                                                                                 \
        }                                                                                          \
        throw check::Failure(CHECK_WHERE #expr " did not throw " #Type);                           \
    } while (false)
