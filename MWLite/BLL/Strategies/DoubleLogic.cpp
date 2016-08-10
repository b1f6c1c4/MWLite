#include "DoubleLogic.h"
#include "../../stdafx.h"
#include "StrategyMacros.h"
#include "SingleLogic.h"

class Interval
{
public:
    // ReSharper disable once CppNonExplicitConvertingConstructor
    Interval(int val) : Lb(val), Ub(val) { }

    Interval(int lb, int ub) : Lb(lb), Ub(ub) { }

    int Lb, Ub;

    int Length() const
    {
        return Ub - Lb;
    }

    Interval &operator+=(int offset)
    {
        Lb += offset;
        Ub += offset;
        return *this;
    }

    Interval &operator-=(int offset)
    {
        Lb -= offset;
        Ub -= offset;
        return *this;
    }

    Interval &operator*=(int mul)
    {
        Lb *= mul;
        Ub *= mul;
        return *this;
    }

    Interval &operator+=(const Interval &other)
    {
        Lb += other.Lb;
        Ub += other.Ub;
        return *this;
    }

    Interval &operator-=(const Interval &other)
    {
        Lb -= other.Lb;
        Ub -= other.Ub;
        return *this;
    }

    Interval &operator*=(const Interval &other)
    {
        if (Lb < other.Lb)
            Lb = other.Lb;
        if (Ub < other.Ub)
            Ub = other.Ub;
        return *this;
    }
};

Interval operator+(Interval lhs, const Interval &rhs)
{
    return lhs += rhs;
}

Interval operator-(Interval lhs, const Interval &rhs)
{
    return lhs -= rhs;
}

Interval operator*(Interval lhs, const Interval &rhs)
{
    return lhs *= rhs;
}

bool operator==(const Interval &lhs, const Interval &rhs)
{
    return lhs.Lb == rhs.Lb && lhs.Ub == rhs.Ub;
}

void Strategies::Double::Update(const State &state, BlockSet &Mt, BlockSet &Bt)
{
    for (auto b1 : B)
        for (auto b2 : B)
        {
            if (b1 == b2)
                continue;

            auto sA = U(b1) - U(b2), sB = U(b2) - U(b1), sC = U(b1) * U(b2);

            auto fAB = f(b1), fBC = f(b2);

            auto lA = (sA * Mt).Count(), lB = (sB * Mt).Count(), lC = (sC * Mt).Count();
            auto uA = (sA - Bt).Count(), uB = (sB - Bt).Count(), uC = (sC - Bt).Count();

            Interval rA(lA, uA), rB(lB, uB), rC(lC, uC);

            REPEAT
                  {
                      rA *= fAB - rC;
                      rB *= fBC - rC;
                      rC *= fAB - rA;
                      rC *= fBC - rB;
                  }
                  UNTIL(rA)
                  UNTIL(rB)
                  UNTIL(rC)
                  END_REPEAT

            if (rA.Ub == lA)
                Bt += sA - Mt;
            else if (rA.Lb == uA)
                Mt += sA - Bt;

            if (rB.Ub == lB)
                Bt += sB - Mt;
            else if (rB.Lb == uB)
                Mt += sB - Bt;

            if (rC.Ub == lC)
                Bt += sC - Mt;
            else if (rC.Lb == uC)
                Mt += sC - Bt;
        }
}

void Strategies::DoubleLogic::Update(const State &state)
{
    USE_LOGIC(Single);
    USE_LOGIC(Double);
}

void Strategies::DoubleLogicExtended::Update(const State &state)
{
    USE_LOGIC(Single);
    USE_LOGIC(Extended);
    USE_LOGIC(Double);
}
