#pragma once

#include "enum.hpp"
#include <unordered_set>
#include <string>

namespace ac
{
  class enum_val : public utils::enum_val
  {
  public:
    virtual ~enum_val() = default;

    virtual std::string to_string() const = 0;
  };

  class bool_val : public enum_val
  {
  public:
    static const bool_val True;
    static const bool_val False;

    bool_val(bool v) : value(v) {}

    bool get_value() const { return value; }

    std::string to_string() const override { return value ? "True" : "False"; }

  private:
    bool value;
  };

  class var
  {
    friend class solver;

  public:
    var(const std::unordered_set<const utils::enum_val *> &&init_dom) noexcept;

    friend std::string to_string(const var &x) noexcept;

  private:
    std::unordered_set<const utils::enum_val *> init_domain; // The initial domain..
    std::unordered_set<const utils::enum_val *> dom;         // The current domain..
  };

  [[nodiscard]] std::string to_string(const var &x) noexcept;
} // namespace ac
