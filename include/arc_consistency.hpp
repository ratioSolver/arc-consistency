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

    std::string to_string() const override { return value ? "⊤" : "⊥"; }

  private:
    bool value;
  };

  class solver
  {
    friend class constraint;

  public:
    static bool_val True;
    static bool_val False;

    [[nodiscard]] utils::var new_sat() noexcept { return new_var({std::ref(solver::True), std::ref(solver::False)}); }
    [[nodiscard]] utils::var new_var(const std::vector<std::reference_wrapper<utils::enum_val>> &domain) noexcept;

    [[nodiscard]] utils::lbool sat_val(const utils::var &x) const noexcept;
    [[nodiscard]] utils::lbool sat_val(const utils::lit &l) const noexcept;
    [[nodiscard]] const std::vector<std::reference_wrapper<utils::enum_val>> domain(utils::var v) const noexcept;

    void add_constraint(const std::shared_ptr<constraint> &c) noexcept;
    void remove_constraint(const std::shared_ptr<constraint> &c) noexcept;

    [[nodiscard]] bool assign(utils::var v, const utils::enum_val &val) noexcept;

    [[nodiscard]] bool propagate() noexcept;

    friend std::string to_string(const solver &s) noexcept;
    friend std::string to_string(const solver &s, utils::var v) noexcept;

  private:
    [[nodiscard]] bool remove(utils::var v, utils::enum_val &val) noexcept;

  private:
    std::vector<std::unordered_set<utils::enum_val *>> init_domain;         // initial domains
    std::vector<std::unordered_set<utils::enum_val *>> dom;                 // current domains
    std::vector<std::unordered_set<std::shared_ptr<constraint>>> watchlist; // watchlist for each variable
    std::unordered_set<std::shared_ptr<constraint>> constraints;            // all constraints
    std::queue<utils::var> to_propagate;                                    // variables to propagate
  };

  [[nodiscard]] std::string to_string(const solver &s) noexcept;
  [[nodiscard]] std::string to_string(const solver &s, utils::var v) noexcept;
} // namespace arc_consistency
