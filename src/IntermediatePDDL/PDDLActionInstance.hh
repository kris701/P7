#ifndef PDDL_ACTION_INSTANCE_HH
#define PDDL_ACTION_INSTANCE_HH

#include <vector>
#include <unordered_set>
#include <string>

#include "PDDLProblem.hh"
#include "PDDLDomain.hh"
#include "PDDLAction.hh"

struct PDDLProblem;

class PDDLActionInstance {
public:
    const PDDLAction *action = nullptr;
    const std::vector<unsigned int> objects;
    PDDLActionInstance() {};
    PDDLActionInstance(const PDDLAction *action, const std::vector<unsigned int> objects) : action(action), objects(objects) {}; 
    std::string ToString(const PDDLProblem* problem, const PDDLDomain* domain);
    std::string LiteralsToString(std::vector<PDDLLiteral> literals, const PDDLDomain* domain, const PDDLProblem* problem);
    
    friend bool operator==(const PDDLActionInstance &lhs, const PDDLActionInstance &rhs) {
        if (lhs.action != rhs.action)
            return false;
        if (lhs.objects != rhs.objects)
            return false;
        return true;
    }
    struct HashFunction {
        std::size_t operator()(PDDLActionInstance const& s) const noexcept
        {
            std::size_t h1 = std::hash<const PDDLAction*>{}(s.action);
            std::size_t h2 = HashVector(s.objects);
            return h1 ^ (h2 << 1);
        }
        //https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
        std::size_t HashVector(const std::vector<unsigned int> &vec) const {
            std::size_t seed = vec.size();
            for(auto& i : vec) {
                seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
private:
};




#endif
