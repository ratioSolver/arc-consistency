#pragma once

#include "var.hpp"
#include "constraint.hpp"
#include <functional>
#include <memory>
#include <queue>

namespace arc_consistency
{
  class solver
  {
  public:
    [[nodiscard]] utils::var new_sat() noexcept;
    [[nodiscard]] utils::var new_var(const std::vector<std::reference_wrapper<utils::enum_val>> &domain) noexcept;

    void add_constraint(const std::shared_ptr<constraint> &c) noexcept;
    void remove_constraint(const std::shared_ptr<constraint> &c) noexcept;

  private:
    std::vector<var> vars;                                       // index is the variable id
    std::unordered_set<std::shared_ptr<constraint>> constraints; // all the constraints
    std::queue<utils::var> to_propagate;                         // variables to propagate
  };
} // namespace arc_consistency
