// XSecAnalyzer includes
#include "XSecAnalyzer/Binning/NC1pBinScheme.hh"
#include "XSecAnalyzer/Binning/Block.hh"

NC1pBinScheme::NC1pBinScheme() : BinSchemeBase( "NC1pBinScheme" ) {}

void NC1pBinScheme::DefineBlocks() {

  ntuple_ttree_name_ = "stv_tree";

  // Run periods for migration matrix plots (BNB runs 1-3)
  runs_to_use_ = { 1, 2, 3 };

  out_config_prefix_ = "nc1p_";
  selection_name_    = "NC1p";
  out_tdir_name_     = "nc1p_xsec";

  // ---- Block 1: proton kinetic energy ----
  // DefinePRDCVHistos bins: xbin_ke = {0,0.05,0.1,0.15,0.2,0.25,0.3,0.4,0.5,0.6}
  std::vector< double > ke_edges = { 0., 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.4, 0.5, 0.6 };

  Block1D* ke_true = new Block1D( "NC1p_True_KE",
    "Proton Kinetic Energy (GeV)", "T_{p}\\,(\\mathrm{GeV})",
    ke_edges, "NC1p_MC_Signal", kSignalTrueBin );

  Block1D* ke_reco = new Block1D( "NC1p_Reco_KE",
    "Proton Kinetic Energy (GeV)", "T_{p}\\,(\\mathrm{GeV})",
    ke_edges, "NC1p_Selected", kOrdinaryRecoBin );

  vect_block.emplace_back( ke_true, ke_reco );

  // ---- Block 2: proton cos(theta) ----
  // DefinePRDCVHistos bins: xbin_costh = {-1,0,0.3,0.5,0.6,0.7,0.8,0.9,1,1.1}
  // (upper edge 1.1 absorbs events at costheta == 1 identically to old ROOT histos)
  std::vector< double > costh_edges = { -1., 0., 0.3, 0.5, 0.6, 0.7, 0.8, 0.9, 1., 1.1 };

  Block1D* costh_true = new Block1D( "NC1p_True_CosTheta",
    "Proton cos#theta", "\\cos\\theta_{p}",
    costh_edges, "NC1p_MC_Signal", kSignalTrueBin );

  Block1D* costh_reco = new Block1D( "NC1p_Reco_CosTheta",
    "Proton cos#theta", "\\cos\\theta_{p}",
    costh_edges, "NC1p_Selected", kOrdinaryRecoBin );

  vect_block.emplace_back( costh_true, costh_reco );

  // ---- Block 3: proton Q2 ----
  // DefineCVHistos: 11 uniform bins [0, 1.1] GeV^2  (binnum=11, binmin=0, binmax=1.1)
  std::vector< double > q2_edges = {
    0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1
  };

  Block1D* q2_true = new Block1D( "NC1p_True_Q2",
    "Q^{2} (GeV^{2})", "Q^{2}\\,(\\mathrm{GeV}^{2})",
    q2_edges, "NC1p_MC_Signal", kSignalTrueBin );

  Block1D* q2_reco = new Block1D( "NC1p_Reco_Q2",
    "Q^{2} (GeV^{2})", "Q^{2}\\,(\\mathrm{GeV}^{2})",
    q2_edges, "NC1p_Selected", kOrdinaryRecoBin );

  vect_block.emplace_back( q2_true, q2_reco );

  // ---- Block 4: proton momentum ----
  // DefineCVHistos xbin_p: {0.2,0.3,0.4,0.5,0.6,0.7,0.8,1,1.2,1.4} GeV/c
  std::vector< double > mom_edges = {
    0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 1.0, 1.2, 1.4
  };

  Block1D* mom_true = new Block1D( "NC1p_True_Momentum",
    "Proton Momentum (GeV/c)", "p_{p}\\,(\\mathrm{GeV}/c)",
    mom_edges, "NC1p_MC_Signal", kSignalTrueBin );

  Block1D* mom_reco = new Block1D( "NC1p_Reco_Momentum",
    "Proton Momentum (GeV/c)", "p_{p}\\,(\\mathrm{GeV}/c)",
    mom_edges, "NC1p_Selected", kOrdinaryRecoBin );

  vect_block.emplace_back( mom_true, mom_reco );
}
