#pragma once
#include <vector>
#include <UltrasonicSensor.hpp>
#include <chassis_defines.h>


class SensorArray
{
private:
    std::vector<UltrasonicSensor> sensors;
    bool m_isInitialized = false;


public:
    SensorArray()
    {

    }

    void Initialize()
    {
        InitializeSensors();
        m_isInitialized = true;
    }



private:
    void InitializeSensors()
    {
        sensors.reserve(chassis_defines::ULTRASONIC_NUMBER_OF_SENSORS);
        for (int i = 0; i < chassis_defines::ULTRASONIC_NUMBER_OF_SENSORS; i++)
        {
            sensors.emplace_back(UltrasonicSensor(chassis_defines::ultrasonicTriggerPins[i], 
                                 chassis_defines::ultrasonicEchoPins[i], 
                                 chassis_defines::ULTRASONNIC_MAX_DISTANCE, 
                                 chassis_defines::sensorDescriptions[i],
                                 chassis_defines::sensorOffsetsXYZFromOriginInMilimeters[i] ));

        }

    }







};