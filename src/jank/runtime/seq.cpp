#include <iostream>
#include <sstream>

#include <jank/runtime/seq.hpp>
#include <jank/runtime/util.hpp>
#include <jank/runtime/hash.hpp>
#include <jank/runtime/obj/function.hpp>
#include <jank/runtime/obj/vector.hpp>
#include <jank/runtime/obj/map.hpp>
#include <jank/runtime/behavior/seq.hpp>
#include <jank/runtime/behavior/callable.hpp>

namespace jank::runtime
{
  /* TODO: Laziness. */
  object_ptr mapv(object_ptr const &f, object_ptr const &seq)
  {
    auto const * const sable(seq->as_seqable());
    if(!sable)
    {
      /* TODO: Throw error. */
      std::cout << "(mapv) not a seq: " << *seq << std::endl;
      return JANK_NIL;
    }

    auto const * const func(f->as_callable());
    if(!func)
    {
      /* TODO: Throw error. */
      std::cout << "(mapv) not callable: " << *f << std::endl;
      return JANK_NIL;
    }

    detail::vector_transient_type ret;

    for(auto s(sable->seq()); s != nullptr; s = s->next())
    { ret.push_back(func->call(s->first())); }

    return make_box<obj::vector>(ret.persistent());
  }

  object_ptr reduce(object_ptr const &f, object_ptr const &initial, object_ptr const &seq)
  {
    auto const * const sable(seq->as_seqable());
    if(!sable)
    {
      /* TODO: Throw error. */
      std::cout << "(mapv) not a seq: " << *seq << std::endl;
      return JANK_NIL;
    }

    auto const * const func(f->as_callable());
    if(!func)
    {
      /* TODO: Throw error. */
      std::cout << "(mapv) not callable: " << *f << std::endl;
      return JANK_NIL;
    }

    object_ptr acc{ initial };

    for(auto s(sable->seq()); s != nullptr; s = s->next())
    { acc = func->call(acc, s->first()); }

    return acc;
  }

  /* TODO: Laziness */
  object_ptr partition_gen_minus_all(object_ptr const &n, object_ptr const &seq)
  {
    auto const * const i(n->as_integer());
    if(!i)
    {
      /* TODO: Throw error */
      std::cout << "(partition-all) size must be an integer: " << *n << std::endl;
      return JANK_NIL;
    }
    size_t const partition_size(i->data);

    auto const * const sable(seq->as_seqable());
    if(!sable)
    {
      /* TODO: Throw error. */
      std::cout << "(partition-all) not a seq: " << *seq << std::endl;
      return JANK_NIL;
    }

    detail::vector_transient_type ret;

    for(auto s(sable->seq()); s != nullptr;)
    {
      detail::vector_transient_type partition;

      for(size_t k{}; k < partition_size && s != nullptr; ++k, s = s->next())
      { partition.push_back(s->first()); }

      ret.push_back(make_box<obj::vector>(partition.persistent()));
    }

    return make_box<obj::vector>(ret.persistent());
  }

  /* TODO: Laziness */
  object_ptr range(object_ptr const &start, object_ptr const &end)
  {
    auto const * const s(start->as_integer());
    auto const * const e(end->as_integer());
    if(!s || !e)
    {
      /* TODO: Throw error */
      std::cout << "(range) start and end must be integers: " << *start << " and " << *end << std::endl;
      return JANK_NIL;
    }

    auto const start_int(s->data);
    auto const end_int(e->data);

    if(end_int < start_int)
    {
      /* TODO: throw error */
      std::cout << "(range start must be < end" << std::endl;
      return JANK_NIL;
    }

    detail::vector_transient_type ret;
    for(auto i(start_int); i < end_int; ++i)
    { ret.push_back(make_box<obj::integer>(i)); }
    return make_box<obj::vector>(ret.persistent());
  }

  object_ptr reverse(object_ptr const &seq)
  {
    auto const * const sable(seq->as_seqable());
    if(!sable)
    {
      /* TODO: Throw error. */
      std::cout << "(reverse) not a seq: " << *seq << std::endl;
      return JANK_NIL;
    }

    /* TODO: Optimize this by supporting a better interface. */
    detail::vector_transient_type in_order, reverse_order;

    for(auto s(sable->seq()); s != nullptr; s = s->next())
    { in_order.push_back(s->first()); }
    for(auto it(in_order.rbegin()); it != in_order.rend(); ++it)
    { reverse_order.push_back(std::move(*it)); }

    return make_box<obj::vector>(reverse_order.persistent());
  }

  /* TODO: Associative interface. */
  object_ptr get(object_ptr const &o, object_ptr const &key)
  {
    auto const * const m(o->as_map());
    if(m)
    {
      if(auto const * found = m->data.find(key))
      { return *found; }
      else
      { return JANK_NIL; }
    }

    auto const * const v(o->as_vector());
    if(v)
    {
      auto const * const n(key->as_integer());
      if(!n)
      {
        /* TODO: Throw error. */
        std::cout << "(get) invalid vector index: " << *key << std::endl;
        return JANK_NIL;
      }

      auto const size(v->data.size());
      if(n->data < 0 || static_cast<size_t>(n->data) >= size)
      { return JANK_NIL; }

      return v->data[n->data];
    }

    std::cout << "(get) not associative: " << *o << " with key: " << *key << std::endl;
    return JANK_NIL;
  }

  /* TODO: Persistent collection interface. */
  object_ptr conj(object_ptr const &o, object_ptr const &val)
  {
    auto const * const m(o->as_map());
    if(m)
    {
      if(auto const * const v = val->as_vector())
      {
        if(v->data.size() != 2)
        {
          /* TODO: Throw error. */
          std::cout << "(conj) invalid map entry: " << *val << std::endl;
          return JANK_NIL;
        }
        detail::map_type copy{ m->data };
        copy.insert_or_assign(v->data[0], v->data[1]);
        return make_box<obj::map>(std::move(copy));
      }
      else
      { return JANK_NIL; }
    }

    auto const * const v(o->as_vector());
    if(v)
    { return make_box<obj::vector>(v->data.push_back(val)); }

    std::cout << "(conj) unsupported for: " << *o << std::endl;
    return JANK_NIL;
  }

  object_ptr assoc(object_ptr const &o, object_ptr const &key, object_ptr const &val)
  {
    auto const * const m(o->as_map());
    if(m)
    {
      detail::map_type copy{ m->data };
      copy.insert_or_assign(key, val);
      return make_box<obj::map>(std::move(copy));
    }

    auto const * const v(o->as_vector());
    if(v)
    {
      auto const * const n(key->as_integer());
      if(!n)
      {
        /* TODO: Throw error. */
        std::cout << "(assoc) invalid vector index: " << *key << std::endl;
        return JANK_NIL;
      }

      auto const size(v->data.size());
      if(n->data < 0 || static_cast<size_t>(n->data) > size)
      {
        /* TODO: Throw error. */
        std::cout << "(assoc) index out of bounds: " << *key << std::endl;
        return JANK_NIL;
      }
      else if(static_cast<size_t>(n->data) == size)
      { return make_box<obj::vector>(v->data.push_back(val)); }

      return make_box<obj::vector>(v->data.set(n->data, val));
    }

    std::cout << "(get) not associative: " << *o << std::endl;
    return JANK_NIL;
  }
}
