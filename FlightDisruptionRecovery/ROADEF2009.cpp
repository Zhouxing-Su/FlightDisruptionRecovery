#include "ROADEF2009.h"

namespace ROADEF2009
{
    void clearLine( std::ifstream &fin, char delimiter )
    {
        fin.ignore( MAX_LINE_LEN, delimiter );
    }

    bool isEndOfFile( std::ifstream &fin )
    {
        return ((fin.eof()) || (fin.peek() == '#'));
    }

    FDR readInstance( const std::string& instDir )
    {
        char cbuf[MAX_LINE_LEN + 1];

        // settings
        FDR::Settings settings;
        std::ifstream settingsFile( instDir + SettingsFileName );
        {
            settingsFile >> settings.recoveryPeriod.startDate
                >> settings.recoveryPeriod.startTime
                >> settings.recoveryPeriod.endDate
                >> settings.recoveryPeriod.endTime;
            clearLine( settingsFile );
        }
        {
            settingsFile.getline( cbuf, MAX_LINE_LEN );
            FDR::Settings::DelayCost delayCost;
            std::istringstream ss( cbuf );
            while (ss >> delayCost.cabin >> delayCost.itinType >> delayCost.costPerMinute) {
                settings.delayCost.push_back( delayCost );
            }
        }
        {
            settingsFile.getline( cbuf, MAX_LINE_LEN );
            FDR::Settings::CancellationCost cancellOutboundCost;
            std::istringstream ss( cbuf );
            while (ss >> cancellOutboundCost.cabin >> cancellOutboundCost.itinType >> cancellOutboundCost.cost) {
                settings.cancellOutboundCost.push_back( cancellOutboundCost );
            }
        }
        {
            settingsFile.getline( cbuf, MAX_LINE_LEN );
            FDR::Settings::CancellationCost cancellInboundCost;
            std::istringstream ss( cbuf );
            while (ss >> cancellInboundCost.cabin >> cancellInboundCost.itinType >> cancellInboundCost.cost) {
                settings.cancellInboundCost.push_back( cancellInboundCost );
            }
        }
        {
            settingsFile.getline( cbuf, MAX_LINE_LEN );
            FDR::Settings::DowngradeCost downgradeCost;
            std::istringstream ss( cbuf );
            while (ss >> downgradeCost.referenceCabin >> downgradeCost.actualCabin >> downgradeCost.itinType >> downgradeCost.cost) {
                settings.downgradeCost.push_back( downgradeCost );
            }
        }
        {
            settingsFile >> settings.violateFamilyCost >> settings.violateModelCost >> settings.violateConfigCost;
            settingsFile >> settings.a >> settings.b >> settings.r;
        }
        settingsFile.close();

        // airport capacity
        std::map<FDR::Airport, FDR::AirportsCapacity> airportsCapacityMap;
        std::ifstream airportCapacityFile( instDir + AirportCapacityFileName );

        while (!isEndOfFile( airportCapacityFile )) {
            FDR::Airport airport;
            FDR::AirportsCapacity airportsCap;
            FDR::AirportsCapacityWithinPeriod capacity;

            airportCapacityFile.getline( cbuf, MAX_LINE_LEN );
            std::istringstream ss( cbuf );
            ss >> airport;
            while (ss >> capacity.DepPerHour >> capacity.ArrPerHour
                >> capacity.startTime >> capacity.endTime) {
                airportsCap.push_back( capacity );
            }
            airportsCapacityMap[airport] = airportsCap;
        }

        airportCapacityFile.close();

        // distance
        FDR::DistList distList;
        std::ifstream distanceFile( instDir + DistanceFileName );

        while (!isEndOfFile( distanceFile )) {
            FDR::Dist dist;
            distanceFile >> dist.origin >> dist.destination >> dist.distance.duration >> dist.distance.itinType;
            distList.push_back( dist );
            clearLine( distanceFile );
        }

        distanceFile.close();

        // flights
        FDR::FlightMap flightMap;
        std::ifstream flightFile( instDir + FlightFileName );

        while (!isEndOfFile( flightFile )) {
            int id;
            FDR::Flight flight;
            flightFile >> id >> flight.origin >> flight.destination
                >> flight.depTime >> flight.arrTime >> flight.prevFlight;
            flightMap[id] = flight;
            clearLine( flightFile );
        }

        flightFile.close();

        // aircrafts
        FDR::AircraftInfoMap aircraftInfoMap;
        std::ifstream aircraftFile( instDir + AircraftFileName );

        while (!isEndOfFile( aircraftFile )) {
            FDR::Aircraft aircraft;
            FDR::AircraftInfo aircraftInfo;
            aircraftFile >> aircraft >> aircraftInfo.model
                >> aircraftInfo.family >> aircraftInfo.config
                >> aircraftInfo.endurance >> aircraftInfo.costPerHour
                >> aircraftInfo.turnRound >> aircraftInfo.transit
                >> aircraftInfo.origin >> aircraftInfo.maintenance;
            aircraftInfoMap[aircraft] = aircraftInfo;
            clearLine( aircraftFile );
        }

        aircraftFile.close();

        // rotations
        FDR::RotationMap rotationMap;
        std::ifstream rotationFile( instDir + RotationFileName );

        while (!isEndOfFile( rotationFile )) {
            FDR::Rotation rotation;
            FDR::Aircraft aircraft;
            rotationFile >> rotation.flightID >> rotation.depDate >> aircraft;
            rotationMap[rotation] = aircraft;
            clearLine( rotationFile );
        }

        rotationFile.close();

        // itinerary
        FDR::ItineraryMap itinMap;
        std::ifstream itineraryFile( instDir + ItineraryFileName );

        while (!isEndOfFile( itineraryFile )) {
            FDR::Itinerary itinerary;
            FDR::ItinID itinID;
            itineraryFile >> itinID >> itinerary.itinChar >> itinerary.unitPrice >> itinerary.customerNum;
            itineraryFile.getline( cbuf, MAX_LINE_LEN );
            std::istringstream ss( cbuf );
            FDR::FlightID flightID;
            FDR::Itinerary::FlightArrange flightArrange;
            while (ss >> flightID >> flightArrange.DepDate >> flightArrange.cabin) {
                itinerary.flightArrangeMap[flightID] = flightArrange;
            }
            itinMap[itinID] = itinerary;
        }

        itineraryFile.close();

        // aircraft position
        FDR::RequiredAircraftPositionMap requiredAircraftPositionMap;
        std::ifstream aircraftPositioningFile( instDir + AircraftPositioningFileName );

        while (!isEndOfFile( aircraftPositioningFile )) {
            FDR::Airport airport;
            aircraftPositioningFile >> airport;
            aircraftPositioningFile.getline( cbuf, MAX_LINE_LEN );
            std::istringstream ss( cbuf );
            FDR::RequiredAircraftNum requiredAircraftNum;
            FDR::RequiredAircraftNumOfAirport requiredAircraftNumOfAirport;
            while (ss >> requiredAircraftNum.model >> requiredAircraftNum.config >> requiredAircraftNum.num) {
                requiredAircraftNumOfAirport.push_back( requiredAircraftNum );
            }
            requiredAircraftPositionMap[airport] = requiredAircraftNumOfAirport;
        }

        aircraftPositioningFile.close();

        // flight disruption
        FDR::AltFlightList altFlightList;
        std::ifstream flightDisruptionFile( instDir + FlightDisruptionFileName );

        while (!isEndOfFile( flightDisruptionFile )) {
            FDR::AltFlight altFlight;
            flightDisruptionFile >> altFlight.flight >> altFlight.depDate >> altFlight.delay;
            altFlightList.push_back( altFlight );
            clearLine( flightDisruptionFile );
        }

        flightDisruptionFile.close();

        // alt aircraft
        FDR::AltAircraftList altAircraftList;
        std::ifstream aircraftDisruptionFile( instDir + AircraftDisruptionFileName );

        while (!isEndOfFile( aircraftDisruptionFile )) {
            FDR::AltAircraft altAircraft;
            aircraftDisruptionFile >> altAircraft.aircraft >> altAircraft.startDate
                >> altAircraft.startTime >> altAircraft.endDate >> altAircraft.endTime;
            altAircraftList.push_back( altAircraft );
        }

        aircraftDisruptionFile.close();

        // alt airport
        FDR::AltAirportList altAirportList;
        std::ifstream airportDisruptionFile( instDir + AirportDisruptionFileName );

        while (!isEndOfFile( airportDisruptionFile )) {
            FDR::AltAirport altAirport;
            airportDisruptionFile >> altAirport.airport
                >> altAirport.startDate >> altAirport.startTime
                >> altAirport.endDate >> altAirport.endTime
                >> altAirport.depPerHour >> altAirport.arrPerHour;
            altAirportList.push_back( altAirport );
        }

        airportDisruptionFile.close();

        return FDR( settings, airportsCapacityMap, distList, flightMap,
            aircraftInfoMap, rotationMap, itinMap, requiredAircraftPositionMap,
            altFlightList, altAircraftList, altAirportList );
    }
}