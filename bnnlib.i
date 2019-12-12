%module bnnlib

%include stl.i
%include std_vector.i
%include std_string.i

using namespace std;

%ignore *::operator[];




%{
#include <stdlib.h>
#include <string>
#include "src/definitions.h"
#include "src/wrapper.h"
#include "src/Network.h"
#include "src/Sequence.h"
#include "src/SequenceSet.h"
#include "src/functions.h"
#include "src/NetworkFactory.h"
#include "src/GnuplotGenerator.h"
#include "src/TrainingConfiguration.h"
#include "src/nodes/Node.h"
#include "src/LSTMCascade.h"
#include "src/callbacks/CallbackHandler.h"
#include "src/callbacks/BiasWeightWatcher.h"
#include "src/callbacks/WeightWatcher.h"
#include "src/trainer/Trainer.h"
#include "src/trainer/BackpropTrainer.h"
#include "src/trainer/RPropTrainer.h"
#include "src/trainer/MyRPropTrainer.h"
#include "src/trainer/ImprovedRPropTrainer.h"
#include "src/trainer/GeneticTrainer.h"
#include "src/trainer/PSOTrainer.h"
#include "src/callbacks/DebugCallback.h"
#include "src/callbacks/GradientWatcher.h"
#include "src/callbacks/DummyCallback.h"
#include "src/trainer/ImprovedRPropTrainer.h"
#include "src/trainer/ARPropTrainer.h"
#include "src/criteria/BinaryClassificationSuccess.h"
#include "src/callbacks/StateCallback.h"
#include "src/callbacks/SaveNetworkCallback.h"
#include "src/DataSet.h"
#include "src/errorfunctions/ThresholdSquaredErrorFunction.h"
#include "src/errorfunctions/ErrorFunction.h"
#include "src/errorfunctions/SquaredErrorFunction.h"
#include "src/errorfunctions/ThresholdSquaredErrorFunction.h"
#include "src/errorfunctions/CrossEntropyErrorFunction.h"
#include "src/errorfunctions/WinnerTakesAllErrorFunction.h"
#include "src/errorfunctions/ManhattanError.h"
#include "src/errorfunctions/CombinedErrorFunction.h"
#include "src/networks/LSTMNetwork.h"
#include "src/callbacks/ReportCallback.h"
#include "src/callbacks/ZeroErrorFailReport.h"
#include "src/callbacks/ClassificationCallback.h"
#include "src/nodes/TanhNode.h"
#include "src/networks/LSTMNetwork.h"
#include "src/nodes/GaussianNode.h"
#include "src/nodes/LinearNode.h"
#include "src/nodes/SigmoidNode.h"
#include "src/nodes/ReLUNode.h"
#include "src/GnuplotGenerator.h"
#include "src/criteria/Criterion.h"
#include "src/criteria/GeneralizationLoss.h"
#include "src/criteria/ZeroErrorCriterion.h"
#include "src/criteria/ZeroValidationErrorCriterion.h"
#include "src/criteria/ErrorBelowCriterion.h"
#include "src/criteria/ValidationErrorBelowCriterion.h"
#include "src/criteria/ConvergenceCriterion.h"
#include "src/criteria/BinaryClassificationSuccess.h"
#include "src/criteria/EarlyStopping.h"
#include "src/ensembles/FeedforwardEnsemble.h"
#include "src/ensembles/DelayEnsemble.h"
#include "src/ensembles/RecurrentEnsemble.h"
#include "src/ensembles/WinnerTakesAllEnsemble.h"
#include "src/ensembles/SlowEnsemble.h"
#include "src/ensembles/SingleCellEnsemble.h"
#include "src/ensembles/ConditionalBiasEnsemble.h"
#include "src/ensembles/SecondOrderEnsemble.h"
#include "src/callbacks/ReportCallback.h"
#include "src/AutoAssociationSequence.h"
#include "src/HarmonicGateNetwork.h"
#include "src/LSTMCascade.h"
#include "src/ensembles/LSTMForgetEnsemble.h"
#include "src/classification/TimeseriesClassification.h"
#include "src/classification/ConfusionMatrix.h"
#include "src/classification/AutoPredictor.h"
#include "src/callbacks/ReportCallback.h"
#include "src/callbacks/ZeroErrorFailReport.h"
#include "src/callbacks/SaveNetworkCallback.h"
#include "src/callbacks/ErrorEvaluationCallback.h"
#include "src/callbacks/ProgressDot.h"
#include "src/tasks/Task.h"
#include "src/tasks/ClassifyFrequency.h"
#include "src/networks/RecurrentNetwork.h"
#include "src/Connection.h"
#include "src/networks/RandomGateNetwork.h"
#include "src/misc/Jacobian.h"
#include "src/trainer/CGTrainer.h"
#include "src/networks/FeedForwardNetwork.h"
#include "src/classification/CrossValidationError.h"
#include "src/classification/interfaces/ClassificatorInterface.h"
#include "src/classification/interfaces/WinnerTakesAllClassificator.h"
#include "src/classification/interfaces/RecursiveThermometricClassificator.h"
#include "src/trainer/SARPropTrainer.h"
#include "src/trainer/ARPropTrainer.h"
#include "src/callbacks/PickBestGeneralizingNetwork.h"

%}


// Instantiate templates used by example
namespace std {
 %template(IntVector) vector<int>;
 %template(NodeVector) vector<Node*>;
 %template(DoubleVector) vector<double>; 
 %rename(String) string;
}


//%ignore SequenceSet::SequenceSet(const char* filename);

// based on what R does not like...
%ignore SequenceSet::UNDEF_CHARACTER;
%ignore ARPropTrainer::eta_plus;
%ignore ARPropTrainer::eta_minus;
%ignore ARPropTrainer::gamma_min;
%ignore ARPropTrainer::gamma_max;
%ignore MyRPropTrainer::eta_plus;
%ignore MyRPropTrainer::eta_minus;
%ignore MyRPropTrainer::gamma_min;
%ignore MyRPropTrainer::gamma_max;

%include std_string.i
//%include src/functions.h
%include src/wrapper.h
%include src/definitions.h
%include src/Trainable.h
%include src/nodes/Node.h
%include src/nodes/TanhNode.h
%include src/nodes/LinearNode.h
%include src/nodes/SigmoidNode.h
%include src/nodes/ReLUNode.h
%include src/Connection.h
%include src/Network.h
%include src/networks/LSTMNetwork.h
%include src/Sequence.h
%include src/SequenceSet.h
%include src/NetworkFactory.h
%include src/errorfunctions/ErrorFunction.h
%include src/errorfunctions/SquaredErrorFunction.h
%include src/errorfunctions/ThresholdSquaredErrorFunction.h
%include src/errorfunctions/CrossEntropyErrorFunction.h
%include src/errorfunctions/WinnerTakesAllErrorFunction.h
%include src/errorfunctions/ManhattanError.h
%include src/errorfunctions/CombinedErrorFunction.h
%include src/errorfunctions/ThresholdSquaredErrorFunction.h
%include src/trainer/Trainer.h
%include src/trainer/BackpropTrainer.h
%include src/trainer/MyRPropTrainer.h
%include "src/trainer/ImprovedRPropTrainer.h"
%include "src/criteria/Criterion.h"
%include "src/criteria/GeneralizationLoss.h"
%include "src/criteria/ZeroErrorCriterion.h"
%include "src/criteria/ZeroValidationErrorCriterion.h"
%include "src/criteria/ErrorBelowCriterion.h"
%include "src/criteria/ValidationErrorBelowCriterion.h"
%include "src/criteria/ConvergenceCriterion.h"
%include "src/criteria/BinaryClassificationSuccess.h"
%include "src/criteria/EarlyStopping.h"
%include src/ensembles/Ensemble.h
%include src/ensembles/FeedforwardEnsemble.h
%include src/ensembles/LSTMEnsemble.h
%include src/ensembles/WinnerTakesAllEnsemble.h
%include src/ensembles/SecondOrderEnsemble.h
%include "src/ensembles/LSTMForgetEnsemble.h"
%include "src/ensembles/ConditionalBiasEnsemble.h"
%include src/callbacks/CallbackHandler.h
%include "src/callbacks/ReportCallback.h"
%include src/callbacks/ZeroErrorFailReport.h
%include src/callbacks/SaveNetworkCallback.h
%include src/callbacks/ErrorEvaluationCallback.h
%include src/callbacks/ProgressDot.h
%include "src/callbacks/ClassificationCallback.h"
%include src/AutoAssociationSequence.h
%include src/LSTMCascade.h
%include src/HarmonicGateNetwork.h
%include "src/classification/TimeseriesClassification.h"
%include "src/classification/ConfusionMatrix.h"
%include "src/classification/AutoPredictor.h"
%include "src/DataSet.h"
//%include "src/errorfunctions/ClassificationError.h"
%include "src/GnuplotGenerator.h" 
%include "src/networks/RecurrentNetwork.h"
%include "src/networks/SlowNetwork.h"
%include "src/networks/RandomGateNetwork.h"
%include "src/misc/Jacobian.h"
%include "src/trainer/CGTrainer.h"
%include "src/networks/FeedForwardNetwork.h"
%include "src/classification/CrossValidationError.h" 
%include "src/classification/interfaces/ClassificatorInterface.h"
%include "src/classification/interfaces/WinnerTakesAllClassificator.h"
%include "src/classification/interfaces/RecursiveThermometricClassificator.h"
%include "src/classification/interfaces/ClassificatorInterface.h"
%include "src/trainer/SARPropTrainer.h"
%include "src/trainer/ARPropTrainer.h"
%include "src/callbacks/PickBestGeneralizingNetwork.h"
