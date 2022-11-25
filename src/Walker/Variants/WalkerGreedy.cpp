#include "WalkerGreedy.hpp"

Path WalkerGreedy::Walk(BaseHeuristic *heuristic, const PDDLState *state, unsigned int* current) {
    std::vector<PDDLActionInstance> steps; 
    steps.reserve(maxStepCount);
    std::unordered_set<PDDLState> visitedStates; 
    visitedStates.reserve(maxStepCount);

    PDDLState endState;
    PDDLState tempState = PDDLState(state->unaryFacts, state->binaryFacts);
    visitedStates.emplace(tempState);
    if (OnTempStateMade != nullptr)
        OnTempStateMade(this->instance, &tempState);

    heuristic->Reset();

    for (int i = 0; i < maxStepCount; i++) {
        if (!widthFunc->Iterate(current))
            break;

        std::vector<PDDLActionInstance> possibleActions;
        possibleActions = actionGenerator.GenerateActions(&tempState);

        if (possibleActions.size() == 0) break;
        PDDLActionInstance *chosenAction = heuristic->NextChoice(&tempState, &possibleActions);
        tempState.DoAction(chosenAction);

        if (visitedStates.contains(tempState))
            break;
        else {
            visitedStates.emplace(tempState);
            if (SaveStates)
                endState = tempState;
            steps.push_back(*chosenAction);

            if (OnStateWalk != nullptr)
                OnStateWalk(this->instance, &tempState, chosenAction);
        }
    }

    if (SaveStates)
        return Path(steps, *state, endState);
    else
        return Path(steps);
}

std::vector<Path> WalkerGreedy::Walk() {
    std::vector<Path> paths;
    unsigned int current;
    if (OnWalkerStart != nullptr)
        OnWalkerStart(this);
    auto startTime = std::chrono::steady_clock::now();
    while (widthFunc->Iterate(&current)) {
        Path path = Walk(heuristic, &this->instance->problem->initState, &current);
        if (path.steps.size() > 1)
            paths.push_back(path);

        if (OnWalkerStep != nullptr)
            OnWalkerStep(this, current);
        _totalIterations++;
    }
    auto ellapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();
    if (OnWalkerEnd != nullptr)
        OnWalkerEnd(this, ellapsed);
    return paths;
}
