#pragma once
#include <NewPing.h>
#include <array>
#include <chassis_defines.h>

class UltrasonicSensor
{
private:
    const NewPing &m_internalSensor;
    unsigned int m_pingIntervalInMiliseconds = chassis_defines::ULTRASONIC_PING_INTERVAL;

    unsigned int m_latestDistance = 0;

public:
    String sensorDescription = "";
    std::array<int,3> positionOffsetXYZFromOriginInMilimeters;




public:

    UltrasonicSensor(uint8_t triggerPin, uint8_t echoPin, unsigned int maxDistanceInCentimeters, String sensorDescription, std::array<int, 3> positionOffsetXYZFromOriginInMilimeters) : 
                                    m_internalSensor(NewPing(triggerPin, echoPin, maxDistanceInCentimeters)),
                                    sensorDescription{sensorDescription}, 
                                    positionOffsetXYZFromOriginInMilimeters {positionOffsetXYZFromOriginInMilimeters}
    {

    }


    void TriggerSensor()
    {

    }

    unsigned int getDistance()
    {

    }


    void setPingInterval(unsigned int pingInterval)
    {
        m_pingIntervalInMiliseconds = pingInterval;
    }


};