#include "var.hpp"
#include <algorithm>
#include <cassert>

namespace deltacsp
{
    const bool_val bool_val::True{true};
    const bool_val bool_val::False{false};

    var::var(const std::unordered_set<const utils::enum_val *> &&dom, const utils::enum_val *init_v) noexcept : domain(std::move(dom)), value(init_v) { assert(init_v == nullptr || domain.find(init_v) != domain.end()); }

    std::string to_string(const var &x) noexcept
    {
        std::string res;
        if (x.value)
        {
            res += " = ";
            if (const auto *ev = dynamic_cast<const enum_val *>(x.value))
                res += ev->to_string();
        }
        if (std::all_of(x.domain.begin(), x.domain.end(), [&x](const utils::enum_val *v)
                        { return dynamic_cast<const enum_val *>(v); }))
        {
            res += " {";
            for (auto it = x.domain.begin(); it != x.domain.end(); ++it)
            {
                const auto *ev = dynamic_cast<const enum_val *>(*it);
                res += ev->to_string();
                if (std::next(it) != x.domain.end())
                    res += ", ";
            }
            res += "}";
        }
        return res;
    }
} // namespace deltacsp
