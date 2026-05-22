#
# complementary SWIG stuff
#

# Some build environments (notably when SWIG-generated R classes are incomplete)
# do not define ExternalReference before this file is sourced.
# Define a safe fallback so class extensions below can be registered.
if (!methods::isClass("ExternalReference")) {
  methods::setClass("ExternalReference", representation(ref = "externalptr"))
}

# add missing
if (!methods::isClass("_p_std__vectorT_std__vectorT_double_std__allocatorT_double_t_t_p_std__allocatorT_std__vectorT_double_std__allocatorT_double_t_t_p_t_t")) {
  methods::setClass("_p_std__vectorT_std__vectorT_double_std__allocatorT_double_t_t_p_std__allocatorT_std__vectorT_double_std__allocatorT_double_t_t_p_t_t", contains = "ExternalReference")
}

if (!methods::isClass("_p_FeedforwardEnsemble")) methods::setClass("_p_FeedforwardEnsemble", contains = c("ExternalReference", "_p_Ensemble"))
if (!methods::isClass("_p_LSTMEnsemble")) methods::setClass("_p_LSTMEnsemble", contains = c("ExternalReference", "_p_Ensemble"))

if (!methods::isClass("_p_BackpropTrainer")) methods::setClass("_p_BackpropTrainer", contains = c("ExternalReference", "_p_Trainer"))
if (!methods::isClass("_p_ImprovedRPropTrainer")) methods::setClass("_p_ImprovedRPropTrainer", contains = c("ExternalReference", "_p_Trainer"))

if (!methods::isClass("_p_RPropTrainer")) methods::setClass("_p_RPropTrainer", contains = c("ExternalReference", "_p_Trainer"))
if (!methods::isClass("_p_ARPropTrainer")) methods::setClass("_p_ARPropTrainer", contains = c("ExternalReference", "_p_Trainer"))
if (!methods::isClass("_p_ADAMTrainer")) methods::setClass("_p_ADAMTrainer", contains = c("ExternalReference", "_p_Trainer"))

if (!methods::isClass("_p_ConvergenceCriterion")) methods::setClass("_p_ConvergenceCriterion", contains = c("ExternalReference", "_p_Criterion"))

if (!methods::isClass("_p_LSTMNetwork")) methods::setClass("_p_LSTMNetwork", contains = c("ExternalReference", "_p_Network"))

# clean up:
# unload dynamic library once package is unloaded
#
.onUnload <- function(libpath) {
  library.dynam.unload("bnnlib", libpath)
}
