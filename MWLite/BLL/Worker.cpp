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
    if (Config->UseTotalMines)
        gen = std::make_shared<TotalGenerator>(Config->Width, Config->Height, Config->TotalMines);
    else
        gen = std::make_shared<ProbGenerator>(Config->Width, Config->Height, Config->Probability);
    if (Config->NotRigorous)
        gen = std::make_shared<NotRigorousGenerator>(gen);

    auto slv = std::make_shared<SolverBuilder>(Logic);

    m_Simulator = std::make_shared<Simulator>(Config, gen, slv, true);
}

size_t Worker::ProcessOne()
{
    return m_Simulator->Simulate(m_Cancel);
}
