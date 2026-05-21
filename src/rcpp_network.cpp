#include <Rcpp.h>
#include "NetworkFactory.h"
#include "Network.h"
#include "nodes/Node.h"
#include "Sequence.h"
#include "SequenceSet.h"
#include "trainer/Trainer.h"
#include "trainer/BackpropTrainer.h"
#include "trainer/ADAMTrainer.h"
#include "trainer/RMSPropTrainer.h"
#include "trainer/RPropTrainer.h"
#include "trainer/MyRPropTrainer.h"

using namespace Rcpp;

namespace {
SEXP make_node_xptr(Node* node) {
  XPtr<Node> ptr(node, false);
  return ptr;
}

SEXP make_sequence_xptr(Sequence* sequence) {
  XPtr<Sequence> ptr(sequence, true);
  return ptr;
}

SEXP make_trainer_xptr(Trainer* trainer) {
  XPtr<Trainer> ptr(trainer, true);
  return ptr;
}
}

extern "C" SEXP bnn_create_feedforward_network(SEXP in_sizeSEXP, SEXP hid_sizeSEXP, SEXP out_sizeSEXP) {
  int in_size = as<int>(in_sizeSEXP);
  int hid_size = as<int>(hid_sizeSEXP);
  int out_size = as<int>(out_sizeSEXP);
  if (in_size <= 0 || hid_size <= 0 || out_size <= 0) stop("All layer sizes must be positive integers.");
  Network* network = NetworkFactory::createFeedForwardNetwork((unsigned int)in_size, (unsigned int)hid_size, (unsigned int)out_size);
  XPtr<Network> ptr(network, true);
  return ptr;
}

extern "C" SEXP bnn_network_num_nodes(SEXP network_xptr) {
  XPtr<Network> net(network_xptr);
  return wrap((int)net->get_num_nodes());
}

extern "C" SEXP bnn_network_node_names(SEXP network_xptr) {
  XPtr<Network> net(network_xptr);
  return wrap(net->get_node_names());
}

extern "C" SEXP bnn_network_get_node(SEXP network_xptr, SEXP index_one_basedSEXP) {
  XPtr<Network> net(network_xptr);
  int idx = as<int>(index_one_basedSEXP);
  if (idx < 1 || idx > (int)net->nodes.size()) stop("index_one_based is out of range.");
  return make_node_xptr(net->nodes[(size_t)(idx - 1)]);
}

extern "C" SEXP bnn_node_name(SEXP node_xptr) {
  XPtr<Node> node(node_xptr);
  return wrap(node->name);
}

extern "C" SEXP bnn_node_num_incoming(SEXP node_xptr) {
  XPtr<Node> node(node_xptr);
  return wrap((int)node->get_num_incoming_connections());
}

extern "C" SEXP bnn_node_num_outgoing(SEXP node_xptr) {
  XPtr<Node> node(node_xptr);
  return wrap((int)node->get_num_outgoing_connections());
}

extern "C" SEXP bnn_sequence_create() {
  return make_sequence_xptr(new Sequence());
}

extern "C" SEXP bnn_sequence_add(SEXP sequence_xptr, SEXP inputSEXP, SEXP targetSEXP) {
  XPtr<Sequence> sequence(sequence_xptr);
  NumericVector input = as<NumericVector>(inputSEXP);
  NumericVector target = as<NumericVector>(targetSEXP);
  std::vector<weight_t>* in = new std::vector<weight_t>(input.begin(), input.end());
  std::vector<weight_t>* tar = new std::vector<weight_t>(target.begin(), target.end());
  sequence->add(in, tar);
  return R_NilValue;
}

extern "C" SEXP bnn_sequence_size(SEXP sequence_xptr) {
  XPtr<Sequence> sequence(sequence_xptr);
  return wrap((int)sequence->size());
}

extern "C" SEXP bnn_sequence_get_input(SEXP sequence_xptr, SEXP index_one_basedSEXP) {
  XPtr<Sequence> sequence(sequence_xptr);
  int idx = as<int>(index_one_basedSEXP);
  if (idx < 1 || idx > (int)sequence->size()) stop("index_one_based is out of range.");
  return wrap(*sequence->get_input((unsigned int)(idx - 1)));
}

extern "C" SEXP bnn_sequence_get_target(SEXP sequence_xptr, SEXP index_one_basedSEXP) {
  XPtr<Sequence> sequence(sequence_xptr);
  int idx = as<int>(index_one_basedSEXP);
  if (idx < 1 || idx > (int)sequence->size()) stop("index_one_based is out of range.");
  return wrap(*sequence->get_target((unsigned int)(idx - 1)));
}

extern "C" SEXP bnn_sequenceset_create() {
  XPtr<SequenceSet> ptr(new SequenceSet(), true);
  return ptr;
}

extern "C" SEXP bnn_sequenceset_add_sequence(SEXP sequenceset_xptr, SEXP sequence_xptr) {
  XPtr<SequenceSet> set(sequenceset_xptr);
  XPtr<Sequence> seq(sequence_xptr);
  set->add_copy_of_sequence(seq.get());
  return R_NilValue;
}

extern "C" SEXP bnn_sequenceset_size(SEXP sequenceset_xptr) {
  XPtr<SequenceSet> set(sequenceset_xptr);
  return wrap((int)set->size());
}

extern "C" SEXP bnn_sequenceset_get_sequence(SEXP sequenceset_xptr, SEXP index_one_basedSEXP) {
  XPtr<SequenceSet> set(sequenceset_xptr);
  int idx = as<int>(index_one_basedSEXP);
  if (idx < 1 || idx > (int)set->size()) stop("index_one_based is out of range.");
  XPtr<Sequence> ptr(new Sequence(*set->get((unsigned int)(idx - 1))), true);
  return ptr;
}

extern "C" SEXP bnn_create_trainer(SEXP trainer_typeSEXP, SEXP network_xptr) {
  std::string trainer_type = as<std::string>(trainer_typeSEXP);
  XPtr<Network> network(network_xptr);

  if (trainer_type == "backprop") return make_trainer_xptr(new BackpropTrainer(network.get()));
  if (trainer_type == "adam") return make_trainer_xptr(new ADAMTrainer(network.get()));
  if (trainer_type == "rmsprop") return make_trainer_xptr(new RMSPropTrainer(network.get()));
  if (trainer_type == "rprop") return make_trainer_xptr(new RPropTrainer(network.get()));
  if (trainer_type == "myrprop") return make_trainer_xptr(new MyRPropTrainer(network.get()));

  stop("Unsupported trainer_type. Use one of: backprop, adam, rmsprop, rprop, myrprop.");
}

extern "C" SEXP bnn_trainer_name(SEXP trainer_xptr) {
  XPtr<Trainer> trainer(trainer_xptr);
  return wrap(trainer->get_name());
}

extern "C" SEXP bnn_trainer_train(SEXP trainer_xptr, SEXP sequenceset_xptr, SEXP iterationsSEXP) {
  XPtr<Trainer> trainer(trainer_xptr);
  XPtr<SequenceSet> set(sequenceset_xptr);
  int iterations = as<int>(iterationsSEXP);
  if (iterations <= 0) stop("iterations must be a positive integer.");
  trainer->train(set.get(), (unsigned int)iterations);
  return R_NilValue;
}
