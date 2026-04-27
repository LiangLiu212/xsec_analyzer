#pragma once

// XSecAnalyzer includes
#include "XSecAnalyzer/Binning/BinSchemeBase.hh"

// NC1p 1D differential cross-section bin scheme.
// Four observables are defined as independent 1D blocks, mirroring the
// DefinePRDCVHistos / DefineCVHistos binning from the legacy make_tree analysis:
//   Block 1 - proton KE      : 9 bins {0, 0.05, ..., 0.6} GeV
//   Block 2 - proton costheta: 9 bins {-1, 0, 0.3, ..., 1.1}
//   Block 3 - proton Q2      : 12 uniform bins [0, 1.2] GeV^2
//   Block 4 - proton momentum: 9 bins {0.2, 0.3, ..., 1.4} GeV/c
class NC1pBinScheme : public BinSchemeBase {

  public:

    NC1pBinScheme();
    virtual void DefineBlocks() override;
};
