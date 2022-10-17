#pragma once
#include <VL53L1X.h>


class TOFDistanceSensor
{
private:
    VL53L1X sensor;
    bool m_isInitialized = false;

public:
    TOFDistanceSensor()
    {

    }

    void Initialize()
    {
        sensor.setTimeout(500);
        if (!sensor.init())
        {
            Serial.println("Failed to detect and initialize sensor!");
        }
        else
        {
            m_isInitialized = true;
            
            //TODO: BAD CODE - fix later !!!!
            VL53L1X::DistanceMode distanceMode = VL53L1X::DistanceMode(chassis_defines::VL53L1X_DISTANCE_MODE);
            sensor.setDistanceMode(distanceMode);
            // !!!!!!!
            
            sensor.setMeasurementTimingBudget(chassis_defines::VL53L1X_TIMING_BUDGET_IN_MICROSECONDS);
            
            // Start continuous readings at a rate of one measurement every 50 ms (the
            // inter-measurement period). This period should be at least as long as the
            // timing budget.
            sensor.startContinuous(chassis_defines::VL53L1X_TIMING_BUDGET_IN_MICROSECONDS/1000);
        }
        
    }


    unsigned int GetDistanceInMilimeters()
    {
        if (m_isInitialized == false)
        {
            return 0;
        }
        //blocking distance read
        auto distance = sensor.read();
        if (sensor.timeoutOccurred()) 
        { 
            Serial.print(" TIMEOUT");
            return 0;
        }

        return distance;
    }


};