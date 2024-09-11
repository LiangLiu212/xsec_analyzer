/*
 * =====================================================================================
 *
 *       Filename:  Utils.cxx
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/16/24 13:47:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Liang Liu (L. Liu), liangliu@fnal.gov
 *		    Fermi National Accelerator Laboratory
 *  Collaboration:  GENIE
 *
 * =====================================================================================
 */

#include "FilePropertiesManager.hh"
#include "SliceBinning.hh"
#include "UniverseMaker.hh"
#include "HistUtils.hh"
#include "ConfigMakerUtils.hh"
#include "WeightHandler.hh"

// Reads a string surrounded by double quotes (") from an input stream
std::string get_double_quoted_string( std::istream& in ) {
  std::string temp_str;
  std::getline( in, temp_str, '\"' );
  std::getline( in, temp_str, '\"' );
  return temp_str;
}

std::istream& operator>>( std::istream& in, SliceVariable& svar ) {
  svar.name_ = get_double_quoted_string( in );
  svar.units_ = get_double_quoted_string( in );
  svar.latex_name_ = get_double_quoted_string( in );
  svar.latex_units_ = get_double_quoted_string( in );
  return in;
}

std::ostream& operator<<( std::ostream& out, const SliceVariable& svar )
{
  out << '\"' << svar.name_ << "\" \"" << svar.units_ << "\" \""
    << svar.latex_name_ << "\" \"" << svar.latex_units_ << '\"';
  return out;
}


std::ostream& operator<<( std::ostream& out,
  const Slice::OtherVariableSpec& ovs )
{
  out << ovs.var_index_ << ' ' << ovs.low_bin_edge_ << ' '
    << ovs.high_bin_edge_;

  return out;
}

std::ostream& operator<<( std::ostream& out, const Slice& slice )
{
  // TODO: add handling for multidimensional slices
  std::string final_axis_label = slice.hist_->GetYaxis()->GetTitle();
  size_t num_active_vars = slice.active_var_indices_.size();
  if ( num_active_vars != 1u ) throw std::runtime_error( "Support for"
    " multidimensional slices has not yet been implemented" );

  out << '\"' << final_axis_label << "\"\n";
  out << num_active_vars;

  size_t avar_idx = slice.active_var_indices_.front();
  int num_edges = slice.hist_->GetNbinsX() + 1;
  out << ' ' << avar_idx << ' ' << num_edges;

  // Note that ROOT TH1 bins have one-based indices (bin zero is used for
  // underflow). By using num_edges as the maximum bin index in the loop below,
  // we also get the lower edge of the overflow bin (equivalent to the upper
  // edge of the last regular bin).
  for ( int bin = 1; bin <= num_edges; ++bin ) {
    out << ' ' << slice.hist_->GetBinLowEdge( bin );
  }
  out << '\n';

  size_t num_other_vars = slice.other_vars_.size();
  out << num_other_vars;
  if ( num_other_vars == 0 ) out << '\n';
  else out << ' ';

  for ( const auto& ovar_spec : slice.other_vars_ ) out << ovar_spec << '\n';

  // We need to "invert" the bin map in order for it to have the correct
  // structure for dumping the configuration easily. Here we'll manually
  // build a map from analysis bins to ROOT histogram bins in the slice.
  // TODO: consider other solutions
  std::map< size_t, std::vector< int > > analysis_to_root_bin_map;
  for ( const auto& bin_pair : slice.bin_map_ ) {
    int root_bin_idx = bin_pair.first;
    const auto& analysis_bin_set = bin_pair.second;

    for ( const size_t& analysis_bin_idx : analysis_bin_set ) {
      // Get access to the vector of ROOT histogram bins for the current
      // analysis bin. Note that the call to std::map::operator[] will create
      // a map entry (with an empty vector) if one does not already exist.
      auto& root_bin_vec = analysis_to_root_bin_map[ analysis_bin_idx ];
      root_bin_vec.push_back( root_bin_idx );
    }
  }

  // TODO: Generalize this for multidimensional slices. At present, we
  // assume here that the *global* ROOT bin numbers (as stored in bin_map_)
  // correspond to the active variable bin numbers on the x-axis. This is
  // only reliably true for 1D histograms, so we're currently cheating a bit.
  size_t num_analysis_bins = analysis_to_root_bin_map.size();
  out << num_analysis_bins;

  for ( const auto& ana_bin_pair : analysis_to_root_bin_map ) {
    const size_t ana_bin_idx = ana_bin_pair.first;
    const auto& root_bin_vec = ana_bin_pair.second;
    size_t num_root_bins = root_bin_vec.size();

    out << '\n' << ana_bin_idx << ' ' << num_root_bins;
    for ( const auto& rbin_idx : root_bin_vec ) {
      out << ' ' << rbin_idx;
    }
  }

  return out;
}


SliceBinning::SliceBinning( const std::string& config_file_name ) {
  std::cout << "\nInitialising SliceBinning object" << std::endl;
  std::cout << "\tconfig_file_name: " << config_file_name << std::endl;

  std::ifstream in_file( config_file_name );

  // First read in the number of slice variables defined for this binning
  // scheme
  int num_variables;
  in_file >> num_variables;

  std::cout << "\tNumber of variables used in slicing: " << num_variables << '\n';

  // Get the slice variable specifications from the configuration file
  SliceVariable temp_sv;
  for ( int v = 0; v < num_variables; ++v ) {
    in_file >> temp_sv;
    slice_vars_.push_back ( temp_sv );
  }

  // Read in the number of slices defined for this binning scheme
  int num_slices;
  in_file >> num_slices;
  std::cout << "\tNumber of slices requested: " << num_slices << std::endl;
  std::cout << "\nSlice Specifications ---------------- " << std::endl;

  // Get the slice specifications from the configuration file
  for ( int s = 0; s < num_slices; ++s ) {

    std::cout << "\nSlice Index: " << s << std::endl;

    // Get the label for the final axis
    std::string final_axis_label = get_double_quoted_string( in_file );

    // Number of variables for which the current slice provides explicit ROOT
    // histogram bins
    int num_active_variables;
    in_file >> num_active_variables;

    if ( num_active_variables != 1 ) { // || num_active_variables > 2 )
      throw std::runtime_error( "Handling of "
        + std::to_string(num_active_variables) + " active variables for a"
        " slice is unimplemented!" );
    }

    // Keys are indices for the active variable(s) in the slice_vars_ vector.
    // Values are vectors of TH1-style bin edges.
    std::map< size_t, std::vector<double> > edge_map;
    for ( int av = 0; av < num_active_variables; ++av ) {
      size_t var_idx;
      int num_edges;
      in_file >> var_idx >> num_edges;
      int num_bins = num_edges - 1;

      std::cout << "\tActive variable: " << slice_vars_.at( var_idx ).name_ << std::endl;
      std::cout << "\tNumber of bins: " << num_bins << " - " << std::endl;

      // Create a new entry in the map of edges for the current active variable
      // TODO: add error handling for the case where a duplicate active
      // variable index appears in the input
      auto& edge_vec = edge_map[ var_idx ] = std::vector<double>();

      // We have one more edge listed than the number of bins. This allows the
      // upper edge of the last bin to be specified
      for ( int e = 0; e < num_edges; ++e ) {
        double edge;
        in_file >> edge;
        edge_vec.push_back( edge );
      }

      for ( int b = 1; b <= num_bins; ++b ) {
        std::cout << "\t\tBin " << b << ": " << edge_vec.at( b - 1 )
          << ' ' << slice_vars_.at( var_idx ).units_ << " \u2264 "
          << slice_vars_.at( var_idx ).name_ << " < "
          << edge_vec.at( b )
          << ' ' << slice_vars_.at( var_idx ).units_ << '\n';
      }
    }

    // Create an object to represent the current slice
    Slice cur_slice;

    // Read in the specifications for the other (i.e., inactive) variables
    int num_other_variables;
    in_file >> num_other_variables;

    for ( int ov = 0; ov < num_other_variables; ++ov ) {
      Slice::OtherVariableSpec ovs;
      in_file >> ovs.var_index_ >> ovs.low_bin_edge_ >> ovs.high_bin_edge_;

      std::cout << "\tSlice has other variable " << slice_vars_.at( ovs.var_index_ ).name_ << " : [" << ovs.low_bin_edge_ << ", " << ovs.high_bin_edge_ << ")" << std::endl;

      cur_slice.other_vars_.push_back( ovs );
    }

    // We're ready to build the ROOT histogram owned by the current slice, so
    // do it now before matching ROOT bins with UniverseMaker bins. Start
    // by building the plot title using the bin limits for the "other"
    // variables.
    std::string slice_title = "slice " + std::to_string( slices_.size() );
    if ( !cur_slice.other_vars_.empty() ) {
      bool first_other_var = true;
      for ( const auto& ovs : cur_slice.other_vars_ ) {
        if ( first_other_var ) {
          slice_title += ": ";
          first_other_var = false;
        }
        else slice_title += ", ";

        // Specify the limits for the current variable in the histogram title
        const auto& var_spec = slice_vars_.at( ovs.var_index_ );
        // Use a std::stringstream to easily get reasonable precision on
        // the numerical bin limits
        std::stringstream temp_ss;
        temp_ss << ovs.low_bin_edge_ << ' ' << var_spec.units_ << " #leq "
          << var_spec.name_ << " < " << ovs.high_bin_edge_
          << ' ' << var_spec.units_;

        slice_title += temp_ss.str();
      }
    }

    // Now label the axes appropriately with the active variable names and
    // units. Retrieve the active variable indices in the slice_vars_ vector
    // from the edge_map.
    for ( const auto& pair : edge_map ) {
      size_t var_idx = pair.first;
      const auto& var_spec = slice_vars_.at( var_idx );

      slice_title += "; " + var_spec.name_;
      if ( !var_spec.units_.empty() ) {
        slice_title += " (" + var_spec.units_ + ')';
      }

      // While we're at it, store the active variable indices in the slice
      // for later retrieval after the edge_map goes out of scope
      cur_slice.active_var_indices_.push_back( var_idx );
    }

    // Name the slice histogram
    std::string slice_hist_name = "slice_" + std::to_string( s );

    // Also add the label for the final axis to the title
    slice_title += ';' + final_axis_label;

    if ( num_active_variables == 1 ) {
      // Retrieve the vector of bin edges for the only active variable
      const auto& bin_edges = edge_map.cbegin()->second;
      int num_bins = bin_edges.size() - 1;

      std::unique_ptr< TH1 > temp_hist(
        new TH1D( slice_hist_name.c_str(), slice_title.c_str(), num_bins,
          bin_edges.data() )
      );

      // Move the ready-to-use histogram into the member smart pointer
      cur_slice.hist_.swap( temp_hist );
    }
    //else if ( num_active_variables == 2 ) {
    // TODO: finish
    //}

    // Prevent auto-deletion of the histogram by ROOT by disassociating
    // it from the current directory
    cur_slice.hist_->SetDirectory( nullptr );

    // Also get rid of the default display of the stats box
    cur_slice.hist_->SetStats( false );

    // Now that we have the histogram made, read in the information needed to
    // map from each UniverseMaker reco bin that contributes to this
    // slice to a TH1 global bin number (conveniently converted from x, y, ...
    // bin indices by the new histogram)
    int num_rmm_bins;
    in_file >> num_rmm_bins;

    std::cout << "\tBin matching between slice TH1 and global TH1 - " << std::endl;

    for ( int cb = 0; cb < num_rmm_bins; ++cb ) {

      // Index of the appropriate reco bin owned by a UniverseMaker
      // object. This index is zero-based.
      size_t rmm_reco_bin_idx;

      // Number of TH1 bins in the current slice to which the
      // current UniverseMaker bin contributes
      size_t num_root_bins;

      in_file >> rmm_reco_bin_idx >> num_root_bins;

      for ( int rtb = 0; rtb < num_root_bins; ++rtb ) {

        // Store the bin indices along each defined axis
        std::vector< int > av_bin_indices;
        for ( int a = 0; a < num_active_variables; ++a ) {
          int idx;
          in_file >> idx;
          av_bin_indices.push_back( idx );
        }

        // Pad with zeros for y and z if needed so that we can use the
        // same function call to get the global bin number for a one-, two-,
        // or three-dimensional histogram
        int pad_entries = 3 - av_bin_indices.size();
        for ( int p = 0; p < pad_entries; ++p ) {
          av_bin_indices.push_back( DUMMY_BIN_INDEX );
        }

        // We're ready. Look up the global bin index in the slice histogram
        // that should be associated with the current UniverseMaker
        // reco bin
        int root_bin_idx = cur_slice.hist_->GetBin(
          av_bin_indices.front(), av_bin_indices.at(1), av_bin_indices.at(2) );

        // Store the indices for both kinds of bins at the appropriate place
        // in the bin map
        auto iter = cur_slice.bin_map_.find( root_bin_idx );
        if ( iter == cur_slice.bin_map_.end() ) {
          cur_slice.bin_map_[ root_bin_idx ]
            = std::set< size_t > { rmm_reco_bin_idx };
        }
        else iter->second.insert( rmm_reco_bin_idx );

        std::cout << "\t\tGlobal bin " << rmm_reco_bin_idx << " is matched to " << root_bin_idx << " in this slice\n";

      } // ROOT bins

    } // UniverseMaker bins

    // Move the completed Slice object into the vector of slices
    slices_.emplace_back( std::move(cur_slice) );

  } // slices

}

void SliceBinning::print_config( std::ostream& out ) const {

  size_t num_variables = slice_vars_.size();
  out << num_variables << '\n';

  for ( const auto& svar : slice_vars_ ) out << svar << '\n';

  size_t num_slices = slices_.size();
  out << num_slices;

  for ( const auto& slice : slices_ ) out << '\n' << slice;
}


std::ostream& operator<<( std::ostream& out, const SliceBinning& sb ) {
  sb.print_config( out );
  return out;
}


// Converts the name of an analysis ntuple file (typically with the full path)
// into a TDirectoryFile name to use as a subfolder of the main output
// TDirectoryFile used for saving response matrices. Since the forward slash
// character '/' cannot be used in a TDirectoryFile name, this function
// replaces all instances of this character by a '+' instead. The technique
// used here is based on https://stackoverflow.com/a/2896627/4081973
std::string ntuple_subfolder_from_file_name( const std::string& file_name ) {
  constexpr char FORWARD_SLASH = '/';
  constexpr char PLUS = '+';

  std::string result = file_name;
  std::replace( result.begin(), result.end(), FORWARD_SLASH, PLUS );
  return result;
}



// Event weights that are below MIN_WEIGHT, above MAX_WEIGHT, infinite, or NaN
// are reset to unity by this function. Other weights are returned unaltered.
inline double safe_weight( double w ) {
  if ( std::isfinite(w) && w >= MIN_WEIGHT && w <= MAX_WEIGHT ) return w;
  else return 1.0;
}

// Utility function used to check endings of (trimmed) weight labels based on
// branch names in the weights TTree
bool string_has_end( const std::string& str, const std::string& end ) {
  if ( str.length() >= end.length() ) {
    int comp = str.compare( str.length() - end.length(),
      end.length(), end );
    bool test_result = ( comp == 0 );
    return test_result;
  }
  return false;
}

// Multiplies a given event weight by extra correction factors as appropriate.
// TODO: include the rootino_fix weight as a correction to the central value
void apply_cv_correction_weights( const std::string& wgt_name,
  double& wgt, double spline_weight, double tune_weight )
{
  if ( string_has_end(wgt_name, "UBGenie") ) {
    wgt *= spline_weight;
  }
  else if ( wgt_name == "weight_flux_all"
    || wgt_name == "weight_reint_all"
    || wgt_name == "weight_xsr_scc_Fa3_SCC"
    || wgt_name == "weight_xsr_scc_Fv3_SCC" )
  {
    wgt *= spline_weight * tune_weight;
  }
  else if ( wgt_name == SPLINE_WEIGHT_NAME ) {
    // No extra weight factors needed
    return;
  }
  else throw std::runtime_error( "Unrecognized weight name" );
}


UniverseMaker::UniverseMaker( const std::string& config_file_name )
{
  std::ifstream in_file( config_file_name );

  // Load the root TDirectoryFile name to use when writing the response
  // matrices to an output ROOT file
  in_file >> output_directory_name_;

  // Load the TTree name to use when processing ntuple input files
  std::string ttree_name;
  in_file >> ttree_name;

  // Initialize the owned input TChain with the configured TTree name
  input_chain_.SetName( ttree_name.c_str() );

  // Load the true bin definitions
  size_t num_true_bins;
  in_file >> num_true_bins;

  for ( size_t tb = 0u; tb < num_true_bins; ++tb ) {
    TrueBin temp_bin;
    in_file >> temp_bin;

    /*
    // DEBUG
    std::cout << "tb = " << tb << '\n';
    std::cout << temp_bin << '\n';
    */

    true_bins_.push_back( temp_bin );
  }

  // Load the reco bin definitions
  size_t num_reco_bins;
  in_file >> num_reco_bins;
  for ( size_t rb = 0u; rb < num_reco_bins; ++rb ) {
    RecoBin temp_bin;
    in_file >> temp_bin;

    /*
    // DEBUG
    std::cout << "rb = " << rb << '\n';
    std::cout << temp_bin << '\n';
    */

    reco_bins_.push_back( temp_bin );
  }

}

void UniverseMaker::add_input_file( const std::string& input_file_name )
{
  // Check to make sure that the input file contains the expected ntuple
  TFile temp_file( input_file_name.c_str(), "read" );

  // Temporary storage
  TTree* temp_tree;

  std::string tree_name = input_chain_.GetName();
  temp_file.GetObject( tree_name.c_str(), temp_tree );
  if ( !temp_tree ) throw std::runtime_error( "Missing ntuple TTree "
    + tree_name + " in the input ntuple file " + input_file_name );

  // If we've made it here, then the input file has passed all of the checks.
  // Add it to the input TChain.
  input_chain_.AddFile( input_file_name.c_str() );
}

void UniverseMaker::prepare_formulas() {

  // Remove any pre-existing TTreeFormula objects from the owned vectors
  true_bin_formulas_.clear();
  reco_bin_formulas_.clear();
  category_formulas_.clear();

  // Get the number of defined EventCategory values by checking the number
  // of elements in the "label map" managed by the EventCategoryInterpreter
  // singleton class
  const auto& eci = EventCategoryInterpreter::Instance();
  size_t num_categories = eci.label_map().size();

  // Create one TTreeFormula for each true bin definition
  for ( size_t tb = 0u; tb < true_bins_.size(); ++tb ) {
    const auto& bin_def = true_bins_.at( tb );
    std::string formula_name = "true_formula_" + std::to_string( tb );

    auto tbf = std::make_unique< TTreeFormula >( formula_name.c_str(),
      bin_def.signal_cuts_.c_str(), &input_chain_ );

    tbf->SetQuickLoad( true );

    true_bin_formulas_.emplace_back( std::move(tbf) );
  }

  // Create one TTreeFormula for each reco bin definition
  for ( size_t rb = 0u; rb < reco_bins_.size(); ++rb ) {
    const auto& bin_def = reco_bins_.at( rb );
    std::string formula_name = "reco_formula_" + std::to_string( rb );

    auto rbf = std::make_unique< TTreeFormula >( formula_name.c_str(),
      bin_def.selection_cuts_.c_str(), &input_chain_ );

    rbf->SetQuickLoad( true );

    reco_bin_formulas_.emplace_back( std::move(rbf) );
  }

  // Create one TTreeFormula for each true EventCategory
  const auto& category_map = eci.label_map();
  for ( const auto& category_pair : category_map ) {

    EventCategory cur_category = category_pair.first;
    std::string str_category = std::to_string( cur_category );

    std::string category_formula_name = "category_formula_" + str_category;

    //DB - Problematic
    std::string category_cuts = "CC1muXp0pi_EventCategory == " + str_category;

    auto cbf = std::make_unique< TTreeFormula >(
      category_formula_name.c_str(), category_cuts.c_str(), &input_chain_ );

    cbf->SetQuickLoad( true );

    category_formulas_.emplace_back( std::move(cbf) );
  }

}

void UniverseMaker::build_universes(
  const std::vector<std::string>& universe_branch_names )
{
  return this->build_universes( &universe_branch_names );
}

void UniverseMaker::build_universes(
  const std::vector<std::string>* universe_branch_names )
{
  int num_input_files = input_chain_.GetListOfFiles()->GetEntries();
  if ( num_input_files < 1 ) {
    std::cout << "ERROR: The UniverseMaker object has not been"
      " initialized with any input files yet.\n";
    return;
  }

  WeightHandler wh;
  wh.set_branch_addresses( input_chain_, universe_branch_names );

  // Make sure that we always have branches set up for the CV correction
  // weights, i.e., the spline and tune weights. Don't throw an exception if
  // these are missing in the input TTree (we could be working with real data)
  wh.add_branch( input_chain_, SPLINE_WEIGHT_NAME, false );
  wh.add_branch( input_chain_, TUNE_WEIGHT_NAME, false );

  this->prepare_formulas();

  // Set up storage for the "is_mc" boolean flag branch. If we're not working
  // with MC events, then we shouldn't do anything with the true bin counts.
  bool is_mc;
  input_chain_.SetBranchAddress( "is_mc", &is_mc );

  // Get the first TChain entry so that we can know the number of universes
  // used in each vector of weights
  input_chain_.GetEntry( 0 );

  // Now prepare the vectors of Universe objects with the correct sizes
  this->prepare_universes( wh );

  int treenumber = 0;
  for ( long long entry = 0; entry < input_chain_.GetEntries(); ++entry ) {
    // Load the TTree for the current TChain entry
    input_chain_.LoadTree( entry );

    // If the current entry is in a new TTree, then have all of the
    // TTreeFormula objects make the necessary updates
    if ( treenumber != input_chain_.GetTreeNumber() ) {
      treenumber = input_chain_.GetTreeNumber();
      for ( auto& tbf : true_bin_formulas_ ) tbf->Notify();
      for ( auto& rbf : reco_bin_formulas_ ) rbf->Notify();
      for ( auto& cbf : category_formulas_ ) cbf->Notify();
    }

    // Find the reco bin(s) that should be filled for the current event
    std::vector< FormulaMatch > matched_reco_bins;
    for ( size_t rb = 0u; rb < reco_bin_formulas_.size(); ++rb ) {
      auto& rbf = reco_bin_formulas_.at( rb );
      int num_formula_elements = rbf->GetNdata();
      for ( int el = 0; el < num_formula_elements; ++el ) {
        double formula_wgt = rbf->EvalInstance( el );
        if ( formula_wgt ) matched_reco_bins.emplace_back( rb, formula_wgt );
      }
    }

    // Find the EventCategory label(s) that apply to the current event
    std::vector< FormulaMatch > matched_category_indices;
    for ( size_t c = 0u; c < category_formulas_.size(); ++c ) {
      auto& cbf = category_formulas_.at( c );
      int num_formula_elements = cbf->GetNdata();
      for ( int el = 0; el < num_formula_elements; ++el ) {
        double formula_wgt = cbf->EvalInstance( el );
        if ( formula_wgt ) {
          matched_category_indices.emplace_back( c, formula_wgt );
        }
      }
    }

    input_chain_.GetEntry( entry );

    std::vector< FormulaMatch > matched_true_bins;
    double spline_weight = 0.;
    double tune_weight = 0.;

    // If we're working with an MC sample, then find the true bin(s)
    // that should be filled for the current event
    if ( is_mc ) {
      for ( size_t tb = 0u; tb < true_bin_formulas_.size(); ++tb ) {
        auto& tbf = true_bin_formulas_.at( tb );
        int num_formula_elements = tbf->GetNdata();
        for ( int el = 0; el < num_formula_elements; ++el ) {
          double formula_wgt = tbf->EvalInstance( el );
          if ( formula_wgt ) matched_true_bins.emplace_back( tb, formula_wgt );
        }
      } // true bins

      // If we have event weights in the map at all, then get the current
      // event's CV correction weights here for potentially frequent re-use
      // below
      auto& wm = wh.weight_map();
      if ( wm.size() > 0u ) {
        spline_weight = wm.at( SPLINE_WEIGHT_NAME )->front();
        tune_weight = wm.at( TUNE_WEIGHT_NAME )->front();
      }
    } // MC event

    for ( const auto& pair : wh.weight_map() ) {
      const std::string& wgt_name = pair.first;
      const auto& wgt_vec = pair.second;

      auto& u_vec = universes_.at( wgt_name );

      for ( size_t u = 0u; u < wgt_vec->size(); ++u ) {

        // No need to use the slightly slower "at" here since we're directly
        // looping over the weight vector
        double w = wgt_vec->operator[]( u );

        // Multiply by any needed CV correction weights
        apply_cv_correction_weights( wgt_name, w, spline_weight, tune_weight );

        // Deal with NaNs, etc. to make a "safe weight" in all cases
        double safe_wgt = safe_weight( w );

        // Get the universe object that should be filled with the processed
        // event weight
        auto& universe = u_vec.at( u );

        for ( const auto& tb : matched_true_bins ) {
          // TODO: consider including the TTreeFormula weight(s) in the check
          // applied via safe_weight() above
          universe.hist_true_->Fill( tb.bin_index_, tb.weight_ * safe_wgt );
          for ( const auto& rb : matched_reco_bins ) {
            universe.hist_2d_->Fill( tb.bin_index_, rb.bin_index_,
              tb.weight_ * rb.weight_ * safe_wgt );
          } // reco bins

          for ( const auto& other_tb : matched_true_bins ) {
            universe.hist_true2d_->Fill( tb.bin_index_, other_tb.bin_index_,
              tb.weight_ * other_tb.weight_ * safe_wgt );
          } // true bins

        } // true bins

        for ( const auto& rb : matched_reco_bins ) {
          universe.hist_reco_->Fill( rb.bin_index_, rb.weight_ * safe_wgt );

          for ( const auto& c : matched_category_indices ) {
            universe.hist_categ_->Fill( c.bin_index_, rb.bin_index_,
              c.weight_ * rb.weight_ * safe_wgt );
          }

          for ( const auto& other_rb : matched_reco_bins ) {
            universe.hist_reco2d_->Fill( rb.bin_index_, other_rb.bin_index_,
              rb.weight_ * other_rb.weight_ * safe_wgt );
          }
        } // reco bins
      } // universes
    } // weight names

    // Fill the unweighted histograms now that we're done with the
    // weighted ones. Note that "unweighted" in this context applies to
    // the universe event weights, but that any implicit weights from
    // the TTreeFormula evaluations will still be applied.
    auto& univ = universes_.at( UNWEIGHTED_NAME ).front();
    for ( const auto& tb : matched_true_bins ) {
      univ.hist_true_->Fill( tb.bin_index_, tb.weight_ );
      for ( const auto& rb : matched_reco_bins ) {
        univ.hist_2d_->Fill( tb.bin_index_, rb.bin_index_,
          tb.weight_ * rb.weight_ );
      } // reco bins

      for ( const auto& other_tb : matched_true_bins ) {
        univ.hist_true2d_->Fill( tb.bin_index_, other_tb.bin_index_,
          tb.weight_ * other_tb.weight_ );
      } // true bins

    } // true bins

    for ( const auto& rb : matched_reco_bins ) {

      univ.hist_reco_->Fill( rb.bin_index_, rb.weight_ );

      for ( const auto& c : matched_category_indices ) {
        univ.hist_categ_->Fill( c.bin_index_, rb.bin_index_,
          c.weight_ * rb.weight_ );
      }

      for ( const auto& other_rb : matched_reco_bins ) {
        univ.hist_reco2d_->Fill( rb.bin_index_, other_rb.bin_index_,
          rb.weight_ * other_rb.weight_ );
      }

    } // reco bins

  } // TChain entries

  input_chain_.ResetBranchAddresses();
}

void UniverseMaker::prepare_universes( const WeightHandler& wh ) {

  size_t num_true_bins = true_bins_.size();
  size_t num_reco_bins = reco_bins_.size();

  for ( const auto& pair : wh.weight_map() ) {
    const std::string& weight_name = pair.first;
    size_t num_universes = pair.second->size();

    std::vector< Universe > u_vec;

    for ( size_t u = 0u; u < num_universes; ++u ) {
      u_vec.emplace_back( weight_name, u, num_true_bins, num_reco_bins );
    }

    universes_[ weight_name ] = std::move( u_vec );
  }

  // Add the special "unweighted" universe unconditionally
  std::vector< Universe > temp_uvec;
  temp_uvec.emplace_back( UNWEIGHTED_NAME, 0, num_true_bins, num_reco_bins );
  universes_[ UNWEIGHTED_NAME ] = std::move( temp_uvec );

}

void UniverseMaker::save_histograms(
  const std::string& output_file_name,
  const std::string& subdirectory_name,
  bool update_file )
{
  // Decide whether to overwrite the output file or simply update the contents.
  // This difference is only important if the output file already exists before
  // this function is called.
  std::string tfile_option( "recreate" );
  if ( update_file ) {
    tfile_option = "update";
  }
  TFile out_file( output_file_name.c_str(), tfile_option.c_str() );

  // Navigate to the subdirectory within the output ROOT file where the
  // response matrix histograms will be saved. Create new TDirectoryFile
  // objects as needed.
  TDirectoryFile* root_tdir = nullptr;
  TDirectoryFile* sub_tdir = nullptr;

  out_file.GetObject( output_directory_name_.c_str(), root_tdir );
  if ( !root_tdir ) {
    // TODO: add error handling for a forward slash in the root TDirectoryFile
    // name
    root_tdir = new TDirectoryFile( output_directory_name_.c_str(),
      "response matrices", "", &out_file );
  }

  // Save the configuration settings for this class to the main
  // TDirectoryFile before moving on to the appropriate subdirectory. If
  // these settings have already been saved, then double-check that they
  // match the current configuration. In the event of a mismatch, throw an
  // exception to avoid data corruption.
  std::string tree_name, true_bin_spec, reco_bin_spec;
  tree_name = this->input_chain().GetName();

  std::ostringstream oss_true, oss_reco;

  for ( const auto& tbin : true_bins_ ) {
    oss_true << tbin << '\n';
  }

  for ( const auto& rbin : reco_bins_ ) {
    oss_reco << rbin << '\n';
  }

  true_bin_spec = oss_true.str();
  reco_bin_spec = oss_reco.str();

  std::string* saved_tree_name = nullptr;
  std::string* saved_tb_spec = nullptr;
  std::string* saved_rb_spec = nullptr;
  root_tdir->GetObject( "ntuple_name", saved_tree_name );
  root_tdir->GetObject( TRUE_BIN_SPEC_NAME.c_str(), saved_tb_spec );
  root_tdir->GetObject( RECO_BIN_SPEC_NAME.c_str(), saved_rb_spec );

  if ( saved_tree_name ) {
    if ( tree_name != *saved_tree_name ) {
      throw std::runtime_error( "Tree name mismatch: " + tree_name
        + " vs. " + *saved_tree_name );
    }
  }
  else {
    root_tdir->WriteObject( &tree_name, "ntuple_name" );
  }

  if ( saved_tb_spec ) {
    if ( true_bin_spec != *saved_tb_spec ) {
      throw std::runtime_error( "Inconsistent true bin specification!" );
    }
  }
  else {
    root_tdir->WriteObject( &true_bin_spec, TRUE_BIN_SPEC_NAME.c_str() );
  }

  if ( saved_rb_spec ) {
    if ( reco_bin_spec != *saved_rb_spec ) {
      throw std::runtime_error( "Inconsistent reco bin specification!" );
    }
  }
  else {
    root_tdir->WriteObject( &reco_bin_spec, RECO_BIN_SPEC_NAME.c_str() );
  }

  std::string subdir_name = ntuple_subfolder_from_file_name(
    subdirectory_name );

  root_tdir->GetObject( subdir_name.c_str(), sub_tdir );
  if ( !sub_tdir ) {
    sub_tdir = new TDirectoryFile( subdir_name.c_str(), "response matrices",
      "", root_tdir );
  }

  // Now we've found (or created) the TDirectoryFile where the output
  // will be saved. Ensure that it is the active file here before writing
  // out the histograms.
  sub_tdir->cd();

  for ( auto& pair : universes_ ) {
    auto& u_vec = pair.second;
    for ( auto& univ : u_vec ) {
      // Always save the reco histograms
      univ.hist_reco_->Write();
      univ.hist_reco2d_->Write();

      // Save the others if the true histogram was filled at least once
      // (used to infer that we have MC truth information)
      if ( univ.hist_true_->GetEntries() > 0. ) {
        univ.hist_true_->Write();
        univ.hist_2d_->Write();
        univ.hist_categ_->Write();
        univ.hist_true2d_->Write();
      }
    } // universes
  } // weight names
}


void WeightHandler::set_branch_addresses( TTree& in_tree,
  const std::vector<std::string>* branch_names )
{
  // Delete any pre-existing contents of the weight map
  weight_map_.clear();

  // Loop over each of the branches of the input TTree
  auto* lob = in_tree.GetListOfBranches();
  for ( int b = 0; b < lob->GetEntries(); ++b ) {

    auto* branch = dynamic_cast< TBranch* >( lob->At(b) );
    std::string br_name = branch->GetName();

    // If the user specified a vector of branch names when calling this
    // function, then check the current branch against them to decide whether
    // it should be included.
    bool include_branch = false;
    if ( branch_names ) {
      // Include the branch if its name can be found in the user-supplied
      // vector
      auto iter = std::find( branch_names->begin(),
        branch_names->end(), br_name );
      include_branch = ( iter != branch_names->end() );
    }
    else {
      // The user didn't supply a vector of branch names, so resort to
      // automatic detection. Include any branch whose name begins with
      // the string "weight_".
      const std::string wgt_br_prefix = "weight_";
      int compare_result = br_name.compare( 0, wgt_br_prefix.size(),
        wgt_br_prefix );
      include_branch = ( compare_result == 0 );
    }

    // Skip to the next branch name if we don't need to include it
    if ( !include_branch ) continue;

    // Assume that all included branches store a std::vector<double> object.
    // Set the branch address so that the vector can accept input from the
    // TTree.
    weight_map_[ br_name ] = MyPointer< std::vector<double> >();

    auto& wgt_vec = weight_map_.at( br_name );
    set_object_input_branch_address( in_tree, br_name, wgt_vec );

  }

  // TODO: add warning or exception for branches listed in the input vector
  // that could not be found in the TTree?
}

void WeightHandler::set_branch_addresses( TTree& in_tree,
  const std::string& branch_name )
{
  std::vector< std::string > br_names = { branch_name };
  set_branch_addresses( in_tree, &br_names );
}

// Allow the user to manually add a new branch
void WeightHandler::add_branch( TTree& in_tree,
  const std::string& branch_name, bool throw_when_missing )
{
  // If we already have an entry in the map for this branch, just return
  // without doing anything
  auto iter = weight_map_.find( branch_name );
  if ( iter != weight_map_.end() ) return;

  // Don't bother to include the new branch if it doesn't actually exist in the
  // input TTree
  TBranch* br = in_tree.GetBranch( branch_name.c_str() );
  if ( !br ) {
    if ( throw_when_missing ) throw std::runtime_error(
      "Missing TTree branch " + branch_name );
    return;
  }

  // Set up the new branch assuming that it holds a vector of double values
  weight_map_[ branch_name ] = MyPointer< std::vector<double> >();
  auto& wgt_vec = weight_map_.at( branch_name );
  set_object_input_branch_address( in_tree, branch_name, wgt_vec );
}


// Helper function used to get the indices for the variables used to define a
// SliceBinning object
int find_slice_var_index( const std::string& name,
  const std::vector< SliceVariable >& sv_vec )
{
  constexpr int BOGUS_INDEX = -1;
  auto iter = std::find_if( sv_vec.cbegin(), sv_vec.cend(),
    [ &name ]( const SliceVariable& svar )
      -> bool { return name == svar.name_; }
  );
  if ( iter == sv_vec.cend() ) return BOGUS_INDEX;
  else return std::distance( sv_vec.cbegin(), iter );
}

// Helper function for adding a new slice to a SliceBinning object. This
// is useful when defining slices simultaneously with a binning scheme.
Slice& add_slice( SliceBinning& sb, const std::vector< double >& bin_edges_1d,
  int active_var_idx, int other_var_idx, double other_low,
  double other_high )
{
  sb.slices_.emplace_back();
  auto& new_slice = sb.slices_.back();

  // Create the slice histogram
  int num_bins = bin_edges_1d.size() - 1;
  TH1D* slice_hist = new TH1D( "slice_hist", ";;events", num_bins,
    bin_edges_1d.data() );
  slice_hist->SetDirectory( nullptr );
  new_slice.hist_.reset( slice_hist );

  // Also set up the slice variable definitions
  new_slice.active_var_indices_.push_back( active_var_idx );

  if ( other_var_idx >= 0 ) {
    new_slice.other_vars_.emplace_back( other_var_idx, other_low, other_high );
  }

  return new_slice;
}

Slice& add_slice( SliceBinning& sb, int num_bins, double active_low,
  double active_high, int active_var_idx, int other_var_idx,
  double other_low , double other_high  )
{
  auto edges = get_bin_low_edges( active_low, active_high, num_bins );

  return add_slice( sb, edges, active_var_idx, other_var_idx, other_low,
    other_high );
}


// Generates a vector of bin low edges equivalent to the approach used by the
// TH1 constructor that takes xmin and xmax in addition to the number of bins
std::vector<double> get_bin_low_edges( double xmin, double xmax, int Nbins )
{
  std::vector<double> bin_low_edges;
  double bin_step = ( xmax - xmin ) / Nbins;
  for ( int b = 0; b <= Nbins; ++b ) {
    double low_edge = xmin + b*bin_step;
    bin_low_edges.push_back( low_edge );
  }

  return bin_low_edges;
}


// Utility functions for manipulating NtupleFileType values

bool ntuple_type_is_detVar( const NtupleFileType& type ) {
  constexpr std::array< NtupleFileType, 12 > detVar_types = {
    NtupleFileType::kDetVarMCCV, NtupleFileType::kDetVarMCLYatten,
    NtupleFileType::kDetVarMCLYdown, NtupleFileType::kDetVarMCLYrayl,
    NtupleFileType::kDetVarMCRecomb2, NtupleFileType::kDetVarMCSCE,
    NtupleFileType::kDetVarMCWMAngleXZ, NtupleFileType::kDetVarMCWMAngleYZ,
    NtupleFileType::kDetVarMCWMdEdx, NtupleFileType::kDetVarMCWMX,
    NtupleFileType::kDetVarMCWMYZ, NtupleFileType::kDetVarMCCVExtra
  };

  const auto begin = detVar_types.cbegin();
  const auto end = detVar_types.cend();
  const auto iter = std::find( begin, end, type );
  if ( iter != end ) return true;
  return false;
}

bool ntuple_type_is_nuedetVar( const NtupleFileType& type){
  constexpr std::array< NtupleFileType, 12 > nuedetVar_types = {
    NtupleFileType::kNueDetVarMCCV, NtupleFileType::kNueDetVarMCLYatten,
    NtupleFileType::kNueDetVarMCLYdown, NtupleFileType::kNueDetVarMCLYrayl,
    NtupleFileType::kNueDetVarMCRecomb2, NtupleFileType::kNueDetVarMCSCE,
    NtupleFileType::kNueDetVarMCWMAngleXZ, NtupleFileType::kNueDetVarMCWMAngleYZ,
    NtupleFileType::kNueDetVarMCWMdEdx, NtupleFileType::kNueDetVarMCWMX,
    NtupleFileType::kNueDetVarMCWMYZ, NtupleFileType::kNueDetVarMCCVExtra
  };
  const auto begin = nuedetVar_types.cbegin();
  const auto end = nuedetVar_types.cend();
  const auto iter = std::find( begin, end, type );
  if ( iter != end ) return true;
  return false;
}

bool ntuple_type_is_altCV( const NtupleFileType& type ) {
  if ( type == NtupleFileType::kAltCVMC ) return true;
  return false;
}

bool ntuple_type_is_mc( const NtupleFileType& type ) {
  if ( type != NtupleFileType::kOnBNB && type != NtupleFileType::kExtBNB && type != NtupleFileType::kOpenBNB ) {
    return true;
  }
  return false;
}

bool ntuple_type_is_reweightable_mc( const NtupleFileType& type ) {

  if ( type == NtupleFileType::kNumuMC
    || type == NtupleFileType::kIntrinsicNueMC
    || type == NtupleFileType::kDirtMC )
  {
    return true;
  }

  return false;
}




