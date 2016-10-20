Args
====

Simple and typesafe commandline parsing for C++14. 

Quickstart
----------

Simply provide a class with fields that is to be filled by command-line arguments:

```cpp
struct hello
{
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

int main(int argc, char const *argv[]) {
    args::parse<hello>(argc, argv);
}
```

The command then could be run like this:

```
$ hello --name Paul --count 3
Paul
Paul
Paul

```

The args library will auto-generate help info:

```
$ hello --help
Usage: hello [options...]

  Simple program that greets NAME for a total of COUNT times.  

Options: 

            -h, --help Show help  
 --count, -C [integer] Number of greetings.  
   --name, -N [string] The person to greet.
```

In addition, nested commands can be created:

```cpp
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

```

So each subcommand can be ran like this:

```
$ cli initdb
Initialize database
```

```
$ cli dropdb
Delete database
```

In addition help is generated for subcommands as well:

```
$ cli --help
Usage: cli [options...] [command]

  Command-line interface to manage a database  

Options: 

 -h, --help Show help  

Commands: 

     dropdb Delete database  
     initdb Initialize database  

```
