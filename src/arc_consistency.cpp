#include "arc_consistency.hpp"
#include <cassert>

namespace arc_consistency
{
    bool_val solver::True{true};
    bool_val solver::False{false};

    utils::var solver::new_sat() noexcept
    {
        const auto x = init_domain.size();
        std::unordered_set<utils::enum_val *> domain_set = {&solver::True, &solver::False};
        init_domain.emplace_back(std::move(domain_set));
        return x;
    }

    utils::var solver::new_var(const std::vector<std::reference_wrapper<utils::enum_val>> &domain) noexcept
    {
        const auto x = init_domain.size();
        std::unordered_set<utils::enum_val *> domain_set;
        for (const auto &ev_ref : domain)
            domain_set.insert(&ev_ref.get());
        init_domain.emplace_back(std::move(domain_set));
        return x;
    }

    const std::vector<std::reference_wrapper<utils::enum_val>> solver::domain(utils::var v) const noexcept
    {
        std::vector<std::reference_wrapper<utils::enum_val>> dom_vec;
        for (auto *ev_ptr : dom[v])
            dom_vec.emplace_back(*ev_ptr);
        return dom_vec;
    }

    void solver::add_constraint(const std::shared_ptr<constraint> &c) noexcept
    {
        for (const auto &v : c->get_scope())
            to_propagate.push(v);
        constraints.insert(c);
    }

    void solver::remove_constraint(const std::shared_ptr<constraint> &c) noexcept
    {
        constraints.erase(c);
    }

    bool solver::remove(utils::var v, const utils::enum_val &val) noexcept
    {
        assert(v < dom.size());
        assert(dom[v].find(const_cast<utils::enum_val *>(&val)) != dom[v].end());
        dom[v].erase(const_cast<utils::enum_val *>(&val));
        if (dom[v].empty())
            return false;
        to_propagate.push(v);
        return true;
    }
} // namespace arc_consistency
