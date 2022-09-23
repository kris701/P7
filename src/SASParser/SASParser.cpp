#include "SASParser.h"

Plan SAS_Parser::Parse(std::string path) {
    std::vector<SASAction> actions;
    int cost;
    std::ifstream stream(path);
    std::string line;
    while (std::getline(stream, line)) {
        if (line[0] == '(') {
            actions.push_back(ParseAction(line.substr(1, line.length() - 2)));
        } else if (line[0] == ';') {
            cost = ParseCost(line);
        }
    }
    return Plan(actions, cost);
}

std::vector<std::string> tokenize(std::string const &str, const char delim) {
    std::stringstream ss(str);
    std::vector<std::string> tokens;
 
    std::string s;
    while (std::getline(ss, s, delim)) {
        tokens.push_back(s);
    }
    return tokens;
}

SASAction SAS_Parser::ParseAction(std::string line) {
    std::vector<std::string> tokens = tokenize(line, ' ');
    std::string actionName = tokens.front(); tokens.erase(tokens.begin());
    std::vector<std::string> parameters = tokens;
    return SASAction(actionName, parameters);
}

int SAS_Parser::ParseCost(std::string line) {
    return -1;
}
