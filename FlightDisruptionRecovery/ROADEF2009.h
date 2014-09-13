/**
*   Usage:
*       handle ROADEF 2009 instances.
*
*   Parameters:
*
*   Problems:
*       1.
*
**/


#ifndef ROADEF2009_H


#include <string>
#include <iostream>
#include <sstream>

#include "FlightDisruptionRecovery.h"

namespace ROADEF2009
{
    static const int MAX_LINE_LEN = 4000;

    const std::string SettingsFileName = "config.csv";
    const std::string AirportCapacityFileName = "airports.csv";
    const std::string DistanceFileName = "dist.csv";
    const std::string FlightFileName = "flights.csv";
    const std::string AircraftFileName = "aircraft.csv";
    const std::string RotationFileName = "rotations.csv";
    const std::string ItineraryFileName = "itineraries.csv";
    const std::string AircraftPositioningFileName = "position.csv";
    const std::string FlightDisruptionFileName = "alt_flights.csv";
    const std::string AircraftDisruptionFileName = "alt_aircraft.csv";
    const std::string AirportDisruptionFileName = "alt_airports.csv";
    const std::string RotationSolutionFileName = "_sol_rotations.csv";
    const std::string ItinerarySolutionFileName = "_sol_itineraries.csv";

    void clearLine( std::ifstream &fin, char delimiter = '\n' )
    {
        fin.ignore( MAX_LINE_LEN, delimiter );
    }

    // must being used in the start of a line
    // ( you can call clearLine() to make sure of it )
    bool isEndOfFile( std::ifstream &fin )
    {
        return ((fin.eof()) || (fin.peek() == '#'));
    }

    // the instDir must end with '/'
    void readInstance( const std::string& instDir )
    {
        char cbuf[MAX_LINE_LEN + 1];

        // settings
        FDR::Settings cfg;
        std::ifstream settingsFile( instDir + SettingsFileName );
        {
            settingsFile >> cfg.recoveryPeriod.startDate
                >> cfg.recoveryPeriod.startTime
                >> cfg.recoveryPeriod.endDate
                >> cfg.recoveryPeriod.endTime;
            clearLine( settingsFile );
        }
        {
            settingsFile.getline( cbuf, MAX_LINE_LEN );
            FDR::Settings::DelayCost delayCost;
            std::istringstream ss( cbuf );
            while (ss >> delayCost.cabin >> delayCost.itinType >> delayCost.costPerMinute) {
                cfg.delayCost.push_back( delayCost );
            }
        }
        {
            settingsFile.getline( cbuf, MAX_LINE_LEN );
            FDR::Settings::CancellationCost cancellOutboundCost;
            std::istringstream ss( cbuf );
            while (ss >> cancellOutboundCost.cabin >> cancellOutboundCost.itinType >> cancellOutboundCost.cost) {
                cfg.cancellOutboundCost.push_back( cancellOutboundCost );
            }
        }
        {
            settingsFile.getline( cbuf, MAX_LINE_LEN );
            FDR::Settings::CancellationCost cancellInboundCost;
            std::istringstream ss( cbuf );
            while (ss >> cancellInboundCost.cabin >> cancellInboundCost.itinType >> cancellInboundCost.cost) {
                cfg.cancellInboundCost.push_back( cancellInboundCost );
            }
        }
        {
            settingsFile.getline( cbuf, MAX_LINE_LEN );
            FDR::Settings::DowngradeCost downgradeCost;
            std::istringstream ss( cbuf );
            while (ss >> downgradeCost.referenceCabin >> downgradeCost.actualCabin >> downgradeCost.itinType >> downgradeCost.cost) {
                cfg.downgradeCost.push_back( downgradeCost );
            }
        }
        {
            settingsFile >> cfg.violateFamilyCost >> cfg.violateModelCost >> cfg.violateConfigCost;
            settingsFile >> cfg.a >> cfg.b >> cfg.r;
        }
        settingsFile.close();

        // airport capacity
        std::map<FDR::Airport, FDR::AirportsCapacity> airportsCapacity;
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
            airportsCapacity[airport] = airportsCap;
        }

        airportCapacityFile.close();

        // distance
        FDR::DistList distList;
        std::ifstream distanceFile( instDir + DistanceFileName );

        while (!isEndOfFile( distanceFile )) {
            FDR::Dist dist;
            distanceFile >> dist.origin >> dist.destination >> dist.duration >> dist.itinType;
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
        FDR::RotationList rotationList;
        std::ifstream rotationFile( instDir + RotationFileName );

        while (!isEndOfFile( rotationFile )) {
            FDR::Rotation rotation;
            rotationFile >> rotation.flightID >> rotation.depDate >> rotation.aircraft;
            rotationList.push_back( rotation );
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
        FDR::AircraftPositionMap aircraftPositionMap;
        std::ifstream aircraftPositioningFile( instDir + AircraftPositioningFileName );

        while (!isEndOfFile( aircraftPositioningFile )) {
            FDR::Airport airport;
            aircraftPositioningFile >> airport;
            aircraftPositioningFile.getline( cbuf, MAX_LINE_LEN );
            std::istringstream ss( cbuf );
            FDR::AircraftNum aircraftNum;
            FDR::AircraftNumOfAirport aircraftNumOfAirport;
            while (ss >> aircraftNum.model >> aircraftNum.config >> aircraftNum.num) {
                aircraftNumOfAirport.push_back( aircraftNum );
            }
            aircraftPositionMap[airport] = aircraftNumOfAirport;
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
    }
}


#define ROADEF2009_H
#endif