#pragma once

#include <functional>
#include <unordered_map>

#include <folly/Synchronized.h>

#include <jank/runtime/obj/symbol.hpp>
#include <jank/runtime/var.hpp>

namespace jank::runtime
{
  struct var;
  struct context;

  struct ns : object, pool_item_base<ns>
  {
    ns(obj::symbol_ptr const &name, context const &c)
      : name{ name }, ctx{ c }
    { }

    static ns_ptr create(obj::symbol_ptr const &n, context const &c);

    detail::boolean_type equal(object const &) const override;
    detail::string_type to_string() const override;
    detail::integer_type to_hash() const override;

    ns const* as_ns() const override;

    bool operator ==(ns const &rhs) const;

    obj::symbol_ptr name;
    folly::Synchronized<std::unordered_map<obj::symbol_ptr, var_ptr>> vars;
    context const &ctx;
  };
  using ns_ptr = detail::box_type<ns>;
}

namespace std
{
  template <>
  struct hash<jank::runtime::var>
  {
    size_t operator()(jank::runtime::var const &o) const noexcept
    { return static_cast<size_t>(o.n->name->to_hash()); }
  };
}
