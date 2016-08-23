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

PROVE_CASE()
{
    cli cmd1;
    args::parse(cmd1, {"init"});
    PROVE_CHECK(cmd1.name == "init");

    cli cmd2;
    args::parse(cmd2, {"delete"});
}
