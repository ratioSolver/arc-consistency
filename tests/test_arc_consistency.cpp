#include "arc_consistency.hpp"
#include "logging.hpp"
#include <cassert>

class test_enum_val : public arc_consistency::enum_val
{
public:
    explicit test_enum_val(std::string name) : name(std::move(name)) {}

    std::string to_string() const override { return name; }

private:
    std::string name;
};

void test0()
{
    arc_consistency::solver s;
    const auto v0 = s.new_sat();
    const auto v1 = s.new_sat();
    s.add_constraint(s.new_clause({{v0, true}, {v1, false}}));
    auto prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 2);
    assert(s.domain(v1).size() == 2);
    s.add_constraint(s.new_assign(v0, arc_consistency::solver::False));
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 1 && *s.domain(v0).begin() == &arc_consistency::solver::False);
    assert(s.domain(v1).size() == 1 && *s.domain(v1).begin() == &arc_consistency::solver::False);
}

void test1()
{
    arc_consistency::solver s;
    const auto v0 = s.new_sat();
    const auto v1 = s.new_sat();
    s.add_constraint(s.new_equal(v0, v1));
    auto prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 2);
    assert(s.domain(v1).size() == 2);
    s.add_constraint(s.new_assign(v0, arc_consistency::solver::True));
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 1 && *s.domain(v0).begin() == &arc_consistency::solver::True);
    assert(s.domain(v1).size() == 1 && *s.domain(v1).begin() == &arc_consistency::solver::True);
}

void test2()
{
    test_enum_val a("A");
    test_enum_val b("B");
    test_enum_val c("C");

    arc_consistency::solver s;
    const auto v0 = s.new_var({a, b, c});
    const auto v1 = s.new_var({a, b, c});
    s.add_constraint(s.new_equal(v0, v1));
    auto prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 3);
    assert(s.domain(v1).size() == 3);
    s.add_constraint(s.new_assign(v0, a));
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 1 && *s.domain(v0).begin() == &a);
    assert(s.domain(v1).size() == 1 && *s.domain(v1).begin() == &a);
}

void test3()
{
    test_enum_val a("A");
    test_enum_val b("B");
    test_enum_val c("C");

    arc_consistency::solver s;
    const auto v0 = s.new_var({a, b, c});
    const auto v1 = s.new_var({a, b, c});
    const auto v2 = s.new_var({a, b, c});
    auto &c1 = s.new_equal(v0, v1);
    auto &c2 = s.new_equal(v1, v2);
    s.add_constraint(c1);
    s.add_constraint(c2);
    auto prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 3);
    assert(s.domain(v1).size() == 3);
    assert(s.domain(v2).size() == 3);
    s.add_constraint(s.new_assign(v0, a));
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 1 && *s.domain(v0).begin() == &a);
    assert(s.domain(v1).size() == 1 && *s.domain(v1).begin() == &a);
    assert(s.domain(v2).size() == 1 && *s.domain(v2).begin() == &a);
    s.retract(c1);
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 1 && *s.domain(v0).begin() == &a);
    assert(s.domain(v1).size() == 3);
    assert(s.domain(v2).size() == 3);
    s.add_constraint(s.new_assign(v2, b));
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 1 && *s.domain(v0).begin() == &a);
    assert(s.domain(v1).size() == 1 && *s.domain(v1).begin() == &b);
    assert(s.domain(v2).size() == 1 && *s.domain(v2).begin() == &b);
}

void test4()
{
    test_enum_val a("A");
    test_enum_val b("B");

    arc_consistency::solver s;
    const auto v0 = s.new_var({a, b});
    const auto v1 = s.new_var({a, b});
    const auto v2 = s.new_var({a, b});
    auto &c1 = s.new_distinct(v0, v1);
    auto &c2 = s.new_distinct(v1, v2);
    s.add_constraint(c1);
    s.add_constraint(c2);
    auto prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 2);
    assert(s.domain(v1).size() == 2);
    assert(s.domain(v2).size() == 2);
    s.add_constraint(s.new_assign(v0, a));
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 1 && *s.domain(v0).begin() == &a);
    assert(s.domain(v1).size() == 1 && *s.domain(v1).begin() == &b);
    assert(s.domain(v2).size() == 1 && *s.domain(v2).begin() == &a);
    s.add_constraint(s.new_forbid(v2, a));
    prop = s.propagate();
    assert(!prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    s.retract(c1);
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 1 && *s.domain(v0).begin() == &a);
    assert(s.domain(v1).size() == 1 && *s.domain(v1).begin() == &a);
    assert(s.domain(v2).size() == 1 && *s.domain(v2).begin() == &b);
}

void test5()
{
    arc_consistency::solver s;
    const auto premise = s.new_sat();
    const auto conclusion = s.new_sat();
    s.add_constraint(s.new_imply(premise, arc_consistency::solver::True, conclusion, arc_consistency::solver::False));
    auto prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    s.add_constraint(s.new_assign(premise, arc_consistency::solver::True));
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(premise).size() == 1 && *s.domain(premise).begin() == &arc_consistency::solver::True);
    assert(s.domain(conclusion).size() == 1 && *s.domain(conclusion).begin() == &arc_consistency::solver::False);
}

void test6()
{
    arc_consistency::solver s;
    const auto premise = s.new_sat();
    const auto conclusion = s.new_sat();
    s.add_constraint(s.new_imply(premise, arc_consistency::solver::True, conclusion, arc_consistency::solver::True));
    auto prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    s.add_constraint(s.new_forbid(conclusion, arc_consistency::solver::True));
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(conclusion).size() == 1 && *s.domain(conclusion).begin() == &arc_consistency::solver::False);
    assert(s.domain(premise).size() == 1 && *s.domain(premise).begin() == &arc_consistency::solver::False);
    s.add_constraint(s.new_assign(premise, arc_consistency::solver::True));
    prop = s.propagate();
    assert(!prop);
    LOG_DEBUG(arc_consistency::to_string(s));
}

int main()
{
    test0();
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();

    return 0;
}
