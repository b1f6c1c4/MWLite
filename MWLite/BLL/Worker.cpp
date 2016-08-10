#include "Worker.h"
#include "IGenerator.h"
#include "TotalGenerator.h"
#include "ProbGenerator.h"
#include "NotRigorousGenerator.h"
#include "Simulator.h"
#include "Strategies/SolverBuilder.h"

Worker::Worker() { }

void Worker::Prepare()
{
    std::shared_ptr<IGenerator> gen;
    if (m_Config->UseTotalMines)
        gen = std::make_shared<TotalGenerator>(m_Config->Width, m_Config->Height, m_Config->TotalMines);
    else
        gen = std::make_shared<ProbGenerator>(m_Config->Width, m_Config->Height, m_Config->Probability);
    if (m_Config->NotRigorous)
        gen = std::make_shared<NotRigorousGenerator>(gen);

    auto slv = std::make_shared<SolverBuilder>(m_Logic);

    m_Simulator = std::make_shared<Simulator>(m_Config, gen, slv, true);
}

size_t Worker::ProcessOne()
{
    return m_Simulator->Simulate(m_Cancel);
}
