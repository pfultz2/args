#include <args.hpp>

struct hello
{
    static const char* help()
    {
        return "Simple program that greets NAME for a total of COUNT times.";
    }
    int count;
    std::string name;

    hello() : count(1)
    {}

    template<class F>
    void parse(F f)
    {
        f(count, "--count", "-C", args::help("Number of greetings."));
        f(name, "--name", "-N", args::help("The person to greet."), args::required());
    }

    void run()
    {
        for(int i=0;i<count;i++) printf("%s\n", name.c_str());
    }
};

int main(int argc, char const *argv[]) 
{
    args::parse<hello>(argc, argv);
}

