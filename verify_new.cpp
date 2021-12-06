#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

void parse_edge_file(int numberOfColors, std::string inputFileName, int &numVertices, std::vector<std::vector<bool> > &existingEdges)
{
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

  existingEdges.resize(edgeMatrix.size());
  for (int i = 0; i < edgeMatrix.size(); ++i)
  {
    existingEdges[i].resize(edgeMatrix.size());
    for (int j = 0; j < edgeMatrix.size(); ++j)
    {
      existingEdges[i][j] = edgeMatrix[i][j];
    }
  }
}

std::vector<std::set<int> > getKCliques(int k, std::vector<std::vector<bool> > edgeMatrix)
{
  std::vector<std::set<int> > res;
  if (k < 2)
  {
    return res;
  }
  if (k == 2)
  {
    for (int i = 0; i < edgeMatrix.size() - 1; i++)
    {
      for (int j = i + 1; j < edgeMatrix.size(); j++)
      {
        if (edgeMatrix[i][j])
        {
          std::set<int> a;
          a.insert(i);
          a.insert(j);
          res.push_back(a);
        }
      }
    }
    return res;
  }
  else
  {
    std::vector<std::set<int> > smallCliques = getKCliques(k - 1, edgeMatrix);
    for (int i = 0; i < smallCliques.size(); i++)
    {
      for (int j = 0; j < edgeMatrix.size(); j++)
      {
        if ((smallCliques[i].find(j) == smallCliques[i].end()) && ((*smallCliques[i].rbegin()) < j))
        {
          bool pos = true;
          for (int t : smallCliques[i])
          {
            if (!edgeMatrix[j][t])
            {
              pos = false;
            }
          }
          if (pos)
          {
            std::set<int> b;
            for (int l : smallCliques[i])
            {
              b.insert(l);
            }
            b.insert(j);
            res.push_back(b);
          }
        }
      }
    }
    return res;
  }
}

std::set<int> getFirstAlmostKCliques(int k, std::vector<std::vector<bool> > edgeMatrix)
{
  // for (int i = 0; i<edgeMatrix.size();i++)
  // {
  //   for (int j = 0; j< edgeMatrix.size();j++)
  //   {
  //     std::cout<< edgeMatrix[i][j];
  //   }
  //   std::cout<< std::endl;
  // }
  std::set<int> res;
  if (k <= 2)
  {
    std::set<int> a;
    a.insert(0);
    return a;
  }
  else
  {
    std::vector<std::set<int> > smallCliques = getKCliques(k - 1, edgeMatrix);
    for (int i = 0; i < smallCliques.size(); i++)
    {
      std::cout << "small clique: ";
      for (int j : smallCliques[i])
      {
        std::cout << j << ",";
      }
      std::cout << std::endl;
    }
    for (int i = 0; i < smallCliques.size(); i++)
    {
      for (int j = 0; j < edgeMatrix.size(); j++)
      {
        if (smallCliques[i].find(j) == smallCliques[i].end())
        {
          int s = 0;
          for (int t : smallCliques[i])
          {
            s += int(edgeMatrix[j][t]);
          }
          if (s == k - 2)
          {
            for (int l : smallCliques[i])
            {
              res.insert(l);
              // std::cout << l << " ";
            }
            res.insert(j);
            // std::cout << j << " ";
            // std::cout << std::endl;
            return res;
          }
        }
      }
    }
    return res;
  }
}

//Always merge to the smaller vertex and delete all connections for the larger vertex
std::vector<std::vector<bool> > merge(int vertA, int vertB, std::vector<std::vector<bool> > edgeMatrix)
{
  int a = std::min(vertA, vertB);
  int b = std::max(vertA, vertB);
  std::vector<std::vector<bool> > res;
  res.resize(edgeMatrix.size());
  for (int i = 0; i < edgeMatrix.size(); i++)
  {
    res[i].resize(edgeMatrix[i].size());
    if (i == a)
    {
      for (int j = 0; j < edgeMatrix[i].size(); j++)
      {
        res[i][j] = (edgeMatrix[a][j] || edgeMatrix[b][j]);
      }
    }
    else if (i != b)
    {
      for (int j = 0; j < edgeMatrix[i].size(); j++)
      {
        if (j == a)
        {
          res[i][j] = (edgeMatrix[i][a] || edgeMatrix[i][b]);
        }
        else if (j == b)
        {
          res[i][b] = 0;
        }
        else
        {
          res[i][j] = edgeMatrix[i][j];
        }
      }
    }
  }
  return res;
}

std::vector<std::vector<bool> > addEdge(int vertA, int vertB, std::vector<std::vector<bool> > edgeMatrix)
{
  std::vector<std::vector<bool> > res;
  res.resize(edgeMatrix.size());
  for (int i = 0; i < edgeMatrix.size(); i++)
  {
    res[i].resize(edgeMatrix[i].size());
    for (int j = 0; j < edgeMatrix[i].size(); j++)
    {
      res[i][j] = edgeMatrix[i][j];
    }
  }
  res[vertA][vertB] = true;
  res[vertB][vertA] = true;
  return res;
}
//if there exists a first almost clique, merge the nodes, return the positive merge literal and the merged graph
std::pair<int, std::vector<std::vector<bool> > > mergeFirstAlmostClique(int numColors, std::vector<std::vector<bool> > edgeMatrix, int numVertex)
{
  std::set<int> clik = getFirstAlmostKCliques(numColors + 1, edgeMatrix);
  if (clik.size())
  {
    std::cout << "Almost Clique:";
    for (int i : clik)
    {
      std::cout << i << " ";
    }
    std::cout << std::endl;
    int mergeA = -1;
    int mergeB = -1;
    for (int i : clik)
    {
      for (int j : clik)
      {
        if ((i < j) && (!edgeMatrix[i][j]))
        {
          // mergePair.push_back(1);
          mergeA = i;
          mergeB = j;
        }
      }
    }
    int mergeLit = numColors * numVertex + std::min(mergeA, mergeB) * numVertex + std::max(mergeA, mergeB) + 1;
    edgeMatrix = merge(mergeA, mergeB, edgeMatrix);
    return std::make_pair(mergeLit, edgeMatrix);
  }
  else
  {
    return std::make_pair(0, edgeMatrix);
  }
}

//parse proof file into list of sets
std::vector<std::set<int> > parse_proof_file(std::string proofFileName, int numVertex, int numColors)
{
  std::vector<std::set<int> > proof;
  std::string line;
  std::ifstream infile(proofFileName);
  while (std::getline(infile, line))
  {
    if (line[0] == 'd')
    {
      continue;
    }
    else
    {
      std::string buf;
      std::stringstream ss(line);
      std::vector<std::string> tokens;
      while (ss >> buf)
      {
        tokens.push_back(buf);
      }
      std::set<int> clause;
      for (int i = 0; i < tokens.size(); i++)
      {
        int num = std::stoi(tokens[i]);
        if ((num != 0) && (std::abs(num) > numVertex * numColors))
        {
          clause.insert(num);
        }
      }
      if (clause.size())
      {
        proof.push_back(clause);
      }
    }
  }
  return proof;
}

int set_one_diff(std::set<int> a, std::set<int> b)
{
  if (a.size() == 1)
  {
    return *a.begin();
  }
  int diff = 0;
  for (int i : a)
  {
    if (b.find(i) == b.end())
    {
      if (!diff)
      {
        diff = i;
      }
      else
      {
        return 0;
      }
    }
  }
  return diff;
}

std::vector<std::vector<int> > convertOpsSet2Vec(std::set<int> ops, int numVertex, int numColors)
{
  std::vector<std::vector<int> > operations;
  for (int j : ops)
  {
    std::vector<int> op;
    if (j > 0)
    {
      op.push_back(-1);
    }
    else
    {
      op.push_back(1);
    }
    //vertNum1, vertNum2 start at 1, while the edgeMatrix is 0-indexed.
    int vertNum1 = std::ceil((std::abs(j) - numVertex * numColors) * 1.0 / numVertex);
    int vertNum2 = std::abs(j) - numVertex * (numColors + vertNum1 - 1);
    op.push_back(vertNum1 - 1);
    op.push_back(vertNum2 - 1);
    operations.push_back(op);
  }
  return operations;
}

std::set<int> convertOpsVec2Set(std::vector<std::vector<int> > impliedOps, int numVertex, int numColors)
{
  std::set<int> convertImp;
  for (int i = 0; i < impliedOps.size(); i++)
  {
    int mergeLit = numColors * numVertex + std::min(impliedOps[i][1], impliedOps[i][2]) * numVertex + std::max(impliedOps[i][1], impliedOps[i][2]) + 1;
    if (impliedOps[i][0] > 0)
    {
      convertImp.insert(-mergeLit);
    }
    else
    {
      convertImp.insert(mergeLit);
    }
  }
  return convertImp;
}
//operate on the graph. If there is an invalid merge, return false
std::pair<bool, std::vector<std::vector<bool> > > batchOperate(std::set<int> ops, std::vector<std::vector<bool> > edgeMatrix, int numVertex, int numColors)
{
  // std::cout <<"here 1" <<std::endl;
  std::vector<std::vector<int> > operations = convertOpsSet2Vec(ops, numVertex, numColors);
  // std::cout<< operations.size()<< std::endl;
  // std::cout <<"here 2" <<std::endl;
  // std::cout << "in batch operate"<<std::endl;
  std::vector<std::set<int> > mergeList;
  for (int i = 0; i < operations.size(); i++)
  {
    // std::cout <<operations[i].size() <<std::endl;
    // std::cout <<"here 3" <<std::endl;
    if (operations[i][0] == -1)
    {
      // std::cout <<"here 4" <<std::endl;
      // std::cout << operations[i][1]<<std::endl;
      // std::cout << operations[i][2]<<std::endl;
      edgeMatrix = addEdge(operations[i][1], operations[i][2], edgeMatrix);
    }
    else
    {
      // std::cout <<"here 10" <<std::endl;
      bool inserted = false;
      for (int j = 0; j < mergeList.size(); j++)
      {
        if ((!inserted) && (mergeList[j].find(operations[i][1]) != mergeList[j].end() || mergeList[j].find(operations[i][2]) != mergeList[j].end()))
        {
          std::set<int> merges = mergeList[j];
          merges.insert(operations[i][1]);
          merges.insert(operations[i][2]);
          mergeList[j] = merges;
          inserted = true;
        }
      }
      // std::cout <<"here 11" <<std::endl;
      if (!inserted)
      {
        // std::cout <<"here 12" <<std::endl;
        std::set<int> merges;
        merges.insert(operations[i][1]);
        merges.insert(operations[i][2]);
        mergeList.push_back(merges);
      }
    }
    // std::cout <<"here 5" <<std::endl;
  }
  // std::cout<<mergeList.size()<<std::endl;
  //attempt to merge the nodes. If there exists an edge between the nodes, return false
  for (int k = 0; k < mergeList.size(); k++)
  {
    std::set<int> m = mergeList[k];
    for (int a : m)
    {
      for (int b : m)
      {
        if (edgeMatrix[a][b])
        {
          // std::cout <<"here 6" <<std::endl;
          return std::make_pair(false, edgeMatrix);
        }
      }
    }
    int vert1 = *m.begin();
    std::set<int>::iterator it = m.begin();
    it++;
    // std::cout <<"here 7" <<std::endl;
    while (it != m.end())
    {
      // std::cout <<"here 8" <<std::endl;
      edgeMatrix = merge(vert1, *it, edgeMatrix);
      // std::cout <<"here 9" <<std::endl;
      it++;
    }
  }
  return std::make_pair(true, edgeMatrix);
}

std::set<int> findImpliedOperations(std::set<int> ops, std::vector<std::vector<bool> > edgeMatrix, int numVertex, int numColors)
{
  // std::cout << "in batch operate"<<std::endl;
  std::vector<std::vector<int> > operations = convertOpsSet2Vec(ops, numVertex, numColors);
  std::vector<std::vector<int> > addList;
  std::vector<std::set<int> > mergeList;
  for (int i = 0; i < operations.size(); i++)
  {
    if (operations[i][0] == -1)
    {
      std::vector<int> addVec;
      addVec.push_back(operations[i][1]);
      addVec.push_back(operations[i][2]);
      addList.push_back(addVec);
    }
    else
    {
      bool inserted = false;
      for (int j = 0; j < mergeList.size(); j++)
      {
        if ((!inserted) && (mergeList[j].find(operations[i][1]) != mergeList[j].end() || mergeList[j].find(operations[i][2]) != mergeList[j].end()))
        {
          std::set<int> merges = mergeList[j];
          merges.insert(operations[i][1]);
          merges.insert(operations[i][2]);
          mergeList[j] = merges;
          inserted = true;
        }
      }
      if (!inserted)
      {
        std::set<int> merges;
        merges.insert(operations[i][1]);
        merges.insert(operations[i][2]);
        mergeList.push_back(merges);
      }
    }
  }
  std::vector<std::vector<int> > impliedOps;
  for (int i = 0; i < addList.size(); i++)
  {
    std::set<int> firstSet;
    firstSet.insert(addList[i][0]);
    std::set<int> secondSet;
    secondSet.insert(addList[i][1]);
    for (int j = 0; j < mergeList.size(); j++)
    {
      if (mergeList[j].find(addList[i][0]) != mergeList[j].end())
      {
        firstSet = mergeList[j];
      }
      if (mergeList[j].find(addList[i][1]) != mergeList[j].end())
      {
        secondSet = mergeList[j];
      }
    }
    for (int a : firstSet)
    {
      for (int b : secondSet)
      {
        if (a != b)
        {
          std::vector<int> op;
          op.push_back(-1);
          op.push_back(a);
          op.push_back(b);
          impliedOps.push_back(op);
        }
      }
    }
  }
  for (int k = 0; k < mergeList.size(); k++)
  {
    std::set<int> m = mergeList[k];
    for (int a : m)
    {
      for (int b : m)
      {
        if (a != b)
        {
          std::vector<int> op;
          op.push_back(1);
          op.push_back(a);
          op.push_back(b);
          impliedOps.push_back(op);
          for (int s = 0; s < edgeMatrix.size(); s++)
          {
            if ((edgeMatrix[a][s]))
            {
              std::vector<int> op;
              op.push_back(-1);
              op.push_back(b);
              op.push_back(s);
              impliedOps.push_back(op);
            }
            if ((edgeMatrix[b][s]))
            {
              std::vector<int> op;
              op.push_back(-1);
              op.push_back(a);
              op.push_back(s);
              impliedOps.push_back(op);
            }
          }
        }
      }
    }
  }
  return convertOpsVec2Set(impliedOps, numVertex, numColors);
}

//return the full list of ops in addition to whether conflict was found
std::pair<bool, std::set<int> > conflictAfterOp(std::set<int> ops, std::vector<std::vector<bool> > edgeMatrix, int numColors, int numVertex)
{
  // std::cout <<"here 1" <<std::endl;
  std::pair<bool, std::vector<std::vector<bool> > > operatedMatrix = batchOperate(ops, edgeMatrix, numVertex, numColors);
  // std::cout <<"here 2" <<std::endl;
  if (!operatedMatrix.first)
  {
    return std::make_pair(true, ops);
  }
  // std::cout <<"here 3" <<std::endl;
  if (getKCliques(numColors + 1, operatedMatrix.second).size())
  {
    // std::cout <<"here 4" <<std::endl;
    return std::make_pair(true, ops);
  }
  // std::cout <<"here 5" <<std::endl;
  std::pair<int, std::vector<std::vector<bool> > > litMatrixPair = mergeFirstAlmostClique(numColors, operatedMatrix.second, numVertex);
  // std::cout <<"here 6" <<std::endl;
  while (litMatrixPair.first)
  {
    ops.insert(-litMatrixPair.first);
    edgeMatrix = litMatrixPair.second;
    if (getKCliques(numColors + 1, edgeMatrix).size())
    {
      return std::make_pair(true, ops);
    }
    else
    {
      litMatrixPair = mergeFirstAlmostClique(numColors, edgeMatrix, numVertex);
    }
  }
  return std::make_pair(false, ops);
}

bool verify_proof(std::vector<std::set<int> > proof, int numVertex, int numColors, std::vector<std::vector<bool> > edgeMatrix)
{
  std::set<int> emptyClause;
  proof.push_back(emptyClause);
  for (int i = 0; i < proof.size(); i++)
  {
    std::cout << "line number: " << i << std::endl;
    std::cout << "verifying proof line ";
    for (int j : proof[i])
    {
      std::cout << j << " ";
    }
    std::cout << "0" << std::endl;
    std::vector<std::vector<int> > operations;
    std::set<int> cur_op;
    for (int j : proof[i])
    {
      cur_op.insert(j);
    }
    bool added = true;
    bool foundConflict = false;
    while (added)
    {
      added = false;
      // std::cout <<"here 1" <<std::endl;
      std::pair<bool, std::set<int> > boolSetpair = conflictAfterOp(cur_op, edgeMatrix, numColors, numVertex);
      // std::cout <<"here 2" <<std::endl;
      if (boolSetpair.first)
      {
        foundConflict = true;
        break;
      }
      else
      {
        // std::cout << "c" << std::endl;
        int cur_op_size = cur_op.size();
        cur_op = boolSetpair.second;
        // std::cout <<"here 3" <<std::endl;
        cur_op = findImpliedOperations(cur_op, edgeMatrix, numVertex, numColors);
        // std::cout <<"here 4" <<std::endl;
        for (int l = i - 1; l >= 0; l--)
        {
          int diff = set_one_diff(proof[l], cur_op);
          // std::cout << "diff is " << diff << std::endl;
          if (diff && (cur_op.find(-diff) == cur_op.end()))
          {
            cur_op.insert(-diff);
            std::cout << "adding " << -diff << " to ops" << std::endl;
          }
        }
        std::cout << "cur_ops ";
        for (int p : cur_op)
        {
          std::cout << p << " ";
        }
        std::cout << "0" << std::endl;
        if (cur_op_size < cur_op.size())
        {
          added = true;
          std::cout << "added, " << cur_op.size() << std::endl;
        }
      }
    }
    if (!foundConflict)
    {
      std::cout << "line number: " << i << std::endl;
      std::cout << "current ops: ";
      for (int j : cur_op)
      {
        std::cout << j << " ";
      }
      std::cout << "0" << std::endl;
      operations = convertOpsSet2Vec(cur_op, numVertex, numColors);
      for (int i = 0; i < operations.size(); i++)
      {
        if (operations[i][0] > 0)
        {
          std::cout << "contract ";
        }
        else
        {
          std::cout << "add edge ";
        }
        for (int j = 1; j < operations[i].size(); j++)
        {
          std::cout << operations[i][j] + 1 << " ";
        }
        std::cout << std::endl;
      }
      std::cout << "does not result in a conflict " << std::endl;
      return false;
    }
  }
  return true;
}

int main(int argc, char **argv)
{
  // take the number of colors and the file path as input
  int numberOfColors;
  std::string dimacsFileName;
  std::vector<std::vector<bool> > edgeMtrx;
  int numVertices;
  std::string proofFileName;
  if (argc == 4)
  {
    numberOfColors = std::atoi(argv[1]);
    dimacsFileName = argv[2];
    proofFileName = argv[3];
  }
  else
  {
    std::cout << "Incorrect number of input arguments" << std::endl;
  }
  parse_edge_file(numberOfColors, dimacsFileName, numVertices, edgeMtrx);
  std::vector<std::set<int> > pf = parse_proof_file(proofFileName, numVertices, numberOfColors);
  std::cout << pf.size() <<std::endl;
  std::cout << verify_proof(pf, numVertices, numberOfColors, edgeMtrx) << std::endl;
}
