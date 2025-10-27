#include "constraint.hpp"
#include <unordered_set>
#include <cassert>

namespace arc_consistency
{
    clause::clause(solver &slv, std::vector<utils::lit> &&lits) noexcept : constraint(slv), lits{std::move(lits)} {}

    std::vector<utils::var> clause::get_scope() const noexcept
    {
        std::vector<utils::var> scope;
        for (const auto &l : lits)
            scope.push_back(utils::variable(l));
        return scope;
    }

    std::string clause::to_string() const noexcept
    {
        std::string result = "(";
        for (auto it = lits.begin(); it != lits.end(); ++it)
        {
            result += utils::to_string(*it);
            if (it + 1 != lits.end())
                result += " ∨ ";
        }
        result += ")";
        return result;
    }

    eq::eq(solver &slv, utils::var var1, utils::var var2) noexcept : constraint(slv), var1{var1}, var2{var2} {}

    std::vector<utils::var> eq::get_scope() const noexcept { return {var1, var2}; }

    std::string eq::to_string() const noexcept { return "v" + std::to_string(var1) + " = v" + std::to_string(var2); }

    neq::neq(solver &slv, utils::var var1, utils::var var2) noexcept : constraint(slv), var1{var1}, var2{var2} {}

    std::vector<utils::var> neq::get_scope() const noexcept { return {var1, var2}; }

    std::string neq::to_string() const noexcept { return "v" + std::to_string(var1) + " ≠ v" + std::to_string(var2); }
} // namespace arc_consistency
