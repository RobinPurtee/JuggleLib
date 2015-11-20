#pragma once


class Trajectory
{
public:
    Trajectory(void);
    Trajectory(double hVelocity, double vVelocity);

    ~Trajectory(void);

    /**
     * Get the total distance covered horizontally by the trajectory
     * @return the total horizontal distance of the trajectory
     */
    double GetRange() const {return range_;}

    /**
     * Get the peak height of the trajectory
     * @return the maximum vertical height of the trajectory
     */
    double GetMaxHeight() const {return max_height_;}

    /**
     * Get the total time in flight of the object
     * @return the total time of the flight in seconds
     */
    double GetFlightTime() const {return flight_time_;}

    /**
     * Get the initial horizontal velocity
     *
     * @return - the horizontal velocity in centimeters per second
     */
    double GetHorizontalVelocity() const      {return horzontal_velocity_;}
    void SetHorizontalVelocity(double velocity);

    /**
     * Get the initial vertical velocity
     * @return The initial vertical velocity in centimeters per second
     */
    double GetVerticalVelocity() const {return vertical_velocity_;}
    void SetVerticalVelocity(double velocity);

    /**
     * Get the initial velocity
     *
     * @return - the initial velocity of the toss in centimeters per second
     * This method will either return the previously set value or calculate the
     * value based in the current values of the horizontal and vertical
     * velocities which will become the new value of the velocity property
     */
    double GetInitialVelocity() const {return initial_velocity_;}
    void SetInitialVelocity(double velocity);

    /**
     * Get the initial angle of the trajectory
     *
     * @return - the initial angle of the trajectory in radians
     */
    double GetInitialTheta() const {return initial_theta_;}

    /**
     * Set the initial angle of the trajectory
     * @param theta - the angle of the vector
     */
    void SetInitialTheta(double theta); 


    double GetLaunchHeight() const               {return launch_height_;}
    void SetLaunchHeight(double height)    {launch_height_ = height;}


    double GetCurrentVerticalVelocity(double curTime);
    double GetDistanceAtTime(double curTime); 
    double GetHeightAtTime(double curTime);
    double GetCurrentTheta(double curTime);
    double GetCurrentVelocity(double curTime);

    friend std::ostream& operator<<(std::ostream& stream, const Trajectory& trajectory);
    
private:
    double CalcHorizontalVelocity(double velocity, double theta);
    double CalcVerticalVelocity(double velocity, double theta); 
    double CalcVelocity(double hVelocity, double vVelocity);
    double CalcTheta(double hVelocity, double vVelocity);
    double CalcRawHeight(double vVelocity, double initY); 
    double CalcRange(double hVelocity, double vVelocity, double initY);
    double CalcTimeToMaxHeight(double vVelocity);
    double CalcHeightAtTime(double vVelocity, double curTime);
    double CalcHeight(double vVelocity);
    double CalcFlightTime(double vVelocity, double initY);
    void UpdateCalculatedValues();

    double horzontal_velocity_;      // the initial velocity on the horizontal axis (v * cos theta)
    double vertical_velocity_;      // the initial velocity on the vertical axis (v * sin theta)
    double initial_velocity_;       // the initial velocity of the object (v)
    double initial_theta_;          // the angle of the init vector of the object
    double flight_time_;     // the total flight time of the object
    double range_;          // the total distance traveled by the object
    double max_height_;         // the maximum height of the trajectory
    double launch_height_;      // the height the projectile is launched from 
};

#include <ostream>

std::ostream& operator<<(std::ostream& stream, const Trajectory& trajectory);

