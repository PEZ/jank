#pragma once

#include <string_view>
#include <variant>

#include <jank/detail/type.hpp>
#include <jank/result.hpp>
#include <jank/option.hpp>

namespace jank::read::lex
{
  enum class token_kind
  {
    open_paren,
    close_paren,
    open_square_bracket,
    close_square_bracket,
    open_curly_bracket,
    close_curly_bracket,
    /* Has string data. */
    symbol,
    /* Has string data. */
    keyword,
    /* Has int data. */
    integer,
    /* Has string data. */
    string,
    eof,
  };

  struct token
  {
    token(token_kind const k) : kind{ k }
    { }
    token(size_t const p, token_kind const k) : pos{ p }, kind{ k }
    { }
    template <typename T>
    token(token_kind const k, T &&t) : kind{ k }, data{ std::forward<T>(t) }
    { }
    template <typename T>
    token(size_t const p, token_kind const k, T &&t)
      : pos{ p }, kind{ k }, data{ std::forward<T>(t) }
    { }

    bool operator ==(token const &rhs) const;
    bool operator !=(token const &rhs) const;

    struct no_data
    {
      bool operator ==(no_data const &rhs) const;
      bool operator !=(no_data const &rhs) const;
    };

    size_t pos{};
    token_kind kind;
    std::variant<no_data, detail::int_type, detail::string_view_type> data;
  };
  std::ostream& operator <<(std::ostream &os, token const &t);
  std::ostream& operator <<(std::ostream &os, token::no_data const &t);
}

namespace jank::read
{
  struct error
  {
    error(size_t const s, detail::string_type const &m)
      : start{ s }, end{ s }, message{ m }
    { }
    error(size_t const s, size_t const e, detail::string_type const &m)
      : start{ s }, end{ e }, message{ m }
    { }

    bool operator ==(error const &rhs) const
    { return !(*this != rhs); }
    bool operator !=(error const &rhs) const
    { return start != rhs.start || end != rhs.end || message != rhs.message; }

    size_t start{};
    size_t end{};
    detail::string_type message;
  };
  std::ostream& operator <<(std::ostream &os, error const &e);
}

namespace jank::read::lex
{
  struct processor
  {
    struct iterator
    {
      using iterator_category = std::input_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using value_type = result<token, error>;
      using pointer = value_type*;
      using reference = value_type&;

      value_type operator *() const;
      iterator& operator ++();
      bool operator ==(iterator const &rhs) const;
      bool operator !=(iterator const &rhs) const;

      option<value_type> latest;
      processor &p;
    };

    processor(detail::string_view_type const &f) : file{ f }
    { }

    result<token, error> next();
    option<char> peek() const;
    option<error> check_whitespace(bool const found_space);

    iterator begin();
    iterator end();

    size_t pos{};
    /* Whether or not the previous token requires a space after it. */
    bool require_space{};
    detail::string_view_type file;
  };
}
