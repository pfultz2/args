#include <args.hpp>
#include "test.hpp"

struct multi_cmd
{
    bool verbose = false;
    bool flag = false;

    template<class F>
    void parse(F f)
    {
        f(verbose, "-v", "--verbose", args::set(true));
        f(verbose, "-q", "--quiet", args::set(false));
        f(flag, "-f", "--flag", args::set(true));
    }

    void run()
    {}
};

PROVE_CASE()
{
    multi_cmd cmd;
    args::parse(cmd, {"--verbose", "--flag"});
    PROVE_CHECK(cmd.verbose == true);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    multi_cmd cmd;
    args::parse(cmd, {"-v", "-f"});
    PROVE_CHECK(cmd.verbose == true);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    multi_cmd cmd;
    args::parse(cmd, {"--verbose", "-f"});
    PROVE_CHECK(cmd.verbose == true);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    multi_cmd cmd;
    args::parse(cmd, {"-v", "--flag"});
    PROVE_CHECK(cmd.verbose == true);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    multi_cmd cmd;
    args::parse(cmd, {"-v"});
    PROVE_CHECK(cmd.verbose == true);
    PROVE_CHECK(cmd.flag == false);
}

PROVE_CASE()
{
    multi_cmd cmd;
    args::parse(cmd, {"-f"});
    PROVE_CHECK(cmd.verbose == false);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    multi_cmd cmd;
    args::parse(cmd, {"-vf"});
    PROVE_CHECK(cmd.verbose == true);
    PROVE_CHECK(cmd.flag == true);
}

PROVE_CASE()
{
    multi_cmd cmd;
    args::parse(cmd, {"-qf"});
    PROVE_CHECK(cmd.verbose == false);
    PROVE_CHECK(cmd.flag == true);
}
