#include <args.hpp>
#include "test.hpp"

struct set_cmd
{
    int count;
    std::string name;

    template<class F>
    void parse(F f)
    {
        f(count, "--single", "-S", args::set(1));
        f(count, "--double", "-D", args::set(2));
        f(count, "--triple", "-T", args::set(3));
        f(name, "--name", "-N");
    }

    void run()
    {}
};

PROVE_CASE()
{
    set_cmd cmd;
    args::parse(cmd, {"--single", "--name", "hello"});
    PROVE_CHECK(cmd.count == 1);
    PROVE_CHECK(cmd.name == "hello");
}

PROVE_CASE()
{
    set_cmd cmd;
    args::parse(cmd, {"--double", "--name", "hello"});
    PROVE_CHECK(cmd.count == 2);
    PROVE_CHECK(cmd.name == "hello");
}

PROVE_CASE()
{
    set_cmd cmd;
    args::parse(cmd, {"--triple", "--name", "hello"});
    PROVE_CHECK(cmd.count == 3);
    PROVE_CHECK(cmd.name == "hello");
}

PROVE_CASE()
{
    set_cmd cmd;
    args::parse(cmd, {"-S", "--name", "hello"});
    PROVE_CHECK(cmd.count == 1);
    PROVE_CHECK(cmd.name == "hello");
}

PROVE_CASE()
{
    set_cmd cmd;
    args::parse(cmd, {"-D", "--name", "hello"});
    PROVE_CHECK(cmd.count == 2);
    PROVE_CHECK(cmd.name == "hello");
}

PROVE_CASE()
{
    set_cmd cmd;
    args::parse(cmd, {"-T", "--name", "hello"});
    PROVE_CHECK(cmd.count == 3);
    PROVE_CHECK(cmd.name == "hello");
}
