#include "FlightDisruptionRecovery.h"

using namespace std;

FDR::FDR( const Settings &s, const AirportsCapacityMap &acm, const DistList &d,
    const FlightMap &fm, const AircraftInfoMap &aim, const RotationMap &rm,
    const ItineraryMap &im, const RequiredAircraftPositionMap &rapm,
    const AltFlightList &afl, const AltAircraftList &aacl, const AltAirportList &aapl )
    : airportCodeList( acm.size() ), airportSerialMap(), settings( s ),
    airportsCapacityMap( acm ), distMat( acm.size(), vector<Distance>( acm.size(), 0 ) ),
    flightMap( fm ), aircraftInfoMap( aim ), rotationMap( rm ), itineraryMap( im ),
    requiredAircraftPositionMap( rapm ), altFlightList( afl ), altAircraftList( aacl ), altAirportList( aapl )
{
    Random::setSeed();
    //Debug<unsigned>::writeln( Random::getSeed() );

    int i = 0;
    for (AirportsCapacityMap::const_iterator iter = acm.begin(); iter != acm.end(); iter++) {
        airportCodeList[i] = iter->first;
        airportSerialMap[iter->first] = i;
    }

    for (DistList::const_iterator iter = d.begin(); iter != d.end(); iter++) {
        int origin = airportSerialMap[iter->origin];
        int destination = airportSerialMap[iter->destination];
        distMat[origin][destination] = iter->distance;
        distMat[destination][origin] = iter->distance;
    }
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

void FDR::solve()
{
    std::ostringstream ss;
    ss << "basic(";
    solvingAlgorithm = ss.str();

    init();

    RangeRand rr( 1, 3 );

    // TODO:

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