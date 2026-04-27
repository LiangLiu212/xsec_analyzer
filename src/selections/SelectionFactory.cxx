// Standard library
#include <stdexcept>
#include <string>

// XSecAnalyzer includes
#include "XSecAnalyzer/Selections/CC1mu1p0pi.hh"
#include "XSecAnalyzer/Selections/CC1mu2p0pi.hh"
#include "XSecAnalyzer/Selections/CC1muNp0pi.hh"
#include "XSecAnalyzer/Selections/NuMICC1e.hh"
#include "XSecAnalyzer/Selections/DummySelection.hh"
#include "XSecAnalyzer/Selections/NC1p.hh"
#include "XSecAnalyzer/Selections/SelectionFactory.hh"

SelectionFactory::SelectionFactory() {
}

SelectionBase* SelectionFactory::CreateSelection(
  const std::string& selection_name )
{
  SelectionBase* sel;
  if ( selection_name == "CC1mu1p0pi" ) {
    sel = new CC1mu1p0pi;
  }
  else if ( selection_name == "CC1mu2p0pi" ) {
    sel = new CC1mu2p0pi;
  }
  else if ( selection_name == "CC1muNp0pi" ) {
    sel = new CC1muNp0pi;
  }
  else if ( selection_name == "NuMICC1e" ) {
    sel = new NuMICC1e;
  }
  else if ( selection_name == "Dummy" ) {
    sel = new DummySelection;
  }
  else if ( selection_name.substr( 0, 4 ) == "NC1p" ) {
    // Optional run period encoded as "NC1p_<N>" (e.g. "NC1p_4").
    // Plain "NC1p" defaults to run period 1.
    int run_period = 1;
    if ( selection_name.size() > 5 && selection_name[4] == '_' ) {
      try {
        run_period = std::stoi( selection_name.substr(5) );
      }
      catch ( const std::exception& ) {
        std::cerr << "WARNING: Could not parse run period from selection name \""
                  << selection_name << "\". Defaulting to run period 1.\n";
      }
    }
    sel = new NC1p( run_period );
  }
  else {
    std::cerr << "Selection name requested: " << selection_name
      << " is not implemented in " << __FILE__ << '\n';
    throw;
  }

  // Ensure that the owned map of category definitions is set up
  sel->define_category_map();

  return sel;
}
