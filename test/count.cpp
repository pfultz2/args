#include <args.hpp>
#include "test.hpp"

struct count_cmd
{
    int verbose = 0;
    bool flag = false;

    template<class F>
    void parse(F f)
    {
        f(verbose, "-v", "--verbose", args::count());
        f(verbose, "-q", "--quiet", args::set(0));
        f(flag, "-f", "--flag", args::set(true));
    }

    void run()
    {}
};

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"--verbose", "--flag"});
    PROVE_CHECK(cmd.verbose == 1);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"-v", "-f"});
    PROVE_CHECK(cmd.verbose == 1);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"--verbose", "-f"});
    PROVE_CHECK(cmd.verbose == 1);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"-v", "--flag"});
    PROVE_CHECK(cmd.verbose == 1);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"-vv", "--flag"});
    PROVE_CHECK(cmd.verbose == 2);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"-vvv", "--flag"});
    PROVE_CHECK(cmd.verbose == 3);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"-v"});
    PROVE_CHECK(cmd.verbose == 1);
    PROVE_CHECK(cmd.flag == false);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"-vv"});
    PROVE_CHECK(cmd.verbose == 2);
    PROVE_CHECK(cmd.flag == false);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"-vvv"});
    PROVE_CHECK(cmd.verbose == 3);
    PROVE_CHECK(cmd.flag == false);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"-f"});
    PROVE_CHECK(cmd.verbose == 0);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"-vf"});
    PROVE_CHECK(cmd.verbose == 1);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"-vvf"});
    PROVE_CHECK(cmd.verbose == 2);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"-vvvf"});
    PROVE_CHECK(cmd.verbose == 3);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"-fvv"});
    PROVE_CHECK(cmd.verbose == 2);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"-fvvv"});
    PROVE_CHECK(cmd.verbose == 3);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"-vvfv"});
    PROVE_CHECK(cmd.verbose == 3);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    count_cmd cmd;
    args::parse(cmd, {"-qf"});
    PROVE_CHECK(cmd.verbose == 0);
    PROVE_CHECK(cmd.flag == true);
}


