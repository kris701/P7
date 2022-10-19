#include "RandomWalkerReformulator.hh"

using namespace std;

PDDLInstance RandomWalkerReformulator::ReformulatePDDL(PDDLInstance* instance) {
	RandomHeuristic<PDDLActionInstance>* heu = new RandomHeuristic<PDDLActionInstance>(PDDLContext(instance->domain, instance->problem));
	BaseWidthFunction *widthFunc;
	if (Configs->ReformulatorTime.Content == -1)
		widthFunc = new ConstantWidthFunction(10000);
	else
		widthFunc = new TimeWidthFunction(Configs->ReformulatorTime.Content);
	auto depthFunction = new ObjectActionDepthFunction(*instance);
	std::vector<Path> paths;
	unsigned int totalActionCount = 0;
	for (int i = 0; i < widthFunc->GetWidth(); i++) {
		Walker walker = Walker(instance,
		ActionGenerator(instance->domain, instance->problem),
		heu,
		depthFunction);
		paths.push_back(walker.Walk());
		totalActionCount += walker.totalActions;
	}
	if (Configs->DebugMode.Content) {
		ConsoleHelper::PrintDebugInfo("[Walker] Total actions performed: " + to_string(totalActionCount), 1);
	}

	// Do Something and transform the input PDDL into a new PDDL format
	PDDLInstance newInstance(instance->domain, instance->problem);

	return *instance;
}

SASPlan RandomWalkerReformulator::RebuildSASPlan(SASPlan* reformulatedSAS) {
	// Do Something and give a "corrected" SAS plan back
	SASPlan newPlan(reformulatedSAS->actions, reformulatedSAS->cost);
	return newPlan;
} 