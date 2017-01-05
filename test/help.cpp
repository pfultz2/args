#include <args.hpp>
#include "test.hpp"

struct help_cmd
{
    int count;
    std::string name;

    template<class F>
    void parse(F f)
    {
        f(count, "--count", "-C", args::help("The number of greetings"));
        f(name, "--name", "-N", args::help("The greeting name"));
    }

    void run()
    {}
};

PROVE_CASE()
{
    help_cmd cmd{};
    args::parse(cmd, {"--count", "5", "--name", "hello"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.name == "hello");
}

PROVE_CASE()
{
    help_cmd cmd{};
    args::parse(cmd, {"--help"});
    PROVE_CHECK(cmd.count == 0);
    PROVE_CHECK(cmd.name == "");
}

PROVE_CASE()
{
    help_cmd cmd{};
    args::parse(cmd, {"--help", "--count", "5", "--name", "hello"});
    PROVE_CHECK(cmd.count == 0);
    PROVE_CHECK(cmd.name == "");
}
