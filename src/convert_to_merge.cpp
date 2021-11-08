#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iostream>
#include <map>

std::vector<int> convert_to_merge_variable(std::vector<int> literals, int numberOfColors, int numberOfVertices)
{
    std::vector<int> result;
    std::map<int, std::vector<int> > colorVertexMap;
    for (int ind = 0; ind < literals.size(); ind++)
    {
        //if it's a merge variables just add it to the return list
        if (std::abs(literals[ind])>numberOfColors*numberOfVertices) result.push_back(literals[ind]);
        assert (literals[ind]<0);
        int color = (std::abs(literals[ind])%numberOfColors);
        colorVertexMap[color].push_back(literals[ind]);
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
        vertexNumj = std::ceil(-1.0*value[0]/numberOfColors);
        OneOfEachColor.push_back(value[0]);
        for (int i = 0; i< value.size()-1; i++)
        {
           
            vertexNumi = vertexNumj;
            vertexNumj = std::ceil(-1.0*value[i+1]/numberOfColors);
            result.push_back(-(numberOfColors*numberOfVertices + (std::min(vertexNumi,vertexNumj)-1)*numberOfVertices+std::max(vertexNumi,vertexNumj)));
        }
    }

    //add negative merge variables for vertices with differnt color
    for (int i=0; i < OneOfEachColor.size()-1; i++)
    {
        for (int j=i+1; j<OneOfEachColor.size();j++)
        {
            int vertexNumi = std::ceil(-1.0*OneOfEachColor[i]/numberOfColors);
            int vertexNumj = std::ceil(-1.0*OneOfEachColor[j]/numberOfColors);
            result.push_back(numberOfColors*numberOfVertices + (std::min(vertexNumi,vertexNumj)-1)*numberOfVertices+std::max(vertexNumi,vertexNumj));
        }
    }
    return result;
}
/*
int main()
{
    std::vector<int> clause;
    clause.push_back(-1);
    clause.push_back(-4);
    clause.push_back(-8);
    clause.push_back(-11);
    clause.push_back(-15);
    std::vector<int> newClause = convert(clause, 3, 5);
    for (int i=0;i<newClause.size();i++)
    {
        std::cout << newClause[i] << std::endl;
    }
}
*/
