#include "WalkerPathVerifyer.hh"

using namespace std;

vector<BadPath> WalkerPathVerifyer::VerifyPaths(vector<Path>* paths, PDDLInstance* instance, Config* config) {
	vector<BadPath> badPaths;

	if (paths->size() > 0) {
		PDDLDomainCodeGenerator domainGenerator(instance->domain);
		domainGenerator.GenerateDomainFile("domain_verify.pddl");
		for (auto pathPtr = paths->begin(); pathPtr != paths->end(); pathPtr++) {
			Path path = *pathPtr;
			if (path.hasStates) {
				vector<SAS::Action> actions;
				for (auto step : path.steps) {
					actions.push_back(GenerateSASActionFromActionInstance(step, instance));
				}
				SAS::Plan checkPlan(actions, actions.size(), 0);

				SAS::CodeGenerator sasGenerator;
				sasGenerator.GenerateCode(checkPlan, "sas_verify.sas");

				PDDLInstance newInstance(
					instance->domain,
					new PDDLProblem(instance->problem->name, instance->domain, instance->problem->objects, instance->problem->objectMap, path.startState, path.endState));

				PDDLProblemCodeGenerator problemGenerator(instance->domain, newInstance.problem);
				problemGenerator.GenerateProblemFile("problem_verify.pddl");

				auto reformulatedSASValidatorResult = PlanValidator::ValidatePlan(*config, "domain_verify.pddl", "problem_verify.pddl", "sas_verify.sas");
				if (reformulatedSASValidatorResult != PlanValidator::PlanMatch) {
					badPaths.push_back(BadPath(path, "Plan did not match with VAL"));
				}
			}
		}
	}

	return badPaths;
}

SAS::Action WalkerPathVerifyer::GenerateSASActionFromActionInstance(PDDLActionInstance action, PDDLInstance *instance) {
	vector<string> params;
	for (auto param : action.objects)
		params.push_back(instance->problem->objects.at(param));
	return SAS::Action(action.action->name, params);
}
