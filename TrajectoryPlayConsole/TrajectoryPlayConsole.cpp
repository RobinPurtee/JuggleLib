// TrajectoryPlayConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Trajectory.h"


int _tmain(int argc, _TCHAR* argv[])
{
    const double flightTime = 0.65;
    const double handDistance = 50.0;

    Trajectory* trajectory = Trajectory::BuildFromTimeDistance(flightTime, handDistance);

    std::cout.precision(8);

    std::cout << *trajectory;

    double curHeight;
    double curDistance;

    for(double t = 0.0 ; t <= flightTime ; t += 0.03)
    {
        curHeight = trajectory->GetHeightAtTime(t);
        curDistance = trajectory->GetDistanceAtTime(t);
        std::cout << "At Time: " << t << " the Height is: " << curHeight << " and distance is: " << curDistance << std::endl;
    }
    return 0;
}

