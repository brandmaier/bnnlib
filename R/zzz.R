#
# complementary SWIG stuff
#

#source("../bnnlib.R")

# add missing
setClass("_p_std__vectorT_std__vectorT_double_std__allocatorT_double_t_t_p_std__allocatorT_std__vectorT_double_std__allocatorT_double_t_t_p_t_t", contains = 'ExternalReference')

setClass('_p_FeedforwardEnsemble', contains = c('ExternalReference','_p_Ensemble'))
setClass('_p_LSTMEnsemble', contains = c('ExternalReference','_p_Ensemble'))

setClass('_p_BackpropTrainer', contains = c('ExternalReference','_p_Trainer'))
setClass('_p_ImprovedRPropTrainer', contains = c('ExternalReference','_p_Trainer'))

setClass('_p_RPropTrainer', contains = c('ExternalReference','_p_Trainer'))
setClass('_p_ARPropTrainer', contains = c('ExternalReference','_p_Trainer'))
setClass('_p_ADAMTrainer', contains = c('ExternalReference','_p_Trainer'))

setClass('_p_ConvergenceCriterion', contains=c('ExternalReference','_p_Criterion'))

setClass('_p_LSTMNetwork', contains = c('ExternalReference','_p_Network'))