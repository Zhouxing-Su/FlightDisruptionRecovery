/**
*   Usage:
*       solve flight disruption recovery problems.
*
*   Parameters:
*
*   Problems:
*       1. date and time data structure should not simply be string.
*
**/


#ifndef FLIGHT_DISRUPTION_RECOVERY_H
#define FLIGHT_DISRUPTION_RECOVERY_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include "../CPPutilibs/Random.h"
#include "../CPPutilibs/RangeRand.h"
#include "../CPPutilibs/RandSelect.h"
#include "../CPPutilibs/Timer.h"
#include "../CPPutilibs/Log.h"


class FDR
{
public:
    typedef double Unit;    // unit of the objective function

    typedef std::string Airport;    // three-character string corresponding to the IATA three-letter code of an airport
    // character specifying whether the itinerary corresponds to the outbound
    // portion of a trip( A ) and to the inbound or return portion of a trip( R )
    typedef char ItinChar;
    // character specifying the flight type or the itinerary type( D for domestic,
    // C for continental, I for Intercontinental, P for proximity )
    typedef char ItinType;
    typedef char Cabin;     // character specifying the cabin class ( F for first, B for business, E for economy )
    typedef int Duration;   // strictly positive integer specifying a duration in minutes
    typedef Unit Price;     // float specifying a price in euros
    typedef int FlightID;   // unique identification number for flight ( stricly positive integer )
    typedef int ItinID;     // unique identification number for itinerary ( stricly positive integer )

    typedef std::string Model;
    typedef std::string Family;
    struct Config
    {
        static const int MAX_CONFIG_STR_LEN = 50;

        int first;
        int business;
        int economy;

        friend std::istream& operator>>(std::istream &is, Config &config)
        {
            std::string sbuf;
            char cbuf[MAX_CONFIG_STR_LEN];

            is >> sbuf;
            std::stringstream ss( sbuf );

            ss.getline( cbuf, MAX_CONFIG_STR_LEN, '/' );
            std::stringstream numStream( cbuf );
            numStream >> config.first;

            ss.getline( cbuf, MAX_CONFIG_STR_LEN, '/' );
            numStream = std::stringstream( cbuf );
            numStream >> config.business;

            ss >> config.economy;

            return is;
        }
    };

    typedef std::string Date;
    typedef std::string Time;
    // typedef std::string Aircraft;    // simple implementation of aircraft id
    struct Aircraft                     // complex implementation of aircraft id
    {
        static const int MAX_AIRCRAFT_STR_LEN = 100;

        Model model;
        int id;

        friend std::istream& operator>>(std::istream &is, Aircraft &aircraft)
        {
            std::string sbuf;
            char cbuf[MAX_AIRCRAFT_STR_LEN];

            is >> sbuf;
            std::istringstream ss( sbuf );
            ss.getline( cbuf, MAX_AIRCRAFT_STR_LEN, '#' );
            aircraft.model = cbuf;
            ss >> aircraft.id;

            return is;
        }

        bool operator<(const Aircraft &aircraft) const
        {
            return (this->model == aircraft.model) ? (this->id < aircraft.id) : (this->model < aircraft.model);
        }
    };
    struct Maint
    {
        // set remainingFlightHours to -1 to indicate
        // no need for maintenance
        static const int NO_NEED_FOR_MAINTENANCE = -1;
        static const int MAX_MAINT_STR_LEN = 100;

        Airport airport;
        Date startDate;
        Time startTime;
        Date endDate;
        Time endTime;
        Duration remainingFlightHours;

        friend std::istream& operator>>(std::istream &is, Maint &maint)
        {
            std::string sbuf;
            char cbuf[MAX_MAINT_STR_LEN + 1];
            is >> sbuf;
            if (sbuf == "NULL") {
                maint.remainingFlightHours = NO_NEED_FOR_MAINTENANCE;
            } else {
                std::istringstream liness( sbuf );

                liness.getline( cbuf, MAX_MAINT_STR_LEN, '-' );
                std::istringstream gridss( cbuf );
                gridss >> maint.airport;

                liness.getline( cbuf, MAX_MAINT_STR_LEN, '-' );
                gridss = std::istringstream( cbuf );
                gridss >> maint.startDate;

                liness.getline( cbuf, MAX_MAINT_STR_LEN, '-' );
                gridss = std::istringstream( cbuf );
                gridss >> maint.startTime;

                liness.getline( cbuf, MAX_MAINT_STR_LEN, '-' );
                gridss = std::istringstream( cbuf );
                gridss >> maint.endDate;

                liness.getline( cbuf, MAX_MAINT_STR_LEN, '-' );
                gridss = std::istringstream( cbuf );
                gridss >> maint.endTime;

                liness >> maint.remainingFlightHours;
            }
            return is;
        }
    };

    ///= data structures for reading instances
    struct Settings
    {
        struct RecoveryPeriod
        {
            Date startDate;
            Time startTime;
            Date endDate;
            Time endTime;
        };

        struct DelayCost
        {
            Cabin cabin;
            ItinType itinType;
            Price costPerMinute;
        };

        struct CancellationCost
        {
            Cabin cabin;
            ItinType itinType;
            Price cost;
        };

        struct DowngradeCost
        {
            Cabin referenceCabin;
            Cabin actualCabin;
            ItinType itinType;
            Price cost;
        };

        RecoveryPeriod recoveryPeriod;
        std::vector<DelayCost> delayCost;
        std::vector<CancellationCost> cancellOutboundCost;
        std::vector<CancellationCost> cancellInboundCost;
        std::vector<DowngradeCost> downgradeCost;
        Price violateFamilyCost;
        Price violateModelCost;
        Price violateConfigCost;    // cabin config
        Unit a;     // factor in objective function
        Unit b;     // factor in objective function
        Unit r;     // factor in objective function
    };

    struct AirportsCapacityWithinPeriod
    {
        int DepPerHour;
        int ArrPerHour;
        Time startTime;
        Time endTime;
    };
    typedef std::vector<AirportsCapacityWithinPeriod> AirportsCapacity;
    typedef std::map<Airport, AirportsCapacity> AirportsCapacityMap;

    struct Distance
    {
        Duration duration;
        ItinType itinType;

        Distance( Duration d = 0, ItinType i = 0 ) :duration( d ), itinType( i ) {}
    };
    struct Dist
    {
        Airport origin;
        Airport destination;
        Distance distance;
    };
    typedef std::vector<Dist> DistList; // assume it contains n*(n-1) elements for n airports

    struct Flight
    {
        // set prevFlight to 0 to indicate no previous flight
        static const int NO_PREV_FLIGHT = 0;

        Airport origin;
        Airport destination;
        Time depTime;
        Time arrTime;
        int prevFlight;
    };
    typedef std::map<FlightID, Flight> FlightMap;

    struct AircraftInfo
    {
        Model model;
        Family family;
        Config config;
        Duration endurance;
        Price costPerHour;
        Duration turnRound;
        Duration transit;
        Airport origin;
        Maint maintenance;
    };
    typedef std::map<Aircraft, AircraftInfo> AircraftInfoMap;

    struct Rotation
    {
        FlightID flightID;
        Date depDate;

        friend bool operator<(const Rotation& l, const Rotation& r)
        {
            return (l.flightID == r.flightID) ? (l.depDate < r.depDate) : (l.flightID < r.flightID);
        }
    };
    typedef std::map<Rotation, Aircraft> RotationMap;

    struct Itinerary
    {
        struct FlightArrange
        {
            Date DepDate;
            Cabin cabin;
        };

        ItinChar itinChar;
        Price unitPrice;
        int customerNum;
        std::map<FlightID, FlightArrange> flightArrangeMap;
    };
    typedef std::map<ItinID, Itinerary> ItineraryMap;

    struct RequiredAircraftNum
    {
        Model model;
        Config config;
        int num;
    };
    typedef std::vector<RequiredAircraftNum> RequiredAircraftNumOfAirport;
    typedef std::map<Airport, RequiredAircraftNumOfAirport> RequiredAircraftPositionMap;

    struct AltFlight
    {
        static const int FLIGHT_CANCELLED = -1;

        FlightID flight;
        Date depDate;
        Duration delay;
    };
    typedef std::vector<AltFlight> AltFlightList;

    struct AltAircraft
    {
        Aircraft aircraft;
        Date startDate;
        Time startTime;
        Date endDate;
        Time endTime;
    };
    typedef std::vector<AltAircraft> AltAircraftList;

    struct AltAirport
    {
        Airport airport;
        Date startDate;
        Time startTime;
        Date endDate;
        Time endTime;
        int depPerHour;
        int arrPerHour;
    };
    typedef std::vector<AltAirport> AltAirportList;


private:
    struct Solution;
public:
    struct Output
    {
        Output() {}
        Output( FDR &fdr, const Solution& s, int iterationCount, int movementCount );

        Unit obj;

        int iterCount;
        int moveCount;
        double duration;
    };

    FDR( const Settings &settings, const AirportsCapacityMap &airportsCapacityMap,
        const DistList &distList, const FlightMap &flightMap, const AircraftInfoMap &aircraftInfoMap,
        const RotationMap &rotationMap, const ItineraryMap &itineraryMap,
        const RequiredAircraftPositionMap &requiredAircraftPositionMap,
        const AltFlightList &altFlightList, const AltAircraftList &altAircraftList,
        const AltAirportList &altAirportList );
    ~FDR();

    void solve();
    bool check() const;

    void printOptima( std::ostream &os ) const;

    static void initResultSheet( std::ofstream &csvFile );
    void appendResultToSheet( const std::string &instanceFileName, std::ofstream &csvFile ) const;





private:
    ///= given information
    // serialize airports
    std::map<std::string, int> airportSerialMap;
    std::vector<std::string> airportCodeList;

    Settings settings;
    AirportsCapacityMap airportsCapacityMap;
    std::vector<std::vector<Distance> > distMat;
    FlightMap flightMap;
    AircraftInfoMap aircraftInfoMap;
    RotationMap rotationMap;
    ItineraryMap itineraryMap;
    RequiredAircraftPositionMap requiredAircraftPositionMap;
    AltFlightList altFlightList;
    AltAircraftList altAircraftList;
    AltAirportList altAirportList;

    ///= objective value updating
    typedef std::map<Family, int> AircraftNumByFamily;
    typedef std::map<Model, int> AircraftNumByModel;
    typedef std::map<Model, int> AircraftNumByConfig;
    struct AircraftNumOfAirport
    {
        AircraftNumByFamily aircraftNumByFamily;
        AircraftNumByModel aircraftNumByModel;
        AircraftNumByConfig aircraftNumByConfig;
    };
    typedef std::map<Airport, AircraftNumOfAirport> AircraftPositionMap;

    static Price getCancellationLegalCompensation( Duration plannedDuration )
    {
        if (plannedDuration == 0) {
            return 0;
        } else if (plannedDuration < 120) {
            return 250;
        } else if (plannedDuration < 270) {
            return 400;
        } else {
            return 600;
        }
    }

    static Price getDelayLegalCompensation( Duration plannedDuration, Duration delay )
    {
        if (delay > 300) {
            return 75;
        } else if (delay > 240) {
            return 15;
        } else if (delay > 180) {
            if (plannedDuration < 270) {
                return 15;
            }
        } else if (delay > 120) {
            if (plannedDuration < 120) {
                return 15;
            }
        }

        return 0;
    }


    ///= data structures about solution
    struct RotationInfo
    {
        Airport origin;
        Airport destination;
        Time depTime;
        Time arrTime;
        int prevFlight;
        Aircraft aircraft;
    };
    typedef std::map<Rotation, RotationInfo> SlnRotationMap;
    typedef ItineraryMap SlnItineraryMap;

    struct Solution
    {
        Solution() {}
        Solution( FDR const *p ) : fdr( p )
        {
        }

        Unit obj;

        SlnRotationMap slnRotationMap;
        SlnItineraryMap slnItineraryMap;

        FDR const *fdr;
    };


    void init();
    void genInitSolution();


    Solution curSln;

    // solution output and statistic data
    int optimaReachCount;

    int iterCount;  // iteration count of local search or tabu search
    int moveCount;  // total move count of local search or tabu search
    int recoverMoveCount;

    // arguments of algorithm
    int MAX_ITER_COUNT;
    int MAX_NO_IMPROVE_COUNT;
    int TABU_TENURE_BASE;
    int PERTURB_STRENGTH;

    Output optima;
    Timer timer;
    std::string solvingAlgorithm;
};


#endif