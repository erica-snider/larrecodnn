cet_make_library(SOURCE
  TritonClient.cc
  TritonData.cc
  triton_utils.cc
  LIBRARIES
  PUBLIC
  TRITON::grpcclient
  gRPC::grpc # Should be a transitive (INTERFACE) dependency of TRITON::grpcclient
  PRIVATE
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  cetlib_except::cetlib_except
)

install_headers()
install_source()
