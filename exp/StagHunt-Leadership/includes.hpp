// All necessary includes and defines are here

#include "defparams.hpp"
#include <iostream>
#include <math.h>
#include <ctime>
#include <fstream>
#include <sferes/phen/parameters.hpp>
#include <sferes/ea/nsga2.hpp>
#include <sferes/ea/rank_simple.hpp>
#include <sferes/ea/cmaes.hpp>
#include <sferes/gen/cmaes.hpp>
#include <sferes/eval/eval.hpp>
#include <sferes/stat/pareto_front.hpp>
#include <sferes/modif/dummy.hpp>
#include <sferes/run.hpp>

#include <modules/fastsim_multi2/simu_fastsim_multi.hpp>
#include <modules/nn2/mlp.hpp>

#ifdef ELMAN
#include <modules/nn2/elman.hpp>
#endif

#include "Hare.hpp"
#include "Hunter.hpp"
#include "Stag.hpp"
#include "StagHuntRobot.hpp"
#include "BestFitEval.hpp"
#include "MeanFitEval.hpp"
#include "BestEverFitEval.hpp"
#include "AllFitEvalStat.hpp"
#include "PhenChasseur.hpp"
#include "ElitistGen.hpp"
#include "FitnessProp.hpp"
#include "Elitist.hpp"
#include "StagHuntEvalParallel.hpp"
#include "BestLeadershipEval.hpp"
#include "AllLeadershipEvalStat.hpp"
#include "AllGenomesTraceStat.hpp"

#ifdef DOUBLE_NN
#include "BestNNEval.hpp"
#include "AllNNEvalStat.hpp"
#endif

#ifdef COEVO
#include "Elitist-CoEvo.hpp"
#include "StagHuntEvalParallel-CoEvo.hpp"
#include "BestFitEval-CoEvo.hpp"
#include "MeanFitEval-CoEvo.hpp"
#include "BestEverFitEval-CoEvo.hpp"
#include "AllFitEvalStat-CoEvo.hpp"
#include "BestFitBehaviour-CoEvo.hpp"
#include "BestFitBehaviourVideo-CoEvo.hpp"
#include "BestLeadershipEval-CoEvo.hpp"
#include "AllLeadershipEvalStat-CoEvo.hpp"
#endif

#ifdef DIVERSITY
#include "BestDiversityEval.hpp"
#include "BestEverDiversityEval.hpp"
#include "MeanDiversityEval.hpp"
#include "AllDiversityEvalStat.hpp"

#ifdef COEVO
#include <sferes/ea/nsga2-CoEvo.hpp>
#endif
#endif

#include "StopEval.hpp"
#include "BestFitBehaviour.hpp"
#include "BestFitBehaviourVideo.hpp"

using namespace sferes;
using namespace sferes::gen::evo_float;
