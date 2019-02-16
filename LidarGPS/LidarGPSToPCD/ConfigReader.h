#pragma once

#include <string>
#include <fstream>
#include <cmath>
#include <algorithm>
#include "Point.h"

class ConfigReader
{
    private:
    std::ifstream configFile;

    std::string lidarFileName, GPSFileName, outputFileName;

    Point shift, boundMax, boundMin;

    double mountingHeight;
    double xOffset, yOffset, zOffset;
    double roll, pitch, yaw;
    
    double mountingAngle;
    double mountingXYDist;

    protected:
    std::string removeAndTrim(std::string str);
    double getValue();
    void calcVals();

    public:
    ConfigReader();

    void read(std::string configFileName);

    std::string getLidarFileName();
    std::string getGPSFileName();
    std::string getOutputFileName();

    double getMountingHeight();
    double getXOffset();
    double getYOffset();
    double getZOffset();
    double getRoll();
    double getPitch();
    double getYaw();

    Point getUpperBounds();
    Point getLowerBounds();
    Point getShift();
    
    double getMountingAngle();
    double getMountingXYDist();

    std::string makeEmptyConfigFile();
};