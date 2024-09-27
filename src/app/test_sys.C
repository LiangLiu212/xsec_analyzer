/*
 * =====================================================================================
 *
 *       Filename:  test_sys.C
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/26/24 10:37:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Liang Liu (L. Liu), liangliu@fnal.gov
 *		    Fermi National Accelerator Laboratory
 *  Collaboration:  GENIE
 *
 * =====================================================================================
 */

// Executable for generating systematic universe files for later analysis. It
// has been adapted from a similar ROOT macro.

// Standard library includes
#include <stdexcept>

// ROOT includes
#include "TBranch.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"

// XSecAnalyzer includes
#include "XSecAnalyzer/FilePropertiesManager.hh"
#include "XSecAnalyzer/MCC9SystematicsCalculator.hh"
#include "XSecAnalyzer/UniverseMaker.hh"

int main( int argc, char* argv[] ) {

  if ( argc != 4 && argc != 5 ) {
    std::cout << "Usage: univmake LIST_FILE"
	      << " UNIVMAKE_CONFIG_FILE OUTPUT_ROOT_FILE"
	      << " [FILE_PROPERTIES_CONFIG_FILE]\n";
    return 1;
  }

  std::string list_file_name( argv[1] );
  std::string univmake_config_file_name( argv[2] );
  std::string output_file_name( argv[3] );

  std::cout << "\nRunning univmake.C with options:\n";
  std::cout << "\tlist_file_name: " << list_file_name << '\n';
  std::cout << "\tunivmake_config_file_name: "
    << univmake_config_file_name << '\n';
  std::cout << "\toutput_file_name: " << output_file_name << '\n';

  // Simultaneously check that we can write to the output file directory, and wipe any information within that file
  TFile* temp_file = new TFile(output_file_name.c_str(), "recreate");
  if (!temp_file || temp_file->IsZombie()) {
    std::cerr << "Could not write to output file: "
      << output_file_name << '\n';
    throw;
  }
  delete temp_file;

  // If the user specified an (optional) non-default configuration file for the
  // FilePropertiesManager on the command line, then load it here. Note that the
  // only place where the FilePropertiesManager configuration is relevant is in
  // the use of MCC9SystematicsCalculator to compute total event count
  // histograms (see below).
  auto& fpm = FilePropertiesManager::Instance();
  //if ( argc == 5 ) {
    std::cout << "\tfile_properties_name: " << argv[1] << '\n';
    fpm.load_file_properties( argv[1] );
  //}

  // Regardless of whether the default was used or not, retrieve the
  // name of the FilePropertiesManager configuration file that was
  // actually used
  std::string fp_config_file_name = fpm.config_file_name();
  std::cout << "\nLoaded FilePropertiesManager configuration from: "
	    << fp_config_file_name << '\n';

  std::cout << "\nCalculating total event counts using all input files:\n";

  // Use a temporary MCC9SystematicsCalculator object to automatically calculate
  // the total event counts in each universe across all input files. Since the
  // get_covariances() member function is never called, the specific systematics
  // configuration file used doesn't matter. The empty string passed as the
  // second argument to the constructor just instructs the
  // MCC9SystematicsCalculator class to use the default systematics
  // configuration file.

  MCC9SystematicsCalculator unfolder( output_file_name, univmake_config_file_name, "" );

  return 0;
}
