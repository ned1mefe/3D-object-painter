#include "BinaryHeap.h"

BinaryHeap::BinaryHeap()
{
    elements.resize(1); //makes the vector start from 1
}


bool BinaryHeap::Add(int uniqueId, double weight)
{
    if ( exists(uniqueId) ) return false;
    
    HeapElement newElement;
    newElement.uniqueId = uniqueId;
    newElement.weight = weight;

    
    elements.push_back(newElement);
    
    bubbleUp(elements.size()-1);

    return true;
}

bool BinaryHeap::PopHeap(int& outUniqueId, double& outWeight)
{
    if (HeapSize() == 0) return false;

    outUniqueId = elements[1].uniqueId;
    outWeight = elements[1].weight;

    elements[1] = elements[HeapSize()];

    bubbleDown(1);

    elements.pop_back();
    return true;
}

bool BinaryHeap::ChangePriority(int uniqueId, double newWeight)
{
    int i;
        
    if(!exists(uniqueId)) return false;
    
    for (i = 1; i < elements.size(); i++) 
        if (elements[i].uniqueId == uniqueId) break; 

    if (newWeight < elements[i].weight)
    {
        elements[i].weight = newWeight;
        bubbleUp(i);
    }

    if (newWeight > elements[i].weight)
    {
        elements[i].weight = newWeight;
        bubbleDown(i);
    }
    return true;
}

int BinaryHeap::HeapSize() const
{
    return elements.size() - 1;
}