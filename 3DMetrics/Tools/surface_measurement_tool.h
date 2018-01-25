#ifndef SURFACEMEASUREMENTTOOL_H
#define SURFACEMEASUREMENTTOOL_H

#include "measurement_tool.h"
#include <iostream>
#include <Eigen/Dense>

class SurfaceMeasurementTool : public MeasurementTool
{

public:
    SurfaceMeasurementTool(ToolHandler *_tool_handler);
    ~SurfaceMeasurementTool();
    void draw();
    void removeLastMeasurement();
    void removeMeasurement(int _meas_index);

    int getMeasurementCounter() const;
    double getArea();
    void resetSurfaceData();


protected:
    //template<class Vector3>
    std::pair<Eigen::Vector3f, Eigen::Vector3f> bestPlaneFromPoints(std::vector<Eigen::Vector3f> &_points);
    void project3DPointsToPlane(std::pair<Eigen::Vector3f, Eigen::Vector3f> &_plane_coeffs, std::vector<Eigen::Vector3f> &_points, std::vector<Eigen::Vector3f> &_projected_points);
    double projPointToArea(std::pair<Eigen::Vector3f, Eigen::Vector3f> &_plane_coeffs, std::vector<Eigen::Vector3f> &_projected_points);

private:
    double m_lastNorm;
    double m_area;

};

#endif // SURFACEMEASUREMENTTOOL_H
