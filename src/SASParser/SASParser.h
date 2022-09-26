#ifndef SAS_PARSER
#define SAS_PARSER

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

using namespace std;

struct SASAction {
public:
    std::string name;
    std::vector<std::string>* parameters;

    SASAction(std::string name, std::vector<std::string>* parameters) : name(name), parameters(parameters) {}
};

struct Plan {
public:
    std::vector<SASAction*>* actions;
    int cost;

    Plan(std::vector<SASAction*>* actions, int cost) : actions(actions), cost(cost) {}
};

class SASParser {
public:
    Plan* SASPlan;
    void Parse(std::string path);

private:
    SASAction ParseAction(std::string line);
    int ParseCost(std::string line);
};

#endif
