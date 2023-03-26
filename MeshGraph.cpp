#include "MeshGraph.h"
#include "BinaryHeap.h"

// For printing
#include <fstream>
#include <iostream>
#include <sstream>

MeshGraph::MeshGraph(const std::vector<Double3>& vertexPositions,
    const std::vector<IdPair>& edges)
{
    int i;

    for (i = 0; i < vertexPositions.size(); i++)
    {
        Vertex newVertex;
        newVertex.id = i;
        newVertex.position3D = vertexPositions[i];

        vertices.push_back(newVertex);
    }

    adjList.resize(i);

    for (int i = 0; i < edges.size(); i++)
    {
        adjList[edges[i].vertexId0].push_back(&vertices[edges[i].vertexId1]);

        adjList[edges[i].vertexId1].push_back(&vertices[edges[i].vertexId0]);

    }
}


// Counts all the edges twice but divides to twice the count again so result is true
double MeshGraph::AverageDistanceBetweenVertices() const
{
    double sum = 0;
    int edgecounter = 0;
    Double3 nothing;

    for (int i = 0; i < adjList.size(); i++)
    {
        for (std::list<Vertex*>::const_iterator j = adjList[i].begin(); j != adjList[i].end(); j++)
        {
            sum += nothing.Distance(vertices[i].position3D, (*j)->position3D);
            edgecounter++;
        }
    }

    if (edgecounter == 0) return 0;

    return sum / (edgecounter);
}

double MeshGraph::AverageEdgePerVertex() const
{
    double vertexcounter, edgecounter = 0;
    for (vertexcounter = 0; vertexcounter < adjList.size(); vertexcounter++)
    {
        edgecounter += adjList[vertexcounter].size();
    }

    if (vertexcounter == 0) return 0;

    return ((edgecounter / 2) / vertexcounter);
}

int MeshGraph::TotalVertexCount() const
{
    return vertices.size();
}

int MeshGraph::TotalEdgeCount() const
{
    int vertexcounter, edgecounter = 0;
    for (vertexcounter = 0; vertexcounter < adjList.size(); vertexcounter++)
    {
        edgecounter += adjList[vertexcounter].size();
    }

    return edgecounter / 2;
}

int MeshGraph::VertexEdgeCount(int vertexId) const
{
    if (vertices.size() <= vertexId) return -1;

    return adjList[vertexId].size();
}

void MeshGraph::ImmediateNeighbours(std::vector<int>& outVertexIds,
    int vertexId) const
{
    outVertexIds.clear();
    if (vertexId >= vertices.size()) return;

    for (std::list<Vertex*>::const_iterator j = adjList[vertexId].begin(); j != adjList[vertexId].end(); j++)
    {
        outVertexIds.push_back((*j)->id);
    }
}

void MeshGraph::PaintInBetweenVertex(std::vector<Color>& outputColorAllVertex,
    int vertexIdFrom, int vertexIdTo,
    const Color& color) const
{
    if (vertexIdTo >= vertices.size()) return;
    if (vertexIdFrom >= vertices.size()) return;

    // arranging output array

    outputColorAllVertex.resize(vertices.size());

    for (int i = 0; i < vertices.size(); i++)
    {
        outputColorAllVertex[i].r = 0;
        outputColorAllVertex[i].b = 0;
        outputColorAllVertex[i].g = 0;
    }

    //finding the shortest way

    std::vector<int> prevTable;
    prevTable.assign(vertices.size(), -1);
    std::vector<double> distTable;
    distTable.assign(vertices.size(), INFINITY);
    distTable[vertexIdFrom] = 0;

    Double3 helper;

    BinaryHeap distanceHeap;
    int outId;
    double outWeight;

    std::vector<int> neighbors;


    for (int i = 0; i < vertices.size(); i++)
        distanceHeap.Add(i, INFINITY);

    distanceHeap.ChangePriority(vertexIdFrom, 0);

    while (distanceHeap.HeapSize())
    {
        distanceHeap.PopHeap(outId, outWeight);
        ImmediateNeighbours(neighbors, outId);

        for (int i = 0; i < neighbors.size(); i++)
        {
            int j = neighbors[i];
            double currDistance = helper.Distance(vertices[j].position3D, vertices[outId].position3D);

            if (currDistance + distTable[outId] < distTable[j])
            {
                distanceHeap.ChangePriority(j, currDistance + outWeight);
                distTable[j] = currDistance + outWeight;
                prevTable[j] = outId;
            }
        }
        neighbors.clear();

        if (outId == vertexIdTo) break;

    }

    // painting the shortest way

    int currId = vertexIdTo;
    while (currId != -1)
    {
        outputColorAllVertex[currId].r = color.r;
        outputColorAllVertex[currId].b = color.b;
        outputColorAllVertex[currId].g = color.g;

        currId = prevTable[currId];
    }

}

void MeshGraph::PaintInRangeGeodesic(std::vector<Color>& outputColorAllVertex,
    int vertexId, const Color& color,
    int maxDepth, FilterType type,
    double alpha) const
{
    if (vertexId >= vertices.size()) return;

    // arranging output array

    outputColorAllVertex.resize(vertices.size());

    for (int i = 0; i < vertices.size(); i++)
    {
        outputColorAllVertex[i].r = 0;
        outputColorAllVertex[i].b = 0;
        outputColorAllVertex[i].g = 0;
    }

    //finding and painting vertices

    std::vector<double> distTable;
    distTable.assign(vertices.size(), INFINITY);
    distTable[vertexId] = 0;

    Double3 helper;

    BinaryHeap depthHeap;
    int outId;
    double outWeight;

    std::vector<int> neighbors;
    std::vector<int> depths;
    depths.resize(vertices.size());
    int currWeight=1;
    int currDepth=1;


    for (int i = 0; i < vertices.size(); i++)
        depthHeap.Add(i, INFINITY);
    depthHeap.ChangePriority(vertexId, 0);
    
    while (depthHeap.HeapSize())
    {
        depthHeap.PopHeap(outId, outWeight);
        if (depths[outId] > maxDepth) break;

        // painting with filters

        if (type == FILTER_GAUSSIAN)
        {
            double xsqr = distTable[outId] * distTable[outId];
            double alphasqr = alpha * alpha;
            double filter = std::exp(-(xsqr / alphasqr));

            outputColorAllVertex[outId].r = color.r * filter;
            outputColorAllVertex[outId].g = color.g * filter;
            outputColorAllVertex[outId].b = color.b * filter;
        }

        if (type == FILTER_BOX)
        {
            if ((distTable[outId] < -alpha) || (distTable[outId] > alpha))
            {
                outputColorAllVertex[outId].r = 0;
                outputColorAllVertex[outId].g = 0;
                outputColorAllVertex[outId].b = 0;
            }
            else
            {
                outputColorAllVertex[outId].r = color.r;
                outputColorAllVertex[outId].g = color.g;
                outputColorAllVertex[outId].b = color.b;
            }
        }

        // adjusting the neighbors

        ImmediateNeighbours(neighbors, outId);
        bubbleSort(neighbors);
        currDepth = depths[outId] +1;

        for (int i = 0; i < neighbors.size(); i++)
        {
            int j = neighbors[i];
            double currDistance = helper.Distance(vertices[j].position3D, vertices[outId].position3D);

            if (distTable[j] == INFINITY)
            {
                depthHeap.ChangePriority(j, ++currWeight);
                distTable[j] = distTable[outId] + currDistance;
                depths[j] = currDepth;
            }
        }

        neighbors.clear();
    }

}

void MeshGraph::PaintInRangeEuclidian(std::vector<Color>& outputColorAllVertex,
    int vertexId, const Color& color,
    int maxDepth, FilterType type,
    double alpha) const
{
    if (vertexId >= vertices.size()) return;

    // arranging output array

    outputColorAllVertex.resize(vertices.size());

    for (int i = 0; i < vertices.size(); i++)
    {
        outputColorAllVertex[i].r = 0;
        outputColorAllVertex[i].b = 0;
        outputColorAllVertex[i].g = 0;
    }

    //finding and painting vertices

    std::vector<double> distTable;
    distTable.assign(vertices.size(), INFINITY);
    distTable[vertexId] = 0;

    Double3 helper;

    BinaryHeap depthHeap;
    int outId;
    double outWeight;

    std::vector<int> neighbors;


    for (int i = 0; i < vertices.size(); i++)
        depthHeap.Add(i, INFINITY);

    depthHeap.ChangePriority(vertexId, 0);
    while (depthHeap.HeapSize())
    {
        depthHeap.PopHeap(outId, outWeight);
        if (outWeight > maxDepth) break;

        // painting with filters

        if (type == FILTER_GAUSSIAN)
        {
            double xsqr = distTable[outId] * distTable[outId];
            double alphasqr = alpha * alpha;
            double filter = std::exp(-(xsqr / alphasqr));

            outputColorAllVertex[outId].r = color.r * filter;
            outputColorAllVertex[outId].g = color.g * filter;
            outputColorAllVertex[outId].b = color.b * filter;
        }

        if (type == FILTER_BOX)
        {
            if ((distTable[outId] < -alpha) || (distTable[outId] > alpha))
            {
                outputColorAllVertex[outId].r = 0;
                outputColorAllVertex[outId].g = 0;
                outputColorAllVertex[outId].b = 0;
            }
            else
            {
                outputColorAllVertex[outId].r = color.r;
                outputColorAllVertex[outId].g = color.g;
                outputColorAllVertex[outId].b = color.b;
            }
        }

        // adjusting the neighbors

        ImmediateNeighbours(neighbors, outId);

        for (int i = 0; i < neighbors.size(); i++)
        {
            int j = neighbors[i];
            double directDistance = helper.Distance(vertices[j].position3D, vertices[vertexId].position3D);

            if (distTable[j] == INFINITY)
            {
                depthHeap.ChangePriority(j, outWeight + 1);
                distTable[j] = directDistance;
            }
        }
        neighbors.clear();
    }

}

void MeshGraph::WriteColorToFile(const std::vector<Color>& colors,
    const std::string& fileName)
{
    // IMPLEMENTED
    std::stringstream s;
    for (int i = 0; i < static_cast<int>(colors.size()); i++)
    {
        int r = static_cast<int>(colors[i].r);
        int g = static_cast<int>(colors[i].g);
        int b = static_cast<int>(colors[i].b);

        s << r << ", " << g << ", " << b << "\n";
    }
    std::ofstream f(fileName.c_str());
    f << s.str();
}

void MeshGraph::PrintColorToStdOut(const std::vector<Color>& colors)
{
    // IMPLEMENTED
    for (int i = 0; i < static_cast<int>(colors.size()); i++)
    {
        std::cout << static_cast<int>(colors[i].r) << ", "
            << static_cast<int>(colors[i].g) << ", "
            << static_cast<int>(colors[i].b) << "\n";
    }
}