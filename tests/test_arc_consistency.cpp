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
    assert(s.domain(v0).size() == 1 && &s.domain(v0).begin()->get() == &arc_consistency::solver::False);
    assert(s.domain(v1).size() == 1 && &s.domain(v1).begin()->get() == &arc_consistency::solver::False);
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
    assert(s.domain(v0).size() == 1 && &s.domain(v0).begin()->get() == &arc_consistency::solver::True);
    assert(s.domain(v1).size() == 1 && &s.domain(v1).begin()->get() == &arc_consistency::solver::True);
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
    assert(s.domain(v0).size() == 1 && &s.domain(v0).begin()->get() == &a);
    assert(s.domain(v1).size() == 1 && &s.domain(v1).begin()->get() == &a);
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
    auto c1 = s.new_equal(v0, v1);
    auto c2 = s.new_equal(v1, v2);
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
    assert(s.domain(v0).size() == 1 && &s.domain(v0).begin()->get() == &a);
    assert(s.domain(v1).size() == 1 && &s.domain(v1).begin()->get() == &a);
    assert(s.domain(v2).size() == 1 && &s.domain(v2).begin()->get() == &a);
    s.remove_constraint(c1);
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 1 && &s.domain(v0).begin()->get() == &a);
    assert(s.domain(v1).size() == 3);
    assert(s.domain(v2).size() == 3);
    s.add_constraint(s.new_assign(v2, b));
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 1 && &s.domain(v0).begin()->get() == &a);
    assert(s.domain(v1).size() == 1 && &s.domain(v1).begin()->get() == &b);
    assert(s.domain(v2).size() == 1 && &s.domain(v2).begin()->get() == &b);
}

void test4()
{
    test_enum_val a("A");
    test_enum_val b("B");

    arc_consistency::solver s;
    const auto v0 = s.new_var({a, b});
    const auto v1 = s.new_var({a, b});
    const auto v2 = s.new_var({a, b});
    auto c1 = s.new_distinct(v0, v1);
    auto c2 = s.new_distinct(v1, v2);
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
    assert(s.domain(v0).size() == 1 && &s.domain(v0).begin()->get() == &a);
    assert(s.domain(v1).size() == 1 && &s.domain(v1).begin()->get() == &b);
    assert(s.domain(v2).size() == 1 && &s.domain(v2).begin()->get() == &a);
    s.add_constraint(s.new_forbid(v2, a));
    prop = s.propagate();
    assert(!prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    s.remove_constraint(c1);
    prop = s.propagate();
    assert(prop);
    LOG_DEBUG(arc_consistency::to_string(s));
    assert(s.domain(v0).size() == 1 && &s.domain(v0).begin()->get() == &a);
    assert(s.domain(v1).size() == 1 && &s.domain(v1).begin()->get() == &a);
    assert(s.domain(v2).size() == 1 && &s.domain(v2).begin()->get() == &b);
}

int main()
{
    test0();
    test1();
    test2();
    test3();
    test4();

    return 0;
}
