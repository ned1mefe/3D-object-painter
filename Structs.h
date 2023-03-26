#pragma once

#include <cmath>

struct IdPair
{
    int vertexId0;
    int vertexId1;
};

struct Double3
{
    double x, y, z;

    static double Distance(Double3 left, Double3 right);
};

inline
double Double3::Distance(Double3 left, Double3 right)
{
    double xsqr = (left.x - right.x) * (left.x - right.x);
    double ysqr = (left.y - right.y) * (left.y - right.y);
    double zsqr = (left.z - right.z) * (left.z - right.z);

    return std::sqrt(xsqr + ysqr + zsqr);
}