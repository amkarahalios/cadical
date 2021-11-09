#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

std::string parse_edge_file(int numberOfColors, std::string inputFileName, int& numVertices)
{
  std::ofstream myfile;
  std::string newFileName = inputFileName + ".cnf";
  myfile.open(newFileName);

  // Parsing
  // - Read line by line
  std::vector<std::pair<int, int> > edges;
  std::vector<std::vector<bool> > edgeMatrix;
  int numberOfVertices;

  std::string line;
  std::ifstream infile(inputFileName);
  while (std::getline(infile, line))
  {
    if (line[0] == 'c')
    {
      continue;
    }
    else if (line[0] == 'p')
    {
      std::string buf;
      std::stringstream ss(line);
      std::vector<std::string> tokens;
      while (ss >> buf)
      {
        tokens.push_back(buf);
      }
      numberOfVertices = std::stoi(tokens[2]);
      numVertices = numberOfVertices;
    }
    else if (line[0] == 'e')
    {
      std::string buf;
      std::stringstream ss(line);
      std::vector<std::string> tokens;
      while (ss >> buf)
      {
        tokens.push_back(buf);
      }
      int vertex1 = std::stoi(tokens[1]);
      int vertex2 = std::stoi(tokens[2]);
      edges.push_back(std::make_pair(vertex1, vertex2));
    }
  }

  // Setup adjacency matrix
  edgeMatrix.resize(numberOfVertices);
  for (int vertexNum = 0; vertexNum < numberOfVertices; ++vertexNum)
  {
    edgeMatrix[vertexNum].resize(numberOfVertices);
    for (int vertexNum1 = 0; vertexNum1 < numberOfVertices; ++vertexNum1)
    {
      edgeMatrix[vertexNum][vertexNum1] = false;
    }
  }

  for (int edgeIndex = 0; edgeIndex < edges.size(); ++edgeIndex)
  {
    const std::pair<int, int> &edge = edges[edgeIndex];
    edgeMatrix[edge.first - 1][edge.second - 1] = true;
    edgeMatrix[edge.second - 1][edge.first - 1] = true;
  }

  // Encoding
  // Literals
  // - One literal for each variable/color combination
  int index = 1;
  std::vector<int> literals;
  for (int vertexNum = 0; vertexNum < numberOfVertices; ++vertexNum)
  {
    for (int colorNum = 0; colorNum < numberOfColors; ++colorNum)
    {
      literals.push_back(index);
      ++index;
    }
  }

  // Clauses
  // - v is colored
  std::vector<std::vector<int> > clauses;
  for (int vertexNum = 0; vertexNum < numberOfVertices; ++vertexNum)
  {
    std::vector<int> literalsInClause;
    for (int colorNum = 0; colorNum < numberOfColors; ++colorNum)
    {
      literalsInClause.push_back((vertexNum * numberOfColors) + colorNum + 1);
    }
    clauses.push_back(literalsInClause);
  }

  // NOTE (redundant, but we want to add it for our proofs)
  // - v has at most one color
  for (int vertexNum = 0; vertexNum < numberOfVertices; ++vertexNum)
  {
    for (int colorNum1 = 0; colorNum1 < numberOfColors - 1; ++colorNum1)
    {
      for (int colorNum2 = colorNum1 + 1; colorNum2 < numberOfColors; ++colorNum2)
      {
        std::vector<int> literalsInClause;
        literalsInClause.push_back(-1 * ((vertexNum * numberOfColors) + colorNum1 + 1));
        literalsInClause.push_back(-1 * ((vertexNum * numberOfColors) + colorNum2 + 1));
        clauses.push_back(literalsInClause);
      }
    }
  }

  // - v and w have a different color
  for (int edgeIndex = 0; edgeIndex < edges.size(); ++edgeIndex)
  {
    const std::pair<int, int> &edge = edges[edgeIndex];
    for (int colorNum = 0; colorNum < numberOfColors; ++colorNum)
    {
      std::vector<int> literalsInClause;
      literalsInClause.push_back(-1 * (((edge.first - 1) * numberOfColors) + colorNum + 1));
      literalsInClause.push_back(-1 * (((edge.second - 1) * numberOfColors) + colorNum + 1));
      clauses.push_back(literalsInClause);
    }
  }

  //myfile << "c END OF ENCODING"<< std::endl;
  // Create merge literals and define them with clauses
  std::vector<std::vector<int> > mergeClauses;
  for (int vertexNum1 = 0; vertexNum1 < numberOfVertices - 1; ++vertexNum1)
  {
    for (int vertexNum2 = 0; vertexNum2 < numberOfVertices; ++vertexNum2)
    {
      //if (!edgeMatrix[vertexNum1][vertexNum2])
      //{
      int mergeLiteral = literals.size() + 1;
      literals.push_back(mergeLiteral);
      for (int colorNum = 0; colorNum < numberOfColors; ++colorNum)
      {
        // If the vertices have same color, force merge literal to be true
        // If the merge literal is false and one of them has color i, the other cannot have color i
        std::vector<int> literalsInClause1;
        literalsInClause1.push_back(mergeLiteral);
        literalsInClause1.push_back(-1 * (((vertexNum1)*numberOfColors) + colorNum + 1));
        literalsInClause1.push_back(-1 * (((vertexNum2)*numberOfColors) + colorNum + 1));
        mergeClauses.push_back(literalsInClause1);

        // If the merge literal is true, if v has color i, then w has color i
        std::vector<int> literalsInClause2;
        literalsInClause2.push_back(-1 * mergeLiteral);
        literalsInClause2.push_back((((vertexNum1)*numberOfColors) + colorNum + 1));
        literalsInClause2.push_back(-1 * (((vertexNum2)*numberOfColors) + colorNum + 1));
        mergeClauses.push_back(literalsInClause2);

        // If the merge literal is true, if w has color i, then v has color i
        std::vector<int> literalsInClause3;
        literalsInClause3.push_back(-1 * mergeLiteral);
        literalsInClause3.push_back(-1 * (((vertexNum1)*numberOfColors) + colorNum + 1));
        literalsInClause3.push_back((((vertexNum2)*numberOfColors) + colorNum + 1));
        mergeClauses.push_back(literalsInClause3);
      }
      //}
    }
  }

  // Output encoding of the coloring problem
  myfile << "p cnf " << literals.size() << " " << clauses.size() + mergeClauses.size() << std::endl;
  for (int clauseIndex = 0; clauseIndex < clauses.size(); ++clauseIndex)
  {
    const std::vector<int> &clause = clauses[clauseIndex];
    for (int literal = 0; literal < clause.size(); ++literal)
    {
      myfile << clause[literal] << " ";
    }
    myfile << "0" << std::endl;
  }

  // Output encoding of merge variables
  for (int clauseIndex=0; clauseIndex<mergeClauses.size(); ++clauseIndex)
  {
    const std::vector<int>& clause = mergeClauses[clauseIndex];
    for (int literal=0; literal<clause.size(); ++literal)
    {
      myfile << clause[literal] << " ";
    }
    myfile << "0" << std::endl;
  }

  myfile.close();
  return newFileName;
}
