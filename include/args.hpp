/*=============================================================================
    Copyright (c) 2016 Paul Fultz II
    args.hpp
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef ARGS_GUARD_ARGS_HPP
#define ARGS_GUARD_ARGS_HPP

#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <initializer_list>
#include <functional>

#include <cassert>

// #include <iostream>

namespace args {

template<int N>
struct rank : rank<N-1> {};

template<>
struct rank<0> {};

}

namespace adl_args {

using std::begin;
using std::end;

template<class T>
auto is_container(args::rank<1>, T&& x) -> decltype(
    x.insert(--end(x), *begin(x)), std::true_type{}
)
{
    return {};
}

template<class T>
std::false_type is_container(args::rank<0>, T&&)
{
    return {};
}

}

namespace args {

template<class T>
struct is_container
: decltype(adl_args::is_container(args::rank<1>{}, std::declval<T>()))
{};

template<class F1, class... Fs>
struct overload_set : F1, overload_set<Fs...>
{
    using F1::operator();
    using overload_set<Fs...>::operator();
    overload_set(F1 f1, Fs... fs) : F1(f1), overload_set<Fs...>(fs...) 
    {}
};

template<class F1>
struct overload_set<F1> : F1
{
    using F1::operator();
    overload_set(F1 f1) : F1(f1) 
    {}
};

template<class... Fs>
overload_set<Fs...> overload(Fs... fs) 
{
    return {fs...};
}

template<class F, class... Ts>
void each_arg(F f, Ts&&... xs)
{
    (void)std::initializer_list<int>{((void)f(std::forward<Ts>(xs)), 0)...};     
}

template<class T>
struct value_parser
{
    static T apply(const std::string& x)
    {
        T result;
        std::stringstream ss;
        ss.str(x);
        ss >> result;
        return result;
    }
};

template<class T, typename std::enable_if<(not is_container<T>{} or std::is_convertible<T, std::string>{}), int>::type = 0>
void write_value_to(T& result, const std::string& x)
{
    result = value_parser<T>::apply(x);
}

template<class T, typename std::enable_if<(is_container<T>{} and not std::is_convertible<T, std::string>{}), int>::type = 0>
void write_value_to(T& result, const std::string& x)
{
    result.insert(--result.end(), value_parser<typename T::value_type>::apply(x));
}

enum class argument_type
{
    none,
    single,
    multiple
};

template<class T>
argument_type get_argument_type(T& x)
{
    if (std::is_same<T, bool>{}) return argument_type::none;
    else if (is_container<T>{}) return argument_type::multiple;
    else return argument_type::single;
}

struct argument
{
    argument_type type;
    std::vector<std::string> flags;

    std::function<void(const std::string&)> write_value;
    std::unordered_map<std::string, std::string> data;

};

struct context
{
    std::vector<argument> arguments;
    std::unordered_map<std::string, int> lookup;

    std::unordered_multimap<int, std::string> raw_options;
    std::vector<std::string> raw_arguments;

    void add(argument arg)
    {
        for(auto&& name:arg.flags) lookup[name] = arguments.size();
        arguments.emplace_back(std::move(arg));
    }

    void add_raw_option(const std::string& name, const std::string& value)
    {
        raw_options.emplace(lookup.at(name), value);
    }

    void add_raw_argument(const std::string& name)
    {
        raw_arguments.push_back(name);
    }

    argument& operator[](const std::string& flag)
    {
        return arguments[lookup.at(flag)];
    }

    const argument& operator[](const std::string& flag) const
    {
        return arguments[lookup.at(flag)];
    }

    void post_process()
    {
        for(auto&& p:raw_options)
        {
            arguments[p.first].write_value(p.second);
        }
    }
};

template<class T>
context build_context(T& cmd)
{
    context ctx;
    cmd.parse([&](auto& x, auto&&... xs)
    {
        argument arg;
        arg.write_value = [&x](const std::string& s) { write_value_to(x, s); };
        arg.type = get_argument_type(x);
        each_arg(overload(
            [&](const std::string& name) { arg.flags.push_back(name); }
        ), std::forward<decltype(xs)>(xs)...);
        ctx.add(std::move(arg));
    });
    return ctx;
}

template<class Iterator>
std::string pop_string(Iterator first, Iterator last)
{
    if (first == last) return std::string();
    else return std::string(first+1, last);
}

std::tuple<std::string, std::string> parse_attached_value(const std::string& s)
{
    assert(s.size() > 0);
    assert(s[0] == '-' && "Not parsing a flag");
    if (s[1] == '-')
    {
        auto it = std::find(s.begin(), s.end(), '=');
        return std::make_tuple(std::string(s.begin(), it), pop_string(it, s.end()));
    }
    else if (s.size() > 2)
    {
        return std::make_tuple(s.substr(0, 2), s.substr(2));
    }
    else
    {
        return std::make_tuple(s, std::string());
    }

}

template<class T>
void parse(T& cmd, std::vector<std::string> a)
{
    context ctx = build_context(cmd);

    bool capture = false;
    std::string core;
    for(auto&& x:a)
    {
        if (x[0] == '-')
        {
            std::string value;
            std::tie(core, value) = parse_attached_value(x);
            if (ctx[core].type == argument_type::none or not value.empty())
            {
                capture = false;
                ctx.add_raw_option(core, value);
            }
            else
            {
                capture = true;
            }
        }
        else if (capture)
        {
            ctx.add_raw_option(core, x);
            capture = ctx[core].type == argument_type::multiple;
        }
        else
        {
            ctx.add_raw_argument(x);
        }
    }
    ctx.post_process();

    cmd.run();
}

template<class T>
void parse(std::vector<std::string> a)
{
    T cmd;
    parse(cmd, std::move(a));
}


} // namespace args

#endif
