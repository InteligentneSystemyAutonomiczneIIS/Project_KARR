#pragma once
#include <Servo.h>
#include <Arduino.h>
#include <Ramp.h>
#include <chassis_defines.h>
#include <utility>



class ServosCamera
{
private:
    uint8_t m_PitchControlPin = -1;
    Servo m_pitchServo;
    uint8_t m_YawControlPin = -1;
    Servo m_yawServo;
    bool m_isInitialized = false;
    float m_currentSetSwingPitch = 0.0f;
    float m_currentSetSwingYaw = 0.0f;
    float m_targetSwingPitch = 0.0f;
    float m_targetSwingYaw = 0.0f;

    rampFloat m_pitchRamp;
    rampFloat m_yawRamp;

    bool m_smoothingEnabled = true;
    uint32_t m_smoothingDurationMs = 100;
    uint32_t m_updateIntervalMs = 10;
    ramp_mode m_smoothingMode = ramp_mode::LINEAR;


    float m_SwingConstraintPitchUp = chassis_defines::SERVO_PITCH_SWING_DEFAULT_CONSTRAINT_UP;
    float m_SwingConstraintPitchDown = chassis_defines::SERVO_PITCH_SWING_DEFAULT_CONSTRAINT_DOWN;
    float m_SwingPitchNeutral = chassis_defines::SERVO_PITCH_DEFAULT_POSITION_MICROSECONDS;

    float m_SwingConstraintYawLeft = chassis_defines::SERVO_YAW_SWING_DEFAULT_CONSTRAINT_LEFT;
    float m_SwingConstraintYawRight = chassis_defines::SERVO_YAW_SWING_DEFAULT_CONSTRAINT_RIGHT;
    float m_SwingYawNeutral = chassis_defines::SERVO_YAW_DEFAULT_POSITION_MICROSECONDS;

    // converts from -1 to 1 range into 0-180 range
    int convertSwingToDegrees(float swing)
    {
        // OldRange = (OldMax - OldMin)
        // NewRange = (NewMax - NewMin)
        // NewValue = (((OldValue - OldMin) * NewRange) / OldRange) + NewMin
        int deg = int(((swing - (-1.0f)) * (180 - 0)) / (1.0f - (-1.0f)) );
        return deg;
    }

    float normalizePitch(float swingPitch)
    {
        return constrain(-1.0f * swingPitch, m_SwingConstraintPitchDown, m_SwingConstraintPitchUp);
    }

    float normalizeYaw(float swingYaw)
    {
        return constrain(-1.0f * swingYaw, m_SwingConstraintYawLeft, m_SwingConstraintYawRight);
    }

    void writePitch(float swingPitch)
    {
        m_pitchServo.write(convertSwingToDegrees(swingPitch));
        m_currentSetSwingPitch = swingPitch;
    }

    void writeYaw(float swingYaw)
    {
        m_yawServo.write(convertSwingToDegrees(swingYaw));
        m_currentSetSwingYaw = swingYaw;
    }

    void attachServos()
    {
        m_pitchServo.attach(m_PitchControlPin);
        m_yawServo.attach(m_YawControlPin);
        
        m_pitchServo.writeMicroseconds(m_SwingPitchNeutral);
        m_yawServo.writeMicroseconds(m_SwingYawNeutral);

        m_currentSetSwingPitch = 0.0f;
        m_currentSetSwingYaw = 0.0f;
        m_targetSwingPitch = 0.0f;
        m_targetSwingYaw = 0.0f;

        m_pitchRamp = rampFloat(0.0f);
        m_yawRamp = rampFloat(0.0f);
        m_pitchRamp.setGrain(m_updateIntervalMs);
        m_yawRamp.setGrain(m_updateIntervalMs);

    }

public:


    ServosCamera()
    {
        
    }

    ServosCamera(uint8_t pitchPin, uint8_t yawPin,
                float pitchLimitUp = 1.0f, float pitchLimitDown = -1.0f, int pitchDefaultPosition = 1500,
                float yawLimitLeft = -1.0f, float yawLimitRight = 1.0f, int yawDefaultPosition = 1500)
    {
        Initialize(pitchPin, yawPin, pitchLimitUp, pitchLimitDown, pitchDefaultPosition, yawLimitLeft, yawLimitRight, yawDefaultPosition );
    }

    void Initialize(uint8_t pitchPin, uint8_t yawPin,
                float pitchLimitUp = 1.0f, float pitchLimitDown = -1.0f, int pitchDefaultPosition = 1500,
                float yawLimitLeft = -1.0f, float yawLimitRight = 1.0f, int yawDefaultPosition = 1500)
    {
        m_PitchControlPin = pitchPin;
        m_YawControlPin = yawPin;

        m_SwingConstraintPitchUp = pitchLimitUp;
        m_SwingConstraintPitchDown = pitchLimitDown;
        m_SwingPitchNeutral = pitchDefaultPosition;
        
        m_SwingConstraintYawLeft = yawLimitLeft;
        m_SwingConstraintYawRight = yawLimitRight;
        m_SwingYawNeutral = yawDefaultPosition;

        attachServos();

        m_isInitialized = true;
        
    }

    void SetPitchYaw(float swingPitch, float swingYaw)
    {
        SetPitch(swingPitch);
        SetYaw(swingYaw);

    }

    void Update()
    {
        if (!m_isInitialized)
        {
            return;
        }

        if (!m_smoothingEnabled)
        {
            writePitch(m_targetSwingPitch);
            writeYaw(m_targetSwingYaw);
            return;
        }

        writePitch(m_pitchRamp.update());
        writeYaw(m_yawRamp.update());
    }

    void ConfigureSmoothing(bool enabled, uint32_t durationMs = 60, uint32_t updateIntervalMs = 10)
    {
        m_smoothingEnabled = enabled;
        m_smoothingDurationMs = (durationMs == 0) ? 1 : durationMs;
        m_updateIntervalMs = (updateIntervalMs == 0) ? 1 : updateIntervalMs;
        m_pitchRamp.setGrain(m_updateIntervalMs);
        m_yawRamp.setGrain(m_updateIntervalMs);
    }

    void ConfigureSmoothingMode(ramp_mode mode)
    {
        m_smoothingMode = mode;
    }

    void SetPitch(float swingPitch)
    {
        if (m_isInitialized)
        {
            m_targetSwingPitch = normalizePitch(swingPitch);

            if (!m_smoothingEnabled)
            {
                writePitch(m_targetSwingPitch);
                m_pitchRamp.go(m_targetSwingPitch, 0, NONE);
            }
            else
            {
                m_pitchRamp.go(m_targetSwingPitch, m_smoothingDurationMs, m_smoothingMode);
            }
        }

    }

    void SetYaw(float swingYaw)
    {
        if (m_isInitialized)
        {
            m_targetSwingYaw = normalizeYaw(swingYaw);

            if (!m_smoothingEnabled)
            {
                writeYaw(m_targetSwingYaw);
                m_yawRamp.go(m_targetSwingYaw, 0, NONE);
            }
            else
            {
                m_yawRamp.go(m_targetSwingYaw, m_smoothingDurationMs, m_smoothingMode);
            }
        }

    }

    
    // Send the "zero" speed signal to servo.
    void SetNeutralSwing()
    {
        if (m_isInitialized)
        {
            m_pitchServo.writeMicroseconds(m_SwingPitchNeutral);
            m_yawServo.writeMicroseconds(m_SwingYawNeutral);
            m_currentSetSwingPitch = 0.0f;
            m_currentSetSwingYaw = 0.0f;
            m_targetSwingPitch = 0.0f;
            m_targetSwingYaw = 0.0f;
            m_pitchRamp.go(0.0f, 0, NONE);
            m_yawRamp.go(0.0f, 0, NONE);
        }
    }

    std::pair<float, float> GetCurrentPitchYaw()
    {
        return std::pair{m_currentSetSwingPitch, m_currentSetSwingYaw};
    }
};