#include <iostream>
#include <fstream>

#include "FlightDisruptionRecovery.h"
#include "ROADEF2009.h"

using namespace std;
using namespace ROADEF2009;


int main()
{
    const string logFileName = "log.csv";
    const string InstanceFileName = "../instance/A01/";

    ofstream csvFile( logFileName );
    FDR fdr = readInstance( InstanceFileName );
    fdr.solve();
    fdr.check();
    fdr.appendResultToSheet( InstanceFileName, csvFile );

    return 0;
}