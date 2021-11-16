#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iostream>
#include <map>
#include <set>

std::vector<int> convert_to_merge_variable(std::vector<int> literals, int numberOfColors, int numberOfVertices)
{
    std::vector<int> result;
    std::set<int> pre_result;
    std::map<int, std::vector<int> > colorVertexMap;
    for (int ind = 0; ind < literals.size(); ind++)
    {   
        //if it's a merge variables or positive just add it to the return list
        if (std::abs(literals[ind]) > numberOfColors * numberOfVertices || literals[ind]>0)
        {
            pre_result.insert(literals[ind]);
        }
        else if (literals[ind] < 0)
        {
            int color = (std::abs(literals[ind]) % numberOfColors);
            // std::cout << std::to_string(literals[ind]) << "has color" << std::to_string(color) <<", is vertex "<< std::to_string(std::ceil(-1.0*literals[ind]/numberOfColors)) <<std::endl;
            colorVertexMap[color].push_back(literals[ind]);
        }
    }
    if (colorVertexMap.size() == 0)
    {   
        return result;
    }
    std::vector<int> OneOfEachColor;
    // add positive merge variables for vertices with the same color
    std::map<int, std::vector<int> >::iterator it;
    for (it = colorVertexMap.begin(); it != colorVertexMap.end(); it++)
    {   
        int key = it->first;
        std::vector<int> value = it->second;
        int vertexNumj;
        int vertexNumi;
        vertexNumj = std::ceil(-1.0 * value[0] / numberOfColors);
        OneOfEachColor.push_back(value[0]);
        //if (value.size()==1)
        //{
            //pre_result.insert(value[0]);
        //}
        for (int i = 0; i < value.size() - 1; i++)
        {
            vertexNumi = vertexNumj;
            vertexNumj = std::ceil(-1.0 * value[i + 1] / numberOfColors);
            pre_result.insert(-(numberOfColors * numberOfVertices + (std::min(vertexNumi, vertexNumj) - 1) * numberOfVertices + std::max(vertexNumi, vertexNumj)));
        }
    }

    //add negative merge variables for vertices with differnt color
    for (int i = 0; i < OneOfEachColor.size() - 1; i++)
    {
        for (int j = i + 1; j < OneOfEachColor.size(); j++)
        {
            int vertexNumi = std::ceil(-1.0 * OneOfEachColor[i] / numberOfColors);
            int vertexNumj = std::ceil(-1.0 * OneOfEachColor[j] / numberOfColors);
            pre_result.insert(numberOfColors * numberOfVertices + (std::min(vertexNumi, vertexNumj) - 1) * numberOfVertices + std::max(vertexNumi, vertexNumj));
        }
    }

    std::copy(pre_result.begin(), pre_result.end(), std::back_inserter(result));
    return result;
}
/*
int main()
{
    std::vector<int> clause;
    clause.push_back(-15);
    clause.push_back(-23);
    clause.push_back(-26);
    clause.push_back(-29);
    // clause.push_back(-15);
    std::vector<int> newClause = convert_to_merge_variable(clause, 3, 11);
    for (int i=0;i<newClause.size();i++)
    {
        std::cout << newClause[i] << std::endl;
    }
}
*/

