#include "State.hh"
#include "Instance.hh"

using namespace std;
using namespace PDDL;

DoActionChanges State::DoAction(const ActionInstance *action) {
    DoActionChanges changes = DoActionChanges();
    int actionEffectLength = action->action->effects.size();
    for (int i = 0; i < actionEffectLength; i++) {
        PDDL::Literal effect = action->action->effects.at(i);
        if (effect.args.size() == 1) {
            auto object = action->objects.at(effect.args.at(0));
            // Handle unary effect
            if (effect.value) {
                if (!unaryFacts.at(effect.predicateIndex).contains(object)) {
                    if (!changes.unaryChanges.contains(effect.predicateIndex))
                        changes.unaryChanges[effect.predicateIndex] = unordered_set<pair<unsigned int, bool>>{ make_pair(object, effect.value) };
                    else
                        changes.unaryChanges.at(effect.predicateIndex).emplace(make_pair(object, effect.value));
                }
                    
                unaryFacts.at(effect.predicateIndex).emplace(object);
            } else {
                if (unaryFacts.at(effect.predicateIndex).contains(object)) {
                    if (!changes.unaryChanges.contains(effect.predicateIndex))
                        changes.unaryChanges[effect.predicateIndex] = unordered_set<pair<unsigned int, bool>>{ make_pair(object, effect.value) };
                    else
                        changes.unaryChanges.at(effect.predicateIndex).emplace(make_pair(object, effect.value));
                }
                unaryFacts.at(effect.predicateIndex).erase(object);
            }
                
        } else {
            auto objects = make_pair(action->objects.at(effect.args.at(0)), action->objects.at(effect.args.at(1)));
            // Handle binary effect
            if (effect.value) {
                if (!binaryFacts.at(effect.predicateIndex).contains(objects)) {
                    if (!changes.binaryChanges.contains(effect.predicateIndex))
                        changes.binaryChanges[effect.predicateIndex] = unordered_set<pair<pair<unsigned int, unsigned int>, bool>>{ make_pair(objects, effect.value) };
                    else
                        changes.binaryChanges.at(effect.predicateIndex).emplace(make_pair(objects, effect.value));
                }
                binaryFacts.at(effect.predicateIndex).emplace(objects);
            } else {
                if (binaryFacts.at(effect.predicateIndex).contains(objects)) {
                    if (!changes.binaryChanges.contains(effect.predicateIndex))
                        changes.binaryChanges[effect.predicateIndex] = unordered_set<pair<pair<unsigned int, unsigned int>, bool>>{ make_pair(objects, effect.value) };
                    else
                        changes.binaryChanges.at(effect.predicateIndex).emplace(make_pair(objects, effect.value));
                }
                binaryFacts.at(effect.predicateIndex).erase(objects);
            }
        }
    }
    return changes;
}

void State::UndoAction(const DoActionChanges *changes) {
    auto unaryChanges = &changes->unaryChanges;
    auto binaryChanges = &changes->binaryChanges;

    for (auto uChangeIter = unaryChanges->begin(); uChangeIter != unaryChanges->end(); uChangeIter++) {
        auto predicate = (*uChangeIter).first;
        auto changes = &(*uChangeIter).second;
        for (auto changeIter = changes->begin(); changeIter != changes->end(); changeIter++) {
            auto change = &*changeIter;
            if (change->second)
                unaryFacts.at(predicate).erase(change->first);
            else
                unaryFacts.at(predicate).emplace(change->first);
        }
    }

    for (auto bChangeIter = binaryChanges->begin(); bChangeIter != binaryChanges->end(); bChangeIter++) {
        auto predicate = (*bChangeIter).first;
        auto changes = &(*bChangeIter).second;
        for (auto changeIter = changes->begin(); changeIter != changes->end(); changeIter++) {
            auto change = &*changeIter;
            if (change->second)
                binaryFacts.at(predicate).erase(change->first);
            else
                binaryFacts.at(predicate).emplace(change->first);
        }
    }
}

string State::ToString(const Instance* instance)
{
    string temp = "State ";
    temp += "\n";
    for (auto &unaryFact : unaryFacts)
    {
        if (unaryFact.second.size() > 0) {
            temp += "(";
            temp += instance->domain->predicates[unaryFact.first].name;

            for (auto objectindex : unaryFact.second) {
                temp += " " + instance->problem->objects[objectindex];
            }
            temp += ")";
        }
    }

    for (auto &binaryFact : binaryFacts)
    {
        temp += "\n";
        if (binaryFact.second.size() > 0) {
            temp += "(";
            temp += instance->domain->predicates[binaryFact.first].name;

            for (auto objectindex : binaryFact.second) {
                temp += " (" + instance->problem->objects[objectindex.first] + " " + instance->problem->objects[objectindex.second] + ")";
            }
            temp += ")";
        }
    }

    return temp;
};
