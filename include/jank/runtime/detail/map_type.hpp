#pragma once

#include <ostream>
#include <memory>
#include <vector>

#include <jank/runtime/hash.hpp>

namespace jank::runtime::detail
{
  /* This is a short map, storing a vector of pairs. This is only until immer has proper
   * support for short maps and map transients. */
  template <typename K, typename V>
  struct map_type_impl
  {
    using value_type = std::vector<std::pair<K, V>>;
    using iterator = typename value_type::iterator;
    using const_iterator = typename value_type::const_iterator;

    map_type_impl() = default;
    map_type_impl(map_type_impl const &s) = default;
    map_type_impl(map_type_impl &&s) = default;

    template <typename NK, typename NV>
    void insert_or_assign(NK &&key, NV &&val)
    {
      if(auto * const found = find(key))
      { *found = std::forward<NV>(val); }
      else
      { data.emplace_back(std::forward<NK>(key), std::forward<NV>(val)); }
    }

    void insert_all_impl()
    { }
    template <typename NK, typename NV, typename... Args>
    void insert_all_impl(NK &&key, NV &&val, Args &&... args)
    {
      if(auto * const found = find(key))
      { *found = std::forward<NV>(val); }
      else
      { data.emplace_back(std::forward<NK>(key), std::forward<NV>(val)); }
      insert_all(std::forward<Args>(args)...);
    }
    template <typename... Args>
    void insert_all(Args &&... args)
    {
      data.reserve(data.size() + sizeof...(args));
      insert_all_impl(std::forward<Args>(args)...);
    }

    V* find(K const &key)
    {
      for(auto &kv : data)
      {
        if(kv.first == key)
        { return &kv.second; }
      }
      return nullptr;
    }
    V const* find(K const &key) const
    {
      for(auto const &kv : data)
      {
        if(kv.first == key)
        { return &kv.second; }
      }
      return nullptr;
    }

    bool operator==(map_type_impl const &s) const
    { return data == s.data; }

    size_t to_hash() const
    {
      if(hash != 0)
      { return hash; }

      hash = data.size();
      for(auto const &kv : data)
      {
        hash = jank::runtime::detail::hash_combine(hash, *kv.first);
        hash = jank::runtime::detail::hash_combine(hash, *kv.second);
      }
      return hash;
    }

    iterator begin()
    { return data.begin(); }
    const_iterator begin() const
    { return data.begin(); }
    iterator end()
    { return data.end(); }
    const_iterator end() const
    { return data.end(); }

    size_t size() const
    { return data.size(); }

    value_type data;
    mutable size_t hash{};
  };
}
