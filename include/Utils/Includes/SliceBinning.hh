#pragma once

#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <iostream>
#include "TH1D.h"

constexpr int DUMMY_BIN_INDEX = 0;

struct SliceVariable {

  SliceVariable() {}

  SliceVariable( const std::string& name, const std::string& latex_name,
    const std::string& units, const std::string& latex_units )
    : name_( name ), latex_name_( latex_name ), units_( units ),
    latex_units_( latex_units ) {}

  std::string name_; // Name to be used when plotting ROOT histograms
  std::string latex_name_; // Form of the name to be used in a LaTeX document
  std::string units_; // Unit specification for ROOT plotting
  std::string latex_units_; // Unit specification (for LaTeX documents)

};



// Reads a string surrounded by double quotes (") from an input stream
//

std::string get_double_quoted_string( std::istream& in );
std::istream& operator>>( std::istream& in, SliceVariable& svar );
std::ostream& operator<<( std::ostream& out, const SliceVariable& svar );

struct Slice {

  Slice() {}

  // ROOT histogram storing the contents of the slice
  std::unique_ptr< TH1 > hist_;

  // Keys are global bin numbers in hist_ (one-based). Values are the
  // contributing reco bin index (zero-based) as defined in the relevant
  // UniverseMaker configuration
  std::map< int, std::set< size_t > > bin_map_;

  // Indices in the slice_vars_ vector for the "active" SliceVariable
  // definitions used to define axes of the owned TH1
  std::vector< size_t > active_var_indices_;

  // Specification for the "other" relevant SliceVariable values
  struct OtherVariableSpec {

    OtherVariableSpec() {}

    OtherVariableSpec( size_t var_idx, double low, double high )
      : var_index_( var_idx ), low_bin_edge_( low ), high_bin_edge_( high ) {}

    // Index for the relevant SliceVariable definition in the slice_vars_
    // vector
    size_t var_index_;
    // Lower bin edge for the current variable in the current slice
    double low_bin_edge_;
    // Upper bin edge for the current variable in the current slice
    double high_bin_edge_;
  };

  // Specifications for the "other" SliceVariable values used to define
  // the slice
  std::vector< OtherVariableSpec > other_vars_;

};

std::ostream& operator<<( std::ostream& out,
    const Slice::OtherVariableSpec& ovs );

std::ostream& operator<<( std::ostream& out, const Slice& slice );

// Defines "slices" of a possibly multidimensional phase space to use for
// plotting results calculated in terms of reco/true bin counts or functions
// thereof. These slices are represented by ROOT histograms.
class SliceBinning {

  public:

    // Use this constructor if you want to fill the SliceBinning object
    // programmatically (i.e., not via a pre-existing configuration file)
    SliceBinning() {}

    // Construct the SliceBinning object from a saved configuration file
    SliceBinning( const std::string& config_file_name );

    // Prints the current configuration of the object to a std::ostream.
    // This function can be used to create a new configuration file.
    void print_config( std::ostream& os ) const;

  //protected:

    std::vector< SliceVariable > slice_vars_;

    std::vector< Slice > slices_;
};

std::ostream& operator<<( std::ostream& out, const SliceBinning& sb );

