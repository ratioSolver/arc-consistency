#pragma once

#include "constraint.hpp"
#include <functional>
#include <memory>
#include <queue>
#ifdef ARCCONSISTENCY_ENABLE_LISTENERS
#include <set>
#endif

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

#ifdef ARCCONSISTENCY_ENABLE_LISTENERS
  class listener;
#endif

  class solver
  {
    friend class constraint;

  public:
    static bool_val True;
    static bool_val False;

    solver() noexcept;

    /**
     * @brief Creates and returns a new SAT variable.
     *
     * This function generates a new variable that can take the values `solver::True` or `solver::False`.
     *
     * @return utils::var The newly created SAT variable.
     */
    [[nodiscard]] utils::var new_sat() noexcept { return new_var({solver::True, solver::False}); }
    /**
     * @brief Creates and returns a new variable with the given domain.
     *
     * This function generates a new variable that can take values from the specified domain.
     *
     * @param domain The domain of the new variable.
     * @return utils::var The newly created variable.
     */
    [[nodiscard]] utils::var new_var(const std::vector<std::reference_wrapper<utils::enum_val>> &domain) noexcept;

    /**
     * @brief Gets the SAT value of a variable.
     *
     * This function returns the SAT value of the specified variable.
     *
     * @param x The variable whose SAT value is to be retrieved.
     * @return utils::lbool The SAT value of the variable: `utils::True`, `utils::False`, or `utils::Undefined`.
     */
    [[nodiscard]] utils::lbool sat_val(const utils::var &x) const noexcept;
    /**
     * @brief Gets the SAT value of a literal.
     *
     * This function returns the SAT value of the specified literal.
     *
     * @param l The literal whose SAT value is to be retrieved.
     * @return utils::lbool The SAT value of the literal: `utils::True`, `utils::False`, or `utils::Undefined`.
     */
    [[nodiscard]] utils::lbool sat_val(const utils::lit &l) const noexcept;
    /**
     * @brief Gets the current domain of a variable.
     *
     * This function returns the current domain of the specified variable.
     *
     * @param v The variable whose domain is to be retrieved.
     * @return std::vector<std::reference_wrapper<utils::enum_val>> The current domain of the variable.
     */
    [[nodiscard]] const std::vector<std::reference_wrapper<utils::enum_val>> domain(utils::var v) const noexcept;

    /**
     * @brief Creates a new clause constraint.
     *
     * This function creates a new clause constraint with the specified literals.
     *
     * @param lits The literals that make up the clause.
     * @return std::shared_ptr<constraint> A shared pointer to the newly created clause constraint.
     */
    [[nodiscard]] std::shared_ptr<constraint> new_clause(std::vector<utils::lit> &&lits) noexcept;
    /**
     * @brief Creates a new equal constraint.
     *
     * This function creates a new equal constraint between two variables.
     *
     * @param x The first variable.
     * @param y The second variable.
     * @return std::shared_ptr<constraint> A shared pointer to the newly created equal constraint.
     */
    [[nodiscard]] std::shared_ptr<constraint> new_equal(utils::var x, utils::var y) noexcept;
    /**
     * @brief Creates a new distinct constraint.
     *
     * This function creates a new distinct constraint between two variables.
     *
     * @param x The first variable.
     * @param y The second variable.
     * @return std::shared_ptr<constraint> A shared pointer to the newly created distinct constraint.
     */
    [[nodiscard]] std::shared_ptr<constraint> new_distinct(utils::var x, utils::var y) noexcept;
    /**
     * @brief Creates a new assign constraint.
     *
     * This function creates a new assign constraint that assigns a specific value to a variable.
     *
     * @param x The variable to be assigned.
     * @param val The value to assign to the variable.
     * @return std::shared_ptr<constraint> A shared pointer to the newly created assign constraint.
     */
    [[nodiscard]] std::shared_ptr<constraint> new_assign(utils::var x, utils::enum_val &val) noexcept;
    /**
     * @brief Creates a new forbid constraint.
     *
     * This function creates a new forbid constraint that forbids a specific value for a variable.
     *
     * @param x The variable to be constrained.
     * @param val The value to forbid for the variable.
     * @return std::shared_ptr<constraint> A shared pointer to the newly created forbid constraint.
     */
    [[nodiscard]] std::shared_ptr<constraint> new_forbid(utils::var x, utils::enum_val &val) noexcept;

    /**
     * @brief Adds a constraint to the solver.
     *
     * This function adds the specified constraint to the solver for propagation.
     *
     * @param c A shared pointer to the constraint to be added.
     */
    void add_constraint(std::shared_ptr<constraint> c) noexcept;
    /**
     * @brief Removes a constraint from the solver.
     *
     * This function removes the specified constraint from the solver.
     *
     * @param c A shared pointer to the constraint to be removed.
     */
    void remove_constraint(const std::shared_ptr<constraint> &c) noexcept;

    /**
     * @brief Propagates all constraints in the solver.
     *
     * This function performs arc consistency propagation on all constraints in the solver.
     *
     * @return true If no domain is emptied during propagation.
     * @return false If a domain is emptied during propagation.
     */
    [[nodiscard]] bool propagate() noexcept;

#ifdef ARCCONSISTENCY_ENABLE_LISTENERS
    /**
     * @brief Adds a listener to the solver.
     *
     * This function registers a listener that will be notified of domain changes.
     *
     * @param l A shared pointer to the listener to be added.
     */
    void add_listener(std::shared_ptr<listener> l) noexcept;
    /**
     * @brief Removes a listener from the solver.
     *
     * This function unregisters a listener from receiving domain change notifications.
     *
     * @param l A shared pointer to the listener to be removed.
     */
    void remove_listener(std::shared_ptr<listener> l) noexcept;
#endif

    friend std::string to_string(const solver &s) noexcept;
    friend std::string to_string(const solver &s, utils::var v) noexcept;

  private:
    [[nodiscard]] bool remove(utils::var v, utils::enum_val &val, constraint &c) noexcept;

  private:
    std::vector<std::unordered_set<utils::enum_val *>> init_domain; // initial domains
    std::vector<std::unordered_set<utils::enum_val *>> dom;         // current domains
    std::vector<std::unordered_set<constraint *>> watchlist;        // watchlist for each variable
    std::unordered_set<std::shared_ptr<constraint>> constraints;    // all constraints
    std::queue<std::pair<utils::var, constraint *>> to_propagate;   // variables to propagate
#ifdef ARCCONSISTENCY_ENABLE_LISTENERS
    std::unordered_map<utils::var, std::set<listener *>> listening; // for each variable, the listeners listening to it..
    std::set<std::shared_ptr<listener>> listeners;                  // the collection of listeners..
#endif
  };

#ifdef ARCCONSISTENCY_ENABLE_LISTENERS
  class listener
  {
  public:
    virtual void on_domain_changed(const utils::var v) noexcept = 0;
  };
#endif

  [[nodiscard]] std::string to_string(const solver &s) noexcept;
  [[nodiscard]] std::string to_string(const solver &s, utils::var v) noexcept;
} // namespace arc_consistency
