#include <args.hpp>
#include "test.hpp"

struct hello
{
    int count;
    std::string name;

    template<class F>
    void parse(F f)
    {
        f(count, "--count", "-C");
        f(name, "--name", "-N");
    }

    void run()
    {}
};

PROVE_CASE()
{
    hello cmd;
    args::parse(cmd, {"--count", "5", "--name", "hello"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.name == "hello");
}

PROVE_CASE()
{
    hello cmd;
    args::parse(cmd, {"--count=5", "--name=hello"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.name == "hello");
}

PROVE_CASE()
{
    hello cmd;
    args::parse(cmd, {"-C", "5", "-N", "hello"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.name == "hello");
}

PROVE_CASE()
{
    hello cmd;
    args::parse(cmd, {"-C5", "-Nhello"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.name == "hello");
}
