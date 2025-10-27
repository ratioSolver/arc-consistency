#pragma once

#include "constraint.hpp"
#include <unordered_set>
#include <functional>
#include <memory>
#include <queue>

namespace arc_consistency
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
    static bool_val True;
    static bool_val False;

    bool_val(bool v) : value(v) {}

    bool get_value() const { return value; }

    std::string to_string() const override { return value ? "True" : "False"; }

  private:
    bool value;
  };

  class solver
  {
    friend class constraint;

  public:
    static bool_val True;
    static bool_val False;

    [[nodiscard]] utils::var new_sat() noexcept;
    [[nodiscard]] utils::var new_var(const std::vector<std::reference_wrapper<utils::enum_val>> &domain) noexcept;

    [[nodiscard]] utils::lbool sat_val(const utils::var &x) const noexcept;
    [[nodiscard]] const std::vector<std::reference_wrapper<utils::enum_val>> domain(utils::var v) const noexcept;

    void add_constraint(const std::shared_ptr<constraint> &c) noexcept;
    void remove_constraint(const std::shared_ptr<constraint> &c) noexcept;

  private:
    [[nodiscard]] bool remove(utils::var v, const utils::enum_val &val) noexcept;

  private:
    std::vector<std::unordered_set<utils::enum_val *>> init_domain; // initial domains
    std::vector<std::unordered_set<utils::enum_val *>> dom;         // current domains
    std::unordered_set<std::shared_ptr<constraint>> constraints;    // all the constraints
    std::queue<utils::var> to_propagate;                            // variables to propagate
  };
} // namespace arc_consistency
