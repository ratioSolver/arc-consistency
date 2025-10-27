#pragma once

#include "bool.hpp"
#include "enum.hpp"
#include "lit.hpp"
#include <vector>
#include <unordered_set>

namespace arc_consistency
{
  class solver;

  class constraint
  {
  public:
    constraint(solver &slv) noexcept : slv(slv) {}
    virtual ~constraint() = default;

    virtual std::vector<utils::var> scope() const noexcept = 0;
    virtual bool propagate(utils::var v) noexcept = 0;

    virtual std::string to_string() const noexcept = 0;

  protected:
    [[nodiscard]] bool remove(utils::var v, utils::enum_val &val) noexcept;
    [[nodiscard]] std::unordered_set<utils::enum_val *> &domain(utils::var v) const noexcept;

  protected:
    solver &slv;
  };

  class assign final : public constraint
  {
  public:
    assign(solver &slv, utils::var v, utils::enum_val &val) noexcept;

    std::vector<utils::var> scope() const noexcept override;
    bool propagate(utils::var v) noexcept override;

    std::string to_string() const noexcept override;

  private:
    utils::var v;
    utils::enum_val &val;
  };

  class forbid final : public constraint
  {
  public:
    forbid(solver &slv, utils::var v, utils::enum_val &val) noexcept;

    std::vector<utils::var> scope() const noexcept override;
    bool propagate(utils::var v) noexcept override;

    std::string to_string() const noexcept override;

  private:
    utils::var v;
    utils::enum_val &val;
  };

  class clause final : public constraint
  {
  public:
    clause(solver &slv, std::vector<utils::lit> &&lits) noexcept;

    std::vector<utils::var> scope() const noexcept override;
    bool propagate(utils::var v) noexcept override;

    std::string to_string() const noexcept override;

  private:
    std::vector<utils::lit> lits;
  };

  class eq final : public constraint
  {
  public:
    eq(solver &slv, utils::var var1, utils::var var2) noexcept;

    std::vector<utils::var> scope() const noexcept override;
    bool propagate(utils::var v) noexcept override;

    std::string to_string() const noexcept override;

  private:
    utils::var var1;
    utils::var var2;
  };

  class neq final : public constraint
  {
  public:
    neq(solver &slv, utils::var var1, utils::var var2) noexcept;

    std::vector<utils::var> scope() const noexcept override;
    bool propagate(utils::var v) noexcept override;

    std::string to_string() const noexcept override;

  private:
    utils::var var1;
    utils::var var2;
  };
} // namespace arc_consistency
