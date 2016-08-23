#include <args.hpp>
#include "test.hpp"

struct cli : args::group<cli>
{
    std::string name;
};

struct init : cli::command<init>
{
    init() {}
    void run(cli& c)
    {
        c.name = "init";
    }
};

struct delete_ : cli::command<delete_>
{
    delete_() {}
    void run()
    {
        
    }
};

struct custom_name : cli::command<custom_name>
{
    static const char* name() { return "run"; }
    custom_name() {}
    void run(cli& c)
    {
        c.name = "run";
    }
};

PROVE_CASE()
{
    cli cmd;
    args::parse(cmd, {"init"});
    PROVE_CHECK(cmd.name == "init");
}

PROVE_CASE()
{
    cli cmd;
    args::parse(cmd, {"run"});
    PROVE_CHECK(cmd.name == "run");
}

PROVE_CASE()
{
    cli cmd;
    args::parse(cmd, {"delete"});
}
