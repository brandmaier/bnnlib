#include <Rcpp.h>
#include "NetworkFactory.h"
#include "Network.h"
#include "nodes/Node.h"

using namespace Rcpp;

namespace {
SEXP make_node_xptr(Node* node) {
  XPtr<Node> ptr(node, false);
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
  Node* node = net->nodes[(size_t)(idx - 1)];
  return make_node_xptr(node);
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
