﻿#include "CommonInterface.hh"

using namespace std;

enum CommonInterface::RunResult CommonInterface::Run(RunReport* report, int reformulatorIndex) {
	int64_t t;
	BaseReformulator* reformulator;
	int timeLimit = config.GetItem<int>("incrementLimit");
	int currentIncrementTimeLimit = config.GetItem<int>("startIncrement");
	bool isDirect = false;

	// Find a suitable reformulator
	ConsoleHelper::PrintInfo("Finding reformulator algorithm...");
	if (config.GetItem<vector<string>>("reformulator").at(reformulatorIndex) == "sameoutput") {
		reformulator = new SameOutputReformulator(&config, report, currentIncrementTimeLimit);
		isDirect = true;
	} else 	if (config.GetItem<vector<string>>("reformulator").at(reformulatorIndex) == "walker") {
		reformulator = new WalkerReformulator(&config, report, currentIncrementTimeLimit);
	}
	else{
		ConsoleHelper::PrintError("Reformulator not found! Reformulator: " + config.GetItem<string>("reformulator"));
		return CommonInterface::RunResult::ErrorsEncountered;
	}

	if (config.GetItem<bool>("debugmode")) {
		// Checking filepaths in the config file
		ConsoleHelper::PrintDebugInfo("Checking filepaths from the config...");
		report->Begin("Checking Filepaths");
		string notFoundPath = PathsChecker::IsPathsOk(&config);
		if (notFoundPath != "") {
			ConsoleHelper::PrintDebugError("Some file paths where not found! Not found path: " + notFoundPath);
			return CommonInterface::RunResult::ErrorsEncountered;
		}
		t = report->Stop();
	}

	// Parse original PDDL files
	ConsoleHelper::PrintInfo("Parsing PDDL files...");
	PDDLDriver originalDriver;
	report->Begin("Parsing PDDL Files");
	if (originalDriver.parse(config.GetItem<filesystem::path>("domain"))) {
		ConsoleHelper::PrintError("Error parsing the domain file!");
		return CommonInterface::RunResult::ErrorsEncountered;
	}
	if (originalDriver.parse(config.GetItem<filesystem::path>("problem"))) {
		ConsoleHelper::PrintError("Error parsing the problem file!");
		return CommonInterface::RunResult::ErrorsEncountered;
	}
	t = report->Stop();

	// Convert PDDL format
	ConsoleHelper::PrintInfo("Converting PDDL format...");
	report->Begin("Converison of PDDL format");
	PDDLDomain domain = PDDLConverter::Convert(originalDriver.domain);
	PDDLProblem problem = PDDLConverter::Convert(&domain, originalDriver.problem);
	PDDLInstance instance = PDDLInstance(&domain, &problem);
	t = report->Stop();

	int reformulationID = report->Setup("Reformulation of PDDL");
	int generatePDDLFilesID = report->Setup("Generating PDDL");
	int runFastDownwardID = report->Setup("Running FastDownward");

	if (isDirect)
		currentIncrementTimeLimit = timeLimit;

	DownwardRunner::DownwardRunnerResult runRes;
	int counter = 1;
	while (currentIncrementTimeLimit <= timeLimit) {
		ConsoleHelper::PrintInfo("Iteration " + to_string(counter) + "(" + to_string(currentIncrementTimeLimit) + "s)");
		reformulator->TimeLimit = currentIncrementTimeLimit;

		// Reformulate the PDDL file
		ConsoleHelper::PrintInfo("Reformulating PDDL...", 1);
		report->Resume(reformulationID);
		PDDLInstance reformulatedInstance = reformulator->ReformulatePDDL(&instance);
		report->Pause(reformulationID);

		// Generate new PDDL files
		ConsoleHelper::PrintInfo("Generating PDDL files...", 1);
		report->Resume(generatePDDLFilesID);
		PDDLCodeGenerator pddlGenerator = PDDLCodeGenerator(PDDLDomainCodeGenerator(reformulatedInstance.domain), PDDLProblemCodeGenerator(reformulatedInstance.domain, reformulatedInstance.problem));
		pddlGenerator.GenerateCode(reformulatedInstance, CommonInterface::TempDomainName, CommonInterface::TempProblemName);
		report->Pause(generatePDDLFilesID);

		// Throw the new pddl files into Fast Downward
		ConsoleHelper::PrintInfo("Run new PDDL files with Fast Downward...", 1);
		report->Resume(runFastDownwardID);
		DownwardRunner runner = DownwardRunner();
		runner.RunDownward(config, CommonInterface::TempDomainName, CommonInterface::TempProblemName, currentIncrementTimeLimit);
		runRes = runner.ParseDownwardLog();
		report->Pause(runFastDownwardID);
		if (runRes == DownwardRunner::FoundPlan) {
			report->Stop(reformulationID);
			report->Stop(generatePDDLFilesID);
			report->Stop(runFastDownwardID);
			break;
		}
		currentIncrementTimeLimit *= config.GetItem<int>("incrementModifier");
		counter++;
	}
	if (runRes != DownwardRunner::FoundPlan) {
		ConsoleHelper::PrintError("Fast downward did not find a plan in time!");
		return CommonInterface::RunResult::ErrorsEncountered;
	}

	if (config.GetItem<bool>("debugmode")) {
		// Check to make sure the reformulated plan also matches the reformulated problem and domain
		ConsoleHelper::PrintDebugInfo("Validate reformulated SAS plan...");
		report->Begin("Validating reformulated SAS plan");
		auto reformulatedSASValidatorResult = PlanValidator::ValidatePlan(config, CommonInterface::TempDomainName, CommonInterface::TempProblemName, CommonInterface::FastDownwardSASName);
		if (reformulatedSASValidatorResult != PlanValidator::PlanMatch) {
			ConsoleHelper::PrintDebugError("Output plan is not valid for reformulated domain and problem!");
			t = report->Stop();
			return CommonInterface::RunResult::ErrorsEncountered;
		}
		t = report->Stop();
	}

	// Parse the output SAS plan
	ConsoleHelper::PrintInfo("Parsing SAS Plan...");
	report->Begin("Parse SAS plan");
	SASParser sasParser;
	filesystem::path sasPath = filesystem::path(CommonInterface::FastDownwardSASName);
	SASPlan reformulatedSASPlan = sasParser.Parse(sasPath);
	t = report->Stop();

	// Rebuild the SAS Plan
	ConsoleHelper::PrintInfo("Rebuilding the SAS plan...");
	report->Begin("Rebuild SAS plan");
	SASPlan outputPlan = reformulator->RebuildSASPlan(&instance, &reformulatedSASPlan);
	t = report->Stop();

	// Output the new SAS plan
	ConsoleHelper::PrintInfo("Output new SAS Plan...");
	report->Begin("Output SAS plan");
	SASCodeGenerator sasGenerator;
	sasGenerator.GenerateCode(outputPlan, CommonInterface::OutputSASName);
	t = report->Stop();

	if (config.GetItem<bool>("debugmode")) {
		// Validate reformulated plan works with original domain and problem
		ConsoleHelper::PrintDebugInfo("Validate new SAS plan...");
		report->Begin("Validate new SAS plan");
		auto newSASValidatorResult = PlanValidator::ValidatePlan(config, config.GetItem<filesystem::path>("domain"), config.GetItem<filesystem::path>("problem"), CommonInterface::OutputSASName);
		if (newSASValidatorResult != PlanValidator::PlanMatch) {
			ConsoleHelper::PrintDebugError("Output plan is not valid for original domain and problem!");
			t = report->Stop();
			return CommonInterface::RunResult::ErrorsEncountered;
		}
		t = report->Stop();
	}
	
	return CommonInterface::RunResult::RanWithoutErrors;
}
