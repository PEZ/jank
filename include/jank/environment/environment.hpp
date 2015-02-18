#pragma once

#include <map>
#include <string>
#include <memory>
#include <experimental/optional>

#include <jank/cell/cell.hpp>
#include <jank/function/arg.hpp>

namespace jank
{
  namespace environment
  {
    struct environment
    {
      std::experimental::optional<cell::cell> find_cell(std::string const &name);
      std::experimental::optional<cell::func> find_function(std::string const &name);
      std::experimental::optional<cell::func> find_special(std::string const &name);

      std::map<std::string, cell::cell> cells;
      
      /* TODO: map<string, vector<func>> for overloading.
       * Each func has a vector<type> for the args.
       * Calling a function first type checks each overload. */
      std::map<std::string, std::vector<cell::func>> funcs;

      std::map<std::string, cell::func> special_funcs;

      // TODO std::shared_ptr<environment> parent;
      environment *parent;
    };
  }
  namespace cell
  {
    template <>
    struct wrapper<type::function>
    {
      using type = std::function<cell (environment::environment&, list const&)>;

      std::vector<function::argument> arguments;
      type data;
      environment::environment env;
    };
  }

  namespace environment
  {
    std::experimental::optional<cell::cell> environment::find_cell(std::string const &name)
    {
      auto const it(cells.find(name));
      if(it == cells.end())
      {
        if(parent)
        { return parent->find_cell(name); }
        else
        { return {}; }
      }
      return { it->second };
    }
    std::experimental::optional<cell::func> environment::find_function(std::string const &name)
    {
      auto const it(funcs.find(name));
      if(it == funcs.end())
      {
        if(parent)
        { return parent->find_function(name); }
        else
        { return {}; }
      }

      if(it->second.empty())
      { throw std::runtime_error{ "unknown function: " + name }; }

      return { it->second[0] };
    }
    std::experimental::optional<cell::func> environment::find_special(std::string const &name)
    {
      auto const it(special_funcs.find(name));
      if(it == special_funcs.end())
      {
        if(parent)
        { return parent->find_special(name); }
        else
        { return {}; }
      }
      return { it->second };
    }
  }
}
