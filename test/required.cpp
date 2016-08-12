#include <args.hpp>
#include "test.hpp"

struct single_value_cmd
{
    int count;
    std::string name;

    template<class F>
    void parse(F f)
    {
        f(count, "--count", "-C", args::required());
        f(name, "--name", "-N", args::required());
    }

    void run()
    {}
};

PROVE_CASE()
{
    single_value_cmd cmd;
    args::parse(cmd, {"--count", "5", "--name", "hello"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.name == "hello");
}

PROVE_CASE()
{
    single_value_cmd cmd;
    PROVE_THROWS(args::parse(cmd, {"--count", "5"}));
    PROVE_CHECK(cmd.count == 5);
}

PROVE_CASE()
{
    single_value_cmd cmd{};
    PROVE_THROWS(args::parse(cmd, {"--name", "hello"}));
    PROVE_CHECK(cmd.name == "hello");
    PROVE_CHECK(cmd.count == 0);
}

PROVE_CASE()
{
    single_value_cmd cmd{};
    PROVE_THROWS(args::parse(cmd, {}));
    PROVE_CHECK(cmd.count == 0);
}
