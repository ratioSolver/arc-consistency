#include "arc_consistency.hpp"

namespace arc_consistency
{
    utils::var solver::new_sat() noexcept
    {
        const auto x = vars.size();
        std::unordered_set<const utils::enum_val *> domain_set = {&bool_val::True, &bool_val::False};
        vars.emplace_back(std::move(domain_set));
        return x;
    }

    utils::var solver::new_var(const std::vector<std::reference_wrapper<const utils::enum_val>> &domain) noexcept
    {
        const auto x = vars.size();
        std::unordered_set<const utils::enum_val *> domain_set;
        for (const auto &ev_ref : domain)
            domain_set.insert(&ev_ref.get());
        vars.emplace_back(std::move(domain_set));
        return x;
    }
} // namespace arc_consistency
