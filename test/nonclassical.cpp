/** Tests on contractions where a summation is involved by more than two
 * factors.
 *
 * A classical tensor contraction has every summation involved by exactly two
 * factors.  The search has an acceleration that skips a partition shattering
 * the factors into more than two already-memoized chunks, which is only valid
 * under that assumption.  These tests cover problems outside it.
 */

#include <sstream>
#include <vector>

#include <libparenth.hpp>

// Disable Catch2's range detection for fbitset by providing stream insertion
// operator.
namespace fbitset {
template <Size N, typename L, typename E>
inline std::ostream& operator<<(std::ostream& os, const Fbitset<N, L, E>& fs)
{
    os << "Fbitset<" << N << ">{count=" << fs.count() << "}";
    return os;
}
}

#include <catch2/catch_test_macros.hpp>

using namespace libparenth;

using Dim = size_t;
using P = Parenther<Dim>;

/** Reads the cost of the optimal evaluation of the whole problem.
 */

static Dim top_cost(const P::Mem& res, size_t n_factors)
{
    P::Factor_subset all(n_factors, true);
    auto it = res.find(all);
    REQUIRE(it != res.end());
    REQUIRE(!it->second.evals.empty());
    return it->second.evals.front().cost;
}

TEST_CASE("A summation over four factors can be parenthesized")
{
    // s = sum_i x[i] y[i] z[i] w[i], with no external index at all.  Every
    // bipartition of the four factors breaks the single summation, so the
    // partitions shatter into four chunks.  Before the acceleration was
    // restricted to classical problems, every candidate was skipped here and
    // the subproblem was left without any evaluation.
    std::vector<Dim> dims = { 100 };
    std::vector<std::vector<size_t>> factors = { { 0 }, { 0 }, { 0 }, { 0 } };

    P parenther(
        dims.cbegin(), dims.cend(), 1, factors.cbegin(), factors.cend());

    SECTION("The greedy strategy terminates with an evaluation")
    {
        auto res = parenther.opt(Mode::GREEDY, false);
        CHECK(top_cost(res, 4) > 0);
    }

    SECTION("The optimal strategy terminates with an evaluation")
    {
        auto res = parenther.opt(Mode::NORMAL, false);
        CHECK(top_cost(res, 4) > 0);
    }

    SECTION("All strategies agree on the optimal cost")
    {
        P p_normal(
            dims.cbegin(), dims.cend(), 1, factors.cbegin(), factors.cend());
        P p_exhaust(
            dims.cbegin(), dims.cend(), 1, factors.cbegin(), factors.cend());

        auto normal = top_cost(p_normal.opt(Mode::NORMAL, false), 4);
        auto exhaust = top_cost(p_exhaust.opt(Mode::EXHAUST, false), 4);
        CHECK(normal == exhaust);
    }
}

TEST_CASE("A shared summation does not hide the optimal parenthesization")
{
    // t[e] = sum_{i, j} a[i, j, e] b[i, j] c[i, j] d[j].  Both summations are
    // involved by more than two factors, so the optimal parenthesization is
    // only reachable through a partition that the acceleration used to skip.
    std::vector<Dim> dims = { 10, 20, 30 };
    std::vector<std::vector<size_t>> factors
        = { { 0, 1, 2 }, { 0, 1 }, { 0, 1 }, { 1 } };

    P p_normal(dims.cbegin(), dims.cend(), 2, factors.cbegin(), factors.cend());
    P p_exhaust(
        dims.cbegin(), dims.cend(), 2, factors.cbegin(), factors.cend());

    auto normal = top_cost(p_normal.opt(Mode::NORMAL, false), 4);
    auto exhaust = top_cost(p_exhaust.opt(Mode::EXHAUST, false), 4);
    CHECK(normal == exhaust);
}
