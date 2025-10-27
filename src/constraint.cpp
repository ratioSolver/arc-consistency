#include "constraint.hpp"
#include "arc_consistency.hpp"
#include <unordered_set>
#include <cassert>

namespace arc_consistency
{
    bool constraint::remove(utils::var v, const utils::enum_val &val) noexcept
    {
        assert(v < slv.vars.size());
        assert(slv.vars[v].dom.find(const_cast<utils::enum_val *>(&val)) != slv.vars[v].dom.end());
        slv.vars[v].dom.erase(const_cast<utils::enum_val *>(&val));
        if (slv.vars[v].dom.empty())
            return false;
        slv.enqueue(v);
        return true;
    }

    clause::clause(solver &slv, std::vector<utils::lit> &&lits) noexcept : constraint(slv), lits{std::move(lits)} {}

    std::vector<utils::var> clause::get_scope() const noexcept
    {
        std::vector<utils::var> scope;
        for (const auto &l : lits)
            scope.push_back(utils::variable(l));
        return scope;
    }

    bool clause::propagate(utils::var v) noexcept
    {
        std::size_t unassigned_count = 0;
        utils::lit unassigned_lit;
        for (const auto &l : lits)
        {
            const auto var = utils::variable(l);
            const auto &var_dom = slv.domain(var);
            if (var_dom.size() == 1)
            {
                const auto &val = var_dom.begin()->get();
                if ((utils::sign(l) && &val == &bool_val::True) || (!utils::sign(l) && &val == &bool_val::False))
                    return false; // Clause is satisfied
            }
            else if (unassigned_count > 1)
                return true; // More than one unassigned literal, nothing to do
            else
            {
                ++unassigned_count;
                unassigned_lit = l;
            }
        }
        if (unassigned_count == 1)
            remove(utils::variable(unassigned_lit), utils::sign(unassigned_lit) ? static_cast<const utils::enum_val &>(bool_val::False) : static_cast<const utils::enum_val &>(bool_val::True));
        else if (unassigned_count == 0)
            return false; // Clause is unsatisfied
        return true;
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
