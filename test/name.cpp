#include <args.hpp>
#include "test.hpp"

struct foo {};
struct foo_ {};
template<class T>
struct foo_template {};

PROVE_CASE()
{
    PROVE_CHECK(args::get_name<foo>() == "foo");
    PROVE_CHECK(args::get_name<foo_>() == "foo");
    PROVE_CHECK(args::get_name<foo_>() != "foo_");
    PROVE_CHECK(args::get_name<foo_template<int>>() == "foo_template<int>");
}

namespace bar {

struct foo {};

struct foo_ {};

template<class T>
struct foo_template {};

struct custom_name
{
    static const char* name() { return "foo"; }
};

}

PROVE_CASE()
{
    PROVE_CHECK(args::get_name<bar::foo>() == "foo");
    PROVE_CHECK(args::get_name<bar::foo_>() == "foo");
    PROVE_CHECK(args::get_name<bar::foo_>() != "foo_");
    PROVE_CHECK(args::get_name<bar::foo_template<int>>() == "foo_template<int>");
    PROVE_CHECK(args::get_name<bar::custom_name>() == "foo");
}
