#include "pch.h"
#include "Trajectory.h"
#include <cmath>


static const double GRAVITY = -980.665; // in centimeters per second



Trajectory::Trajectory(void)
:launch_height_(0.0)
,horzontal_velocity_(0.0)
,vertical_velocity_(0.0)
,initial_velocity_(0.0)
,initial_theta_(0.0)
{
    UpdateCalculatedValues();
}

Trajectory::Trajectory(double hVelocity, double vVelocity)
:launch_height_(0.0)
,horzontal_velocity_(hVelocity)
,vertical_velocity_(vVelocity)
{
    initial_velocity_ = CalcVelocity(hVelocity, vVelocity);
    initial_theta_ = CalcTheta(hVelocity, vVelocity);
    UpdateCalculatedValues();
}

Trajectory::~Trajectory(void)
{
}


/**
 * Create a Trajectory that travels a given distance over a given time
 * @param double time - the number of second the trip will take
 * @param double distance - the number of meters to cover horizontaly
 *
 * @return Trajectory* - a pointer to a new trajectory object that follows given path
 */
Trajectory* Trajectory::BuildTrajectoryFromTimeDistance(double time, double distance)
{
    
     double vVelocity(0.0);
     double hVelocity(0.0);
     if(0.0 != time)
     {
        vVelocity = GRAVITY / (time / 2.0); 
        if(0.0 != distance)
        {
            hVelocity = distance / time;
        }
     }
     return new Trajectory(vVelocity, hVelocity);
}

/**
 * Calculate the horizontal velocity given the vector of the object
 *
 * @param velocity - the velocity of the object vector
 * @param theta    - the angle of the vector
 * @return The horizontal velocity of the object
 */
double Trajectory::CalcHorizontalVelocity(double velocity, double theta) 
{
    double hVelocity = 0.0;
    if (0.0 != velocity) 
    {
        hVelocity = velocity * std::cos(theta);
    }
    return hVelocity;
}

/**
 * Calculate the vertical velocity given the vector of the object
 *
 * @param velocity - the velocity of the object vector
 * @param theta    - the angle of the vector
 * @return The vertical velocity of the object
 */
double Trajectory::CalcVerticalVelocity(double velocity, double theta) 
{
    double verticalVelocity = 0.0;
    if (0.0 != velocity) 
    {
        verticalVelocity = velocity * std::sin(theta);
    }
    return verticalVelocity;
}

/**
 * Calculate the vector's velocity from the  horizontal and vertical velocities
 *
 * @return - the velocity of the projectile
 */
double Trajectory::CalcVelocity(double hVelocity, double vVelocity) 
{
    double velocity = 0.0;
    if (hVelocity != 0.0 && vVelocity != 0.0) 
    {
        if (vVelocity == 0.0) 
        {
            velocity = hVelocity;
        }
        else if (hVelocity == 0.0) 
        {
            velocity = vVelocity;
        }
        else 
        {
            // initial speed is the hypotenuse of the horizontal and vertical velocities
            velocity = std::sqrt((hVelocity * hVelocity) + (vVelocity * vVelocity));
        }
    }
    return velocity;
}

/**
 * Calculate the angle of theta from the horizontal and vertical velocities
 *
 * @return - the angle of theta in radians
 */
double Trajectory::CalcTheta(double hVelocity, double vVelocity) 
{
    return std::atan(vVelocity / hVelocity);
}

/**
 * Calculate the maximum height of the trajectory given the vertical velocity and initial height of the object
 * @param vVelocity - the vertical velocity of the object
 * @param initY - the initial height of the object
 * @return the maximum height the object will reach
 */
double Trajectory::CalcRawHeight(double vVelocity, double initY) 
{
    double ret = vVelocity;
    // the initial height is not zero take this into account, otherwise we can optimize the calculation
    if (0.0 != initY) 
    {
        ret *= vVelocity;               // ret = vertical velicity squared
        ret += 2 * GRAVITY * initY;
        ret = std::sqrt(ret);
    }
    ret += vVelocity;
    return ret;
}

/**
 * Calculate the Horizontal distance traveled in the trajectory
 * @param hVelocity - The initial horizontal velocity
 * @param vVelocity - The initial vertical velocity
 * @param initY - The initial vertical height of the object
 * @return The horizontal distance cover by the object
 */
double Trajectory::CalcRange(double hVelocity, double vVelocity, double initY) 
{
    double range = hVelocity;
    if (0.0 != range) 
    {
        range /= GRAVITY;
        range *= CalcRawHeight(vVelocity, initY);
    }
    return range;
}

/**
 * Calculate the time to reach the maximum height of the trajectory
 * @param vVelocity - the vertical velocity of the object
 * @return the time in seconds to the trajectories peak
 */
double Trajectory::CalcTimeToMaxHeight(double vVelocity) 
{
    // time = -Vv / g
    return -vVelocity / GRAVITY;
}

/**
 * Calculate the height of the object at a given time
 * @param vVelocity - the vertical velocity of the object
 * @param curTime - the time to calculate the height at
 * @return the height of the object in centimeters
 */
double Trajectory::CalcHeightAtTime(double vVelocity, double curTime) 
{
    double height = 0.0;
    if (0.0 != curTime) 
    {
        height = (GRAVITY * (curTime * curTime))/2 + (vVelocity * curTime);
    }
    return height;
}

/**
 * Calculate the height of the object at its peak
 * @param vVelocity - the vertical velocity of the object
 * @return the height of the object at its peak in centimeters
 */
double Trajectory::CalcHeight(double vVelocity) 
{
    return CalcHeightAtTime(vVelocity, CalcTimeToMaxHeight(vVelocity));
}

/**
 * Calculate the total time of the flight
 * @param vVelocity - the vertical velocity of the object
 * @param initY - the initial height of the object
 * @return the number of seconds until the full trajectory completes
 */
double Trajectory::CalcFlightTime(double vVelocity, double initY) 
{
    return std::abs(CalcRawHeight(vVelocity, initY) / GRAVITY);
}

/**
 * Provides a single call location to update all purely calculated values
 */
void Trajectory::UpdateCalculatedValues() 
{
    max_height_ = CalcHeight(GetVerticalVelocity());
    range_ = CalcRange(GetHorizontalVelocity(), GetVerticalVelocity(), GetLaunchHeight());
    flight_time_ = CalcFlightTime(GetVerticalVelocity(), GetLaunchHeight());
}

/**
 * Set the initial horizontal velocity
 *
 * @param velocity - the initial horizontal velocity in centimeters per second
 */
void Trajectory::SetHorizontalVelocity(double velocity) 
{
    horzontal_velocity_ = velocity;
    initial_velocity_ = CalcVelocity(horzontal_velocity_, vertical_velocity_);
    initial_theta_ = CalcTheta(horzontal_velocity_, vertical_velocity_);
    UpdateCalculatedValues();
}

/**
 * Set the initial vertical velocity of the object
 * @param velocity - - the initial vertical velocity in centimeters per second
 */
void Trajectory::SetVerticalVelocity(double velocity) 
{
    vertical_velocity_ = velocity;
    initial_velocity_ = CalcVelocity(horzontal_velocity_, vertical_velocity_);
    initial_theta_ = CalcTheta(horzontal_velocity_, vertical_velocity_);
    UpdateCalculatedValues();
}

/**
 * Set the initial velocity of the object
 * @param velocity - the total velocity in centimeters per second
 */
void Trajectory::SetInitialVelocity(double velocity) 
{
    initial_velocity_ = velocity;
    horzontal_velocity_ = CalcHorizontalVelocity(initial_velocity_, initial_theta_);
    vertical_velocity_ = CalcVerticalVelocity(initial_velocity_, initial_theta_);
    UpdateCalculatedValues();
}

/**
 * Set the initial angle of the trajectory
 * @param theta - the angle of the vector
 */
void Trajectory::SetInitialTheta(double theta) 
{
    initial_theta_ = theta;
    horzontal_velocity_ = CalcHorizontalVelocity(initial_velocity_, initial_theta_);
    vertical_velocity_ = CalcVerticalVelocity(initial_velocity_, initial_theta_);
    UpdateCalculatedValues();
}

/**
 * Get the vertical velocity at a given time
 * @param curTime - the time to calculate the vertical velocity at
 * @return The vertical velocity at the given time in the trajectory
 */
double Trajectory::GetCurrentVerticalVelocity(double curTime)
{
    return GetVerticalVelocity() - (GRAVITY * curTime);
}

/**
 * Get the horizontal distance traveled at the given time
 *
 * @param curTime - the number of seconds elapsed since start of action
 * @return - distance traveled by the projectile at the given time
 */
double Trajectory::GetDistanceAtTime(double curTime) 
{
    // this the full equation that should be used to show a slowdown in acceleration of the prop over time.
    return GetHorizontalVelocity() * curTime;
}

/**
 * Get the vertical height of the projectile at the given time
 *
 * @param curTime - the number of seconds elapsed since start of action
 * @return - height of the projectile at the given time
 */
double Trajectory::GetHeightAtTime(double curTime)
{
    double height = GetLaunchHeight();
    if (0.0 != curTime)
    {
        height += CalcHeightAtTime(GetVerticalVelocity(), curTime);
    }
    return height;
}

/**
 * Get the angle of trajectory at the given time
 * @param curTime - the number of seconds elapsed since start of action
 * @return The angle of trajectory at the given time in radians
 */
double Trajectory::GetCurrentTheta(double curTime)
{
    return CalcTheta(GetHorizontalVelocity(), GetCurrentVerticalVelocity(curTime));
}

/**
 * Get the velocity of trajectory at the given time
 * @param curTime - the number of seconds elapsed since start of action
 * @return The velocity of the object at the given time in cm/s
 */
double Trajectory::GetCurrentVelocity(double curTime)
{
    return CalcVelocity(GetHorizontalVelocity(), GetCurrentVerticalVelocity(curTime));
}


std::ostream& operator<<(std::ostream& stream, const Trajectory& trajectory)
{
    std::streamsize width = stream.width(8);
    std::streamsize prescision = stream.precision(3);
    stream << "Initial launch height: " << trajectory.GetLaunchHeight();
    stream << "Velocity: " << trajectory.GetInitialVelocity() << "cm/s, "; 
    stream << "Angle: " << trajectory.GetInitialTheta() << "radians, ";
    stream << "Vertical Velocity: " << trajectory.GetVerticalVelocity()<< "cm/s, "; 
    stream << "Horizontal Velocity: " << trajectory.GetHorizontalVelocity() << "cm/s, ";
    stream << "Maximum Height: " << trajectory.GetMaxHeight() << " cm, ";
    stream << "Range: " << trajectory.GetRange() << "cm,";
    stream << "Flight Time: " << trajectory.GetFlightTime() << "seconds";

    stream.width(width);
    stream.precision(prescision);

    return stream;
}
