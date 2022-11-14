#include "PartialActionGenerator.hh"

#include "../../Helpers/AlgorithmHelper.hh"

std::unordered_set<PartialAction> PartialActionGenerator::ExpandState(const PDDLState *state) {
    std::unordered_set<PartialAction> partialActions;

    // Generate actions for unary facts
    for (auto iter = state->unaryFacts.begin(); iter != state->unaryFacts.end(); iter++)
        for (auto objectIter = (*iter).second.begin(); objectIter != (*iter).second.end(); objectIter++)
            AlgorithmHelper::InsertAll<PartialAction>(partialActions, ExpandUnary(std::make_pair((*iter).first, (*objectIter))));

    // Generate actions for binary facts
    for (auto iter = state->binaryFacts.begin(); iter != state->binaryFacts.end(); iter++)
        for (auto objectIter = (*iter).second.begin(); objectIter != (*iter).second.end(); objectIter++)
            AlgorithmHelper::InsertAll<PartialAction>(partialActions, ExpandBinary(std::make_pair((*iter).first, (*objectIter))));    

    // Who cares about multifacts
    // To Be Done

    return partialActions;
}

std::unordered_set<PartialAction> PartialActionGenerator::ExpandUnary(std::pair<unsigned int, unsigned int> predicateObject) {
    std::unordered_set<PartialAction> partialActions;

    for (int i = 0; i < actions->size(); i++) {
        const PDDLAction *action = &actions->at(i);

        for (int t = 0; t < action->effects.size(); t++) {
            const PDDLLiteral *effect = &action->effects.at(t);

            if (predicateObject.first == effect->predicateIndex && effect->value) {
                partialActions.emplace(CreateFromUnary(action, effect->args.at(0), predicateObject.second));
                break;
            }
        }
    }

    return partialActions;
}

std::unordered_set<PartialAction> PartialActionGenerator::ExpandBinary(std::pair<unsigned int, std::pair<unsigned int, unsigned int>> predicateObjects) {
    std::unordered_set<PartialAction> partialActions;

    for (int i = 0; i < actions->size(); i++) {
        const PDDLAction *action = &actions->at(i);

        for (int t = 0; t < action->effects.size(); t++) {
            const PDDLLiteral *effect = &action->effects.at(t);

            if (predicateObjects.first == effect->predicateIndex && effect->value) {
                partialActions.emplace(CreateFromBinay(action, std::make_pair(effect->args.at(0), effect->args.at(1)), predicateObjects.second));
                break;
            }
        }
    }

    return partialActions;
}

PartialAction PartialActionGenerator::CreateFromUnary(const PDDLAction *action, const unsigned int index, const unsigned int object) {
    std::vector<unsigned int*> tempObjects;

    for (int i = 0; i < action->parameters.size(); i++) {
        if (index == i)
            tempObjects.push_back(new unsigned int(object));
        else
            tempObjects.push_back(nullptr);
    }

    return PartialAction(action, tempObjects);
}

PartialAction PartialActionGenerator::CreateFromBinay(const PDDLAction *action, const std::pair<unsigned int, unsigned int> indexes, const std::pair<unsigned int, unsigned int> objects) {
    std::vector<unsigned int*> tempObjects;

    for (int i = 0; i < action->parameters.size(); i++) {
        if (indexes.first == i)
            tempObjects.push_back(new unsigned int(objects.first));
        else if (indexes.second == i)
            tempObjects.push_back(new unsigned int(objects.second));
        else
            tempObjects.push_back(nullptr);
    }

    return PartialAction(action, tempObjects);
}

PDDLActionInstance PartialActionGenerator::FillPartialAction(const PDDLInstance *instance, const PartialAction *partialAction) {
    std::vector<unsigned int> objects;
    for (unsigned int i = 0; i < partialAction->parameters.size(); i++) {
        if (partialAction->parameters.at(i) != nullptr)
            objects.push_back(*partialAction->parameters.at(i));
        else
            objects.push_back(GetParameterCandidate(instance, partialAction->action, &i));
    }
    return PDDLActionInstance(partialAction->action, objects);
}

unsigned int PartialActionGenerator::GetParameterCandidate(const PDDLInstance *instance, const PDDLAction *action, const unsigned int *paramIndex) {
    printf("\b");
}