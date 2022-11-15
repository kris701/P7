#ifndef Partial_REGRESSOR
#define Partial_REGRESSOR

#include "BaseRegressor.hh"

#include "../IntermediatePDDL/PDDLInstance.hh"

#include "PartialActionGenerator/PartialActionGenerator.hh"

class PartialRegressor : public BaseRegressor {
public:
    PartialRegressor(const PDDLInstance *instance, BaseDepthFunction *depthFunc, BaseWidthFunction *widthFunc) : BaseRegressor(instance, depthFunc, widthFunc) {
        actionGenerator = new PartialActionGenerator(&instance->domain->actions, instance->problem->objects.size());
    };
    ~PartialRegressor() {
        free(actionGenerator);
    }
protected:
    Path RegressFromState(const PDDLState *state) override;
    
private:
    PartialActionGenerator *actionGenerator;
    PartialAction* GetPredecessorStates(std::vector<PartialAction> *actions, std::unordered_set<PDDLState> &states);
    // Foreach precondition, get which states can lead to it
    // For now it ignores partial parameters
    std::unordered_set<PDDLState> GetPredecessorStates(const PartialAction *action);
    void GetPredecessorState(PDDLState *state, const PDDLActionInstance *action);
    PDDLState GeneratePreconditionState(const PDDLLiteral *literal);
    Path ConvertToPath(std::vector<PartialAction> actions);
    void SwitchToNonPartial(std::vector<PartialAction> *actions);
};

#endif
