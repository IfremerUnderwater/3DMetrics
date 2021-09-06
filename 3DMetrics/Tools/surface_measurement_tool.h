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

    void endMeasurement(bool _meas_info_is_set=false);
    void cancelMeasurement();
    void removeLastMeasurement();
    void removeMeasurement(int _meas_index);

    int getMeasurementCounter() const;
    double getArea();
    QString getTextFormattedResult();

    void encodeToJSON(QJsonObject & _root_obj);
    void decodeJSON(QJsonObject & _root_obj);

    virtual void onMousePress(Qt::MouseButton _button, int _x, int _y);


protected:
    //template<class Vector3>
    std::pair<Eigen::Vector3f, Eigen::Vector3f> bestPlaneFromPoints(std::vector<Eigen::Vector3f> &_points);
    void project3DPointsToPlane(std::pair<Eigen::Vector3f, Eigen::Vector3f> &_plane_coeffs, std::vector<Eigen::Vector3f> &_points, std::vector<Eigen::Vector3f> &_projected_points);
    double projPointToArea(std::pair<Eigen::Vector3f, Eigen::Vector3f> &_plane_coeffs, std::vector<Eigen::Vector3f> &_projected_points);

private:
    double m_area;

    // Map to keep history of measurements results
    QMap<int, double > m_measurements_area;


};

#endif // SURFACEMEASUREMENTTOOL_H