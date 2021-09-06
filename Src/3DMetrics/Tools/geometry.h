#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <Eigen/Dense>
#include <vector>

class Geometry
{
public:
    static std::pair<Eigen::Vector3f, Eigen::Vector3f> bestPlaneFromPoints(std::vector<Eigen::Vector3f> & _points);
    static void project3DPointsToPlane(std::pair<Eigen::Vector3f, Eigen::Vector3f> &_plane_coeffs, std::vector<Eigen::Vector3f> &_points, std::vector<Eigen::Vector3f> &_projected_points);
    static double projPointToArea(std::pair<Eigen::Vector3f, Eigen::Vector3f> &_plane_coeffs, std::vector<Eigen::Vector3f> &_projected_points);
};

#endif // GEOMETRY_H
