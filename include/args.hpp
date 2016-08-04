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
#include <algorithm>
#include <numeric>

#include <iostream>
#include <iomanip>

#include <cassert>

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
auto adl_begin(T&& x) -> decltype(begin(x))
{
    return begin(x);
}

template<class T>
auto is_container(args::rank<1>, T&& x) -> decltype(
    x.insert(end(x), *begin(x)), std::true_type{}
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

template<class Range>
using value_of = std::decay_t<decltype(*adl_args::adl_begin(std::declval<Range>()))>;

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

std::vector<std::string> wrap(const std::string& text, int line_length = 72)
{
    std::vector<std::string> output;
    std::istringstream iss(text);

    std::string line;

    do
    {
        std::string word;
        iss >> word;

        if (line.length() + word.length() > line_length)
        {
            output.push_back(line);
            line.clear();
        }
        line += word + " ";

    } while (iss);

    if (!line.empty())
    {
        output.push_back(line);
    }
    return output;
}

template<class Range>
std::string join(Range&& r, std::string delim)
{
    return std::accumulate(std::begin(r), std::end(r), std::string(), [&](const std::string &x, const std::string &y)
    {
        if (x.empty()) return y;
        if (y.empty()) return x;
        return x + delim + y;
    });
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
    result.insert(result.end(), value_parser<typename T::value_type>::apply(x));
}

void write_value_to(std::nullptr_t, const std::string&)
{
    // Do nothing
}

enum class argument_type
{
    none,
    single,
    multiple
};

template<class T>
argument_type get_argument_type(const T&)
{
    if (std::is_same<T, bool>() or std::is_same<T, std::nullptr_t>()) return argument_type::none;
    else if (is_container<T>()) return argument_type::multiple;
    else return argument_type::single;
}

template<class T>
std::string type_to_help_impl(rank<0>)
{
    if (std::is_same<T, bool>()) return "bool";
    else if (std::is_convertible<T, std::string>()) return "string";
    else if (std::is_integral<T>()) return "integer";
    else if (std::is_floating_point<T>()) return "number";
    else return "argument";
}

template<class T>
auto type_to_help_impl(rank<1>) -> typename std::enable_if<(is_container<T>() and not std::is_convertible<T, std::string>()), std::string>::type
{
    if (is_container<T>()) return type_to_help_impl<value_of<T>>(rank<1>{}) + "...";
}

template<class T>
std::string type_to_help(const T&)
{
    return "[" + type_to_help_impl<T>(rank<1>{}) + "]";
}

struct argument
{
    argument_type type;
    std::vector<std::string> flags;

    bool has_value = false;
    std::function<void(const std::string&)> write_value;
    std::vector<std::function<void(const argument&)>> callbacks;
    std::vector<std::function<void(const argument&)>> eager_callbacks;
    std::string help, metavar;

    std::unordered_map<std::string, std::string> data;

    template<class F>
    void add_callback(F f)
    {
        callbacks.emplace_back(std::move(f));
    }

    template<class F>
    void add_eager_callback(F f)
    {
        eager_callbacks.emplace_back(std::move(f));
    }

    std::string get_flags() const
    {
        std::string result = join(flags, ", ");
        if (type != argument_type::none) result += " " + metavar;
        return result;
    }

    bool write(const std::string& s)
    {
        write_value(s);
        has_value = true;
        for(auto&& f:eager_callbacks) f(*this);
        return not eager_callbacks.empty();
    }

};

struct context
{
    std::vector<argument> arguments;
    std::unordered_map<std::string, int> lookup;

    void add(argument arg)
    {
        if (arg.flags.empty()) lookup[""] = arguments.size();
        else for(auto&& name:arg.flags) lookup[name] = arguments.size();
        arguments.emplace_back(std::move(arg));
    }

    template<class T, class... Ts>
    void parse(T&& x, Ts&&... xs)
    {
        argument arg;
        arg.write_value = [&x](const std::string& s) { write_value_to(x, s); };
        arg.type = get_argument_type(x);
        arg.metavar = type_to_help(x);
        each_arg(overload(
            [&, this](const std::string& name) { arg.flags.push_back(name); },
            [&, this](auto&& attribute) -> decltype(attribute(x, *this, arg), void()) { attribute(x, *this, arg); }
        ), std::forward<Ts>(xs)...);
        this->add(std::move(arg));
    }

    argument& operator[](const std::string& flag)
    {
        return arguments[lookup.at(flag)];
    }

    const argument& operator[](const std::string& flag) const
    {
        return arguments[lookup.at(flag)];
    }

    void show_help(std::string name, std::string description) const
    {
        const int total_width = 80;
        std::vector<std::string> flags;
        int width = 0;
        for(auto&& arg:arguments)
        {
            std::string flag = arg.get_flags();
            width = std::max(width, int(flag.size()));
            flags.push_back(std::move(flag));
        }
        std::cout << "Usage: " << name << " [options...]";

        if (lookup.count("") > 0)
        {
            std::cout << " " << (*this)[""].metavar;
        }
        
        std::cout << std::endl;
        std::cout << std::endl;
        for(auto line:wrap(description, total_width-2)) std::cout << "  " << line << std::endl;
        std::cout << std::endl;
        std::cout << "Options: " << std::endl << std::endl;
        for(auto&& arg:arguments)
        {
            auto txt = wrap(arg.help, total_width-width-2);
            assert(!txt.empty());
            std::cout << " " << std::setw(width) << arg.get_flags() << " " << txt[0] << std::endl;
            std::for_each(txt.begin()+1, txt.end(), [&](std::string line)
            {
                std::cout << " " << std::setw(width) << " " << " " << line << std::endl;
            });
        }
        std::cout << std::endl;
    }

    void post_process()
    {
        for(auto&& arg:arguments)
        {
            for(auto&& f:arg.callbacks) f(arg);
        }
    }
};

template<class F>
auto callback(F f)
{
    return [f](auto&& data, context& ctx, argument& a)
    {
        a.add_callback([f, &ctx, &data](const argument& arg)
        {
            f(data, ctx, arg);
        });
    };
}

template<class F>
auto eager_callback(F f)
{
    return [f](auto&& data, context& ctx, argument& a)
    {
        a.add_eager_callback([f, &data, &ctx](const argument& arg)
        {
            f(data, ctx, arg);
        });
    };
}

#define ARGS_SET_ARG(name) \
template<class T> \
auto name(T&& x) \
{ \
    return [=](auto&&, context&, argument& a) \
    { \
        a.name = x; \
    }; \
}

ARGS_SET_ARG(help);
ARGS_SET_ARG(metavar);

template<class T>
context build_context(T& cmd)
{
    context ctx;
    ctx.parse(nullptr, "-h", "--help", args::help("Show help"), 
        eager_callback([](std::nullptr_t, const context& c, const argument&)
    {
        c.show_help("program", "This program");
    }));
    cmd.parse([&](auto&&... xs)
    {
        ctx.parse(std::forward<decltype(xs)>(xs)...);
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
            // TODO: Check if flag exists
            std::string value;
            std::tie(core, value) = parse_attached_value(x);
            if (ctx[core].type == argument_type::none or not value.empty())
            {
                capture = false;
                if (ctx[core].write(value)) return;
            }
            else
            {
                capture = true;
            }
        }
        else if (capture)
        {
            if (ctx[core].write(x)) return;
            capture = ctx[core].type == argument_type::multiple;
        }
        else
        {
            if (ctx[""].write(x)) return;
        }
    }
    ctx.post_process();

    cmd.run();
}

template<class T>
void parse(std::vector<std::string> a)
{
    // TODO: zero initialize T
    T cmd;
    parse(cmd, std::move(a));
}

} // namespace args

#endif
