#include <iostream>
#include <fstream>

#include "FlightDisruptionRecovery.h"
#include "ROADEF2009.h"

using namespace std;
using namespace ROADEF2009;


int main()
{
    FDR fdr = readInstance( "../instance/A01/" );

    return 0;
}