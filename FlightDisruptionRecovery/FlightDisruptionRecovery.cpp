#include "FlightDisruptionRecovery.h"

using namespace std;

FDR::FDR()
{
    Random::setSeed();
    Debug<unsigned>::writeln( Random::getSeed() );
}

FDR::~FDR()
{
}


FDR::Output::Output( FDR &fdr, const Solution& s, int iterationCount, int movementCount )
{
    fdr.timer.record();
    duration = fdr.timer.getTotalDuration();

}


void FDR::init()
{
    genInitSolution();
}


void FDR::solve( int maxIterCount, int maxNoImproveCount,
    int tabuTenureAssign, int tabuTenureOpenMedian, int tabuTenureCloseMedian, int randomWalkStep )
{
    MAX_ITER_COUNT = maxIterCount;
    MAX_NO_IMPROVE_COUNT = maxNoImproveCount;
    TABU_TENURE_BASE = tabuTenureAssign;
    PERTURB_STRENGTH = randomWalkStep;

    std::ostringstream ss;
    ss << "basic(";
    solvingAlgorithm = ss.str();

    init();

    RangeRand rr( 1, 3 );

    while (iterCount++ < MAX_ITER_COUNT) {

    }
    Log<>::write( "[optimaReachCount] " );
    Log<int>::writeln( optimaReachCount );
}


void FDR::genInitSolution()
{
    optima = Output( *this, curSln, 0, 0 );
}







bool FDR::check() const
{
    return true;
}






void FDR::printOptima( ostream &os ) const
{
    os << optima.obj << endl;
}


void FDR::initResultSheet( ofstream &csvFile )
{
    csvFile << "Date, Instance, Algorithm, MAX_ITER_COUNT, MAX_NO_IMPROVE_COUNT, RandSeed, Duration, IterCount, MoveCount, Obj., SolutionVector" << std::endl;
}

void FDR::appendResultToSheet( const std::string &instanceFileName, std::ofstream &csvFile ) const
{
    csvFile << Timer::getLocalTime() << ", "
        << instanceFileName << ", "
        << solvingAlgorithm << ", "
        << MAX_ITER_COUNT << ", "
        << MAX_NO_IMPROVE_COUNT << ", "
        << Random::getSeed() << ", "
        << optima.duration << ", "
        << optima.iterCount << ", "
        << optima.moveCount << ", "
        << optima.obj << ", ";

    csvFile << "NULL";

    csvFile << std::endl;
}