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

    void clearLine( std::ifstream &fin, char delimiter = '\n' );

    // must being used in the start of a line
    // ( you can call clearLine() to make sure of it )
    bool isEndOfFile( std::ifstream &fin );

    // the instDir must end with '/'
    FDR readInstance( const std::string& instDir );
}


#define ROADEF2009_H
#endif