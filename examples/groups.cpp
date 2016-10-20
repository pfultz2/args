#include <args.hpp>

struct cli : args::group<cli>
{};

struct initdb : cli::command<initdb>
{
    initdb() {}
    static const char* help()
    {
        return "Initialize database";
    }
    void run()
    {
        printf("Initialize database\n");
    }
};

struct dropdb : cli::command<dropdb>
{
    dropdb() {}
    static const char* help()
    {
        return "Delete database";
    }
    void run()
    {
        printf("Delete database\n");
    }
};

int main(int argc, char const *argv[]) {
    args::parse<cli>(argc, argv);
}

