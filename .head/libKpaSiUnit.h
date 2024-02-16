#ifndef __H_LIBKPASIUNIT__
#define __H_LIBKPASIUNIT__

// ======== ======== ======== ========
// SI unit postfix define
// ======== ======== ======== ========

#define _kpa_si_unit(si)                                                                \
    constexpr long double operator"" G##si(long double v) { return 1E+9 * v; };         \
    constexpr long double operator"" G##si(unsigned long long v) { return 1E+9 * v; };  \
    constexpr long double operator"" M##si(long double v) { return 1E+6 * v; };         \
    constexpr long double operator"" M##si(unsigned long long v) { return 1E+6 * v; };  \
    constexpr long double operator"" K##si(long double v) { return 1E+3 * v; };         \
    constexpr long double operator"" K##si(unsigned long long v) { return 1E+3 * v; };  \
    constexpr long double operator""##si(long double v) { return v; };                  \
    constexpr long double operator""##si(unsigned long long v) { return v; };           \
    constexpr long double operator"" m##si(long double v) { return 1E-3 * v; };         \
    constexpr long double operator"" m##si(unsigned long long v) { return 1E-3 * v; };  \
    constexpr long double operator"" u##si(long double v) { return 1E-6 * v; };         \
    constexpr long double operator"" u##si(unsigned long long v) { return 1E-6 * v; }   \
    constexpr long double operator"" n##si(long double v) { return 1E-9 * v; };         \
    constexpr long double operator"" n##si(unsigned long long v) { return 1E-9 * v; };  \
    constexpr long double operator"" p##si(long double v) { return 1E-12 * v; };        \
    constexpr long double operator"" p##si(unsigned long long v) { return 1E-12 * v; }; \
    constexpr long double operator"" f##si(long double v) { return 1E-15 * v; };        \
    constexpr long double operator"" f##si(unsigned long long v) { return 1E-15 * v; };

#endif // __H_LIBKPASIUNIT__