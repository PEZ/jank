#pragma once

#include <jank/translate/cell/cell.hpp>
#include <jank/translate/environment/scope.hpp>
#include <jank/interpret/environment/scope.hpp>
#include <jank/interpret/plugin/detail/find_declaration.hpp>

namespace jank
{
  namespace interpret
  {
    namespace plugin
    {
      namespace detail
      {
        template <typename... Args>
        void make_function
        (
          std::shared_ptr<translate::environment::scope> const &trans_scope,
          std::shared_ptr<environment::scope> const &int_scope,
          std::string const &name,
          plugin::detail::native_function_definition::function_type const &apply,
          translate::cell::detail::type_reference<translate::cell::cell> const &ret_type,
          Args &&...args
        )
        {
          auto const &decl
          (
            detail::find_declaration
            (
              trans_scope, name,
              ret_type,
              std::forward<Args>(args)...
            )
          );
          int_scope->native_function_definitions[decl] =
          { name, apply };
        }
      }
    }
  }
}