#pragma once

#include <vector>

struct HeapElement
{
    int     uniqueId;
    double  weight;
};

class BinaryHeap
{
    private:
    std::vector<HeapElement> elements;

    bool exists(int id) const
    {
        for (int i = 1; i < elements.size(); i++) 
            if (elements[i].uniqueId == id) return true;
        return false;
    }

    void bubbleUp(int hole)
    {
        elements[0] = elements[hole];
        
        while(elements[0].weight < elements[hole/2].weight)
        {
            elements[hole] = elements[hole/2];
            hole /= 2;
        }
        elements[hole] = elements[0];
    }

    void bubbleDown(int hole)
    {
        int child = hole * 2;

        elements[0] = elements[hole];
        
        while(child <= elements.size()-1)
        {
            if((child + 1 <= elements.size()-1) && (elements[child+1].weight < elements[child].weight))
            {
                child++;
            }

            if (elements[child].weight < elements[0].weight)
            {
                elements[hole] = elements[child];
                hole = child;
                child = hole * 2;
            }
            else break;
        }
        elements[hole] = elements[0];
    }

    // Do not remove this the tester will utilize this
    // to access the private parts.
    friend class HW3Tester;

    protected:
    public:
        // Constructors & Destructor
                            BinaryHeap();
        //
        bool                Add(int uniqueId, double weight);
        bool                PopHeap(int& outUniqueId, double& outWeight);
        bool                ChangePriority(int uniqueId, double newWeight);
        int                 HeapSize() const;
};