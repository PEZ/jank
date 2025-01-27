#pragma once

#include <functional>

#include <folly/Synchronized.h>

#include <jank/runtime/object.hpp>
#include <jank/runtime/obj/symbol.hpp>

namespace jank::runtime
{
  struct ns;
  using ns_ptr = detail::box_type<ns>;

  struct var : object, pool_item_base<var>
  {
    var(var const&) = default;
    var(var &&) = default;
    var(ns_ptr const &n, obj::symbol_ptr const &s)
      : n{ n }, name{ s }
    { }
    var(ns_ptr const &n, obj::symbol_ptr const &s, object_ptr const &o)
      : n{ n }, name{ s }, root{ o }
    { }

    static detail::box_type<var> create(ns_ptr const &n, obj::symbol_ptr const &s);
    static detail::box_type<var> create(ns_ptr const &n, obj::symbol_ptr const &s, object_ptr const &root);

    detail::boolean_type equal(object const &) const override;
    detail::string_type to_string() const override;
    detail::integer_type to_hash() const override;

    var const* as_var() const override;

    bool operator ==(var const &rhs) const;

    object_ptr get_root() const;
    void set_root(object_ptr const &r);

    ns_ptr n;
    obj::symbol_ptr name;

  private:
    folly::Synchronized<object_ptr> root;
  };
  using var_ptr = detail::box_type<var>;
}
