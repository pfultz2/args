#include <args.hpp>
#include "test.hpp"

struct single_value_cmd
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
    single_value_cmd cmd;
    args::parse(cmd, {"--count", "5", "--name", "hello"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.name == "hello");
}

PROVE_CASE()
{
    single_value_cmd cmd;
    args::parse(cmd, {"--count", "5"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.name == "");
}

PROVE_CASE()
{
    single_value_cmd cmd;
    args::parse(cmd, {"--count=5", "--name=hello"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.name == "hello");
}

PROVE_CASE()
{
    single_value_cmd cmd;
    args::parse(cmd, {"-C", "5", "-N", "hello"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.name == "hello");
}

PROVE_CASE()
{
    single_value_cmd cmd;
    args::parse(cmd, {"-C5", "-Nhello"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.name == "hello");
}

struct multi_value_cmd
{
    int count;
    std::vector<std::string> names;

    template<class F>
    void parse(F f)
    {
        f(count, "--count", "-C");
        f(names, "--name", "-N");
    }

    void run()
    {}
};

PROVE_CASE()
{
    multi_value_cmd cmd;
    args::parse(cmd, {"-N", "1", "-N", "2", "-N", "3", "--count", "5"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.names[0] == "1");
    PROVE_CHECK(cmd.names[1] == "2");
    PROVE_CHECK(cmd.names[2] == "3");
}

PROVE_CASE()
{
    multi_value_cmd cmd;
    args::parse(cmd, {"-N", "1", "2", "3", "--count", "5"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.names[0] == "1");
    PROVE_CHECK(cmd.names[1] == "2");
    PROVE_CHECK(cmd.names[2] == "3");
}

PROVE_CASE()
{
    multi_value_cmd cmd;
    args::parse(cmd, {"-N1", "-N2", "-N3", "--count", "5"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.names[0] == "1");
    PROVE_CHECK(cmd.names[1] == "2");
    PROVE_CHECK(cmd.names[2] == "3");
}

PROVE_CASE()
{
    multi_value_cmd cmd;
    args::parse(cmd, {"--name", "1", "--name", "2", "--name", "3", "--count", "5"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.names[0] == "1");
    PROVE_CHECK(cmd.names[1] == "2");
    PROVE_CHECK(cmd.names[2] == "3");
}

PROVE_CASE()
{
    multi_value_cmd cmd;
    args::parse(cmd, {"--name", "1", "2", "3", "--count", "5"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.names[0] == "1");
    PROVE_CHECK(cmd.names[1] == "2");
    PROVE_CHECK(cmd.names[2] == "3");
}

PROVE_CASE()
{
    multi_value_cmd cmd;
    args::parse(cmd, {"--name=1", "--name=2", "--name=3", "--count", "5"});
    PROVE_CHECK(cmd.count == 5);
    PROVE_CHECK(cmd.names[0] == "1");
    PROVE_CHECK(cmd.names[1] == "2");
    PROVE_CHECK(cmd.names[2] == "3");
}

struct no_data
{
    std::string name;

    template<class F>
    void parse(F f)
    {
        f(nullptr, "--null");
        f(name, "--name", "-N");
    }

    void run()
    {}
};

PROVE_CASE()
{
    no_data cmd;
    args::parse(cmd, {"--null", "--name=hello"});
    PROVE_CHECK(cmd.name == "hello");
}
