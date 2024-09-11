/*
 * =====================================================================================
 *
 *       Filename:  BinScheme.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/16/24 17:20:04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Liang Liu (L. Liu), liangliu@fnal.gov
 *		    Fermi National Accelerator Laboratory
 *  Collaboration:  GENIE
 *
 * =====================================================================================
 */

#include "MakeConfig.h"

void MakeConfig::BinScheme(){

  auto& fpm = FilePropertiesManager::Instance();
  fpm.load_file_properties("/exp/uboone/app/users/liangliu/analysis-code/xsec_analyzer_liang/Configs/file_properties_cthorpe_v00_00_04.txt");
  DIRECTORY = "muon_2d_bin"; 
  TREE  = "stv_tree";
  RUNS = {1,2,3};
  vect_block.clear();
  TString branchexpr, title, textitle, selection;

  // configure 1D block proton momentum
  //
  // using a double vector to define uneven bin size

  // cos_theta_mu in 1D
  std::vector< double > cos_theta_mu_1D_edges = { -1., -0.85, -0.775, -0.7,
    -0.625, -0.55, -0.475, -0.4, -0.325, -0.25, -0.175, -0.1, -0.025, 0.05,
    0.125, 0.2, 0.275, 0.35, 0.425, 0.5, 0.575, 0.65, 0.725, 0.8, 0.85,
    0.875, 0.9, 0.925, 0.950, 0.975, 1. };

  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
  branchexpr = "CC1muXp0pi_true_muon_costh ; GeV_c";
  // the title "title; unit" is used in plot in root style
  title = "muon cos#theta; GeV/c";
  // the tex title "tex title; units" is used in latex format
  textitle = "\\cos\\theta_{\\mu}; GeV/c";
  // selection
  selection = "CC1muXp0pi_MC_Signal";

  Block1D *b1dt = new Block1D(branchexpr, title, textitle, cos_theta_mu_1D_edges, selection, kSignalTrueBin);



  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
  branchexpr = "CC1muXp0pi_reco_muon_costh ; GeV_c";
  // the title "title; unit" is used in plot in root style
  // 
  // the tex title "tex title; units" is used in latex format
  // selection
  selection = "CC1muXp0pi_Selected";

  // only the name of branch and the selection is different from true.
  Block1D *b1dr = new Block1D(branchexpr, title, textitle, cos_theta_mu_1D_edges, selection, kOrdinaryRecoBin);

  vect_block.emplace_back(b1dt, b1dr);

  // muon momentum
  //

  // p_mu in 1D
  std::vector< double > pmu_1D_edges = { 0.1, 0.175, 0.2, 0.225, 0.25,
    0.275, 0.3, 0.325, 0.35, 0.375, 0.4, 0.425, 0.45, 0.475, 0.5, 0.55, 0.6,
    0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1.0, 1.1, 1.2 };


  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
  branchexpr = "CC1muXp0pi_true_muon_p ; GeV_c";
  // the title "title; unit" is used in plot in root style
  title = "muon mom; GeV/c";
  // the tex title "tex title; units" is used in latex format
  textitle = "\\mu_{P}; GeV/c";
  // selection
  selection = "CC1muXp0pi_MC_Signal";

  Block1D *b1dt_pmu = new Block1D(branchexpr, title, textitle, pmu_1D_edges, selection, kSignalTrueBin);

  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
  branchexpr = "CC1muXp0pi_reco_muon_p ; GeV_c";
  // the title "title; unit" is used in plot in root style
  // the tex title "tex title; units" is used in latex format
  // selection
  selection = "CC1muXp0pi_Selected";

  // only the name of branch and the selection is different from true.
  Block1D *b1dr_pmu = new Block1D(branchexpr, title, textitle, pmu_1D_edges, selection, kOrdinaryRecoBin);

  vect_block.emplace_back(b1dt_pmu, b1dr_pmu);





  // Bin definitions for the 2D cross-section measurements

  // Using floating-point numbers as std::map keys is admittedly evil, but
  // it's safe in this case: all we'll do with this map is iterate over the
  // elements. Keys are muon momentum bin edges, values are muon scattering
  // cosine bin edges.
//  std::map< double, std::vector<double> > MUON_2D_BIN_EDGES = {
//
//    // No need for an underflow bin: due to the signal definition, all muons
//    // with reco momentum below 0.1 GeV/c will be lost
//    { 0.1, { -1, -0.55, -0.25, 0., 0.25, 0.45, 0.7, 1.00 }, },
//    { 0.24, { -1, -0.55, -0.25, 0., 0.25, 0.45, 0.7, 1.00 } },
//    { 0.3,  { -1, -0.4, -0.1, 0.1, 0.35, 0.5, 0.7, 0.85, 1. } },
//    { 0.38, { -1, 0, 0.5, 0.65, 0.8, 0.92, 1.00 } },
//    { 0.48, { -1, 0.2, 0.5, 0.65, 0.8, 0.875, 0.950, 1.00 } },
//    { 0.7, { -1, 0.65, 0.8, 0.875, 0.950, 1.00 } },
//    { 0.85, { -1, 0.85, 0.9, 0.950, 1.00 } },
//
//    // Upper edge of the last bin. Due to the signal definition, no overflow
//    // bin is needed for muons above 1.2 GeV/c
//    { 1.2, {} }
//
//  };
//
//  Block2D *b2dmuon_mom_costheta_true = new Block2D("CC1muXp0pi_true_muon_p; GeV/c; CC1muXp0pi_true_muon_costh; ", 
//      "muon mom; GeV/c; cos#theta; ",
//      "P_{\\mu}; GeV/c; \\cos\\theta; ",
//      MUON_2D_BIN_EDGES, "CC1muXp0pi_MC_Signal", kSignalTrueBin);
//  Block2D *b2dmuon_mom_costheta_reco = new Block2D("CC1muXp0pi_reco_muon_p; GeV/c; CC1muXp0pi_reco_muon_costh; ",
//      "muon mom; GeV/c; cos#theta; ", 
//      "P_{\\mu}; GeV/c; \\cos\\theta; ",
//      MUON_2D_BIN_EDGES, "CC1muXp0pi_Selected", kOrdinaryRecoBin);
//  vect_block.emplace_back(b2dmuon_mom_costheta_true, b2dmuon_mom_costheta_reco);
 

//  // cos_theta_mu in 1D
//  std::vector< double > cos_theta_mu_1D_edges_0p = { -1., -0.85, -0.775, -0.7,
//    -0.625, -0.55, -0.475, -0.4, -0.325, -0.25, -0.175, -0.1, -0.025, 0.05,
//    0.125, 0.2, 0.275, 0.35, 0.425, 0.5, 0.575, 0.65, 0.725, 0.8, 0.85,
//    0.875, 0.9, 0.925, 0.950, 0.975, 1. };
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_muon_costh ; GeV_c";
//  // the title "title; unit" is used in plot in root style
//  title = "0p muon cos#theta; GeV/c";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "0p \\cos\\theta_{\\mu}; GeV/c";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range == 0";
//
//  Block1D *b1dt_0p = new Block1D(branchexpr, title, textitle, cos_theta_mu_1D_edges_0p, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_muon_costh ; GeV_c";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates == 0";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_0p = new Block1D(branchexpr, title, textitle, cos_theta_mu_1D_edges_0p, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_0p, b1dr_0p);
//
//
//
//  // cos_theta_mu in 1D
//  std::vector< double > cos_theta_mu_1D_edges_Np = { -1., -0.85, -0.775, -0.7,
//    -0.625, -0.55, -0.475, -0.4, -0.325, -0.25, -0.175, -0.1, -0.025, 0.05,
//    0.125, 0.2, 0.275, 0.35, 0.425, 0.5, 0.575, 0.65, 0.725, 0.8, 0.85,
//    0.875, 0.9, 0.925, 0.950, 0.975, 1. };
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_muon_costh ; GeV_c";
//  // the title "title; unit" is used in plot in root style
//  title = "Np muon cos#theta; GeV/c";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "Np \\cos\\theta_{\\mu}; GeV/c";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 0";
//
//  Block1D *b1dt_Np = new Block1D(branchexpr, title, textitle, cos_theta_mu_1D_edges_Np, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_muon_costh ; GeV_c";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 0";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_Np = new Block1D(branchexpr, title, textitle, cos_theta_mu_1D_edges_Np, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_Np, b1dr_Np);


//  // cos_theta_mu in 1D
//  std::vector< double > delta_alpha3D_edges = { 0, 0.4,  0.7, 1.0,
//    1.3,
//    1.6,
//    1.9,
//    2.2, 2.5, 2.8, 3.1415927 };
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_lead_proton_delta_alpha3DMu ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "leading proton #Delta #alpha 3D #mu; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "leading proton \\Delta \\alpha 3D \\mu; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 0";
//
//  Block1D *b1dt_delta_alpha_3d = new Block1D(branchexpr, title, textitle, delta_alpha3D_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_lead_proton_delta_alpha3DMu ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 0";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_delta_alpha_3d = new Block1D(branchexpr, title, textitle, delta_alpha3D_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_delta_alpha_3d, b1dr_delta_alpha_3d);
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_hadron_delta_alpha3DMu ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "sum of protons #Delta #alpha 3D #mu; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "sum of protons \\Delta \\alpha 3D \\mu; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 1";
//
//  Block1D *b1dt_hadron_delta_alpha_3d = new Block1D(branchexpr, title, textitle, delta_alpha3D_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_hadron_delta_alpha3DMu ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 1";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_hadron_delta_alpha_3d = new Block1D(branchexpr, title, textitle, delta_alpha3D_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_hadron_delta_alpha_3d, b1dr_hadron_delta_alpha_3d);


//  // cos_theta_mu in 1D
//  std::vector< double > delta_alpha3Dq_edges = { 0, 0.4,  0.7, 1.0,
//    1.3,
//    1.6,
//    1.9,
//    2.2, 2.5, 2.8, 3.1415927 };
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_lead_proton_delta_alpha3Dq ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "leading proton #Delta #alpha 3D q; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "leading proton \\Delta \\alpha 3D q; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 0";
//
//  Block1D *b1dt_delta_alpha3Dq = new Block1D(branchexpr, title, textitle, delta_alpha3Dq_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_lead_proton_delta_alpha3Dq ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 0";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_delta_alpha3Dq = new Block1D(branchexpr, title, textitle, delta_alpha3Dq_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_delta_alpha3Dq, b1dr_delta_alpha3Dq);
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_hadron_delta_alpha3Dq ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "sum of protons #Delta #alpha 3D q; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "sum of protons \\Delta \\alpha 3D q; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 1";
//
//  Block1D *b1dt_hadron_delta_alpha3Dq = new Block1D(branchexpr, title, textitle, delta_alpha3Dq_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_hadron_delta_alpha3Dq ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 1";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_hadron_delta_alpha3Dq = new Block1D(branchexpr, title, textitle, delta_alpha3Dq_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_hadron_delta_alpha3Dq, b1dr_hadron_delta_alpha3Dq);





//  // cos_theta_mu in 1D
//  std::vector< double > delta_alphaT_edges = { 0, 0.4,  0.7, 1.0,
//    1.3,
//    1.6,
//    1.9,
//    2.2, 2.5, 2.8, 3.1415927 };
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_lead_proton_delta_alphaT ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "leading proton #Delta #alpha T; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "leading proton \\Delta \\alpha T; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 0";
//
//  Block1D *b1dt_delta_alphaT = new Block1D(branchexpr, title, textitle, delta_alphaT_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_lead_proton_delta_alphaT ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 0";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_delta_alphaT = new Block1D(branchexpr, title, textitle, delta_alphaT_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_delta_alphaT, b1dr_delta_alphaT);
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_hadron_delta_alphaT ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "sum of protons #Delta #alpha T; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "sum of protons \\Delta \\alpha T; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 1";
//
//  Block1D *b1dt_hadron_delta_alphaT = new Block1D(branchexpr, title, textitle, delta_alphaT_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_hadron_delta_alphaT ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 1";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_hadron_delta_alphaT = new Block1D(branchexpr, title, textitle, delta_alphaT_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_hadron_delta_alphaT, b1dr_hadron_delta_alphaT);
//

  // cos_theta_mu in 1D
//  std::vector< double > delta_pT_edges = { 0., 0.06, 0.12, 0.18, 0.24, 0.32,
//    0.4, 0.48, 0.55, 0.68, 0.75, 0.9 };
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_lead_proton_delta_pT ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "leading proton #Delta pT; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "leading proton \\Delta pT; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 0";
//
//  Block1D *b1dt_delta_pT = new Block1D(branchexpr, title, textitle, delta_pT_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_lead_proton_delta_pT ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 0";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_delta_pT = new Block1D(branchexpr, title, textitle, delta_pT_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_delta_pT, b1dr_delta_pT);
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_hadron_delta_pT ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "sum of protons #Delta pT; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "sum of protons \\Delta pT; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 1";
//
//  Block1D *b1dt_hadron_delta_pT = new Block1D(branchexpr, title, textitle, delta_pT_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_hadron_delta_pT ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 1";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_hadron_delta_pT = new Block1D(branchexpr, title, textitle, delta_pT_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_hadron_delta_pT, b1dr_hadron_delta_pT);
//

  // cos_theta_mu in 1D
//  std::vector< double > pn_edges = { 0., 0.07, 0.14, 0.21, 0.28, 0.35,
//    0.45, 0.54, 0.66, 0.77, 0.9 };
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_lead_proton_pn ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "leading proton #delta pn; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "leading proton \\Delta pn; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 0";
//
//  Block1D *b1dt_pn = new Block1D(branchexpr, title, textitle, pn_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_lead_proton_pn ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 0";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_pn = new Block1D(branchexpr, title, textitle, pn_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_pn, b1dr_pn);
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_hadron_pn ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "sum of protons #delta pn; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "sum of protons \\Delta pn; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 1";
//
//  Block1D *b1dt_hadron_pn = new Block1D(branchexpr, title, textitle, pn_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_hadron_pn ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 1";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_hadron_pn = new Block1D(branchexpr, title, textitle, pn_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_hadron_pn, b1dr_hadron_pn);



//  // cos_theta_mu in 1D
//  std::vector< double > delta_pL_edges = { -1, -0.6, -0.45, -0.35, -0.25,
//     -0.15, -0.075, 0, 0.075, 0.15, 0.25, 0.35, 1 };
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_lead_proton_delta_pL ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "leading proton #Delta pL; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "\\Delta pL; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 0";
//
//  Block1D *b1dt_delta_pL = new Block1D(branchexpr, title, textitle, delta_pL_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_lead_proton_delta_pL ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 0";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_delta_pL = new Block1D(branchexpr, title, textitle, delta_pL_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_delta_pL, b1dr_delta_pL);
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_hadron_delta_pL ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "sum of protons #Delta pL; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "\\Delta pL; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 1";
//
//  Block1D *b1dt_hadron_delta_pL = new Block1D(branchexpr, title, textitle, delta_pL_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_hadron_delta_pL ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 1";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_hadron_delta_pL = new Block1D(branchexpr, title, textitle, delta_pL_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_hadron_delta_pL, b1dr_hadron_delta_pL);


//  // cos_theta_mu in 1D
//  std::vector< double > delta_phi3D_edges = { 0, 0.2, 0.4,  0.7, 1.0,
//    1.3,
//    1.6,
//    1.9,
//    2.2, 2.5, 3.1415927 };
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_lead_proton_delta_phi3D ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "leading proton #Delta #phi 3D; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "leading proton \\Delta \\phi 3D; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 0";
//
//  Block1D *b1dt_delta_phi3D = new Block1D(branchexpr, title, textitle, delta_phi3D_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_lead_proton_delta_phi3D ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 0";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_delta_phi3D = new Block1D(branchexpr, title, textitle, delta_phi3D_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_delta_phi3D, b1dr_delta_phi3D);
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_hadron_delta_phi3D ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "sum of protons #Delta #phi 3D; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "sum of protons \\Delta \\phi 3D; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 1";
//
//  Block1D *b1dt_hadron_delta_phi3D = new Block1D(branchexpr, title, textitle, delta_phi3D_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_hadron_delta_phi3D ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 1";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_hadron_delta_phi3D = new Block1D(branchexpr, title, textitle, delta_phi3D_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_hadron_delta_phi3D, b1dr_hadron_delta_phi3D);

  // cos_theta_mu in 1D
//  std::vector< double > delta_phiT_edges = { 0, 0.2, 0.4,  0.7, 1.0,
//    1.3,
//    1.6,
//    1.9,
//    2.2, 2.5, 3.1415927 };
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_lead_proton_delta_phiT ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "leading proton #Delta #phi T; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "leading proton \\Delta \\phi T; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 0";
//
//  Block1D *b1dt_delta_phiT = new Block1D(branchexpr, title, textitle, delta_phiT_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_lead_proton_delta_phiT ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 0";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_delta_phiT = new Block1D(branchexpr, title, textitle, delta_phiT_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_delta_phiT, b1dr_delta_phiT);
//
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_true_hadron_delta_phiT ; ";
//  // the title "title; unit" is used in plot in root style
//  title = "sum of protons #Delta #phi T; ";
//  // the tex title "tex title; units" is used in latex format
//  textitle = "sum of protons \\Delta \\phi T; ";
//  // selection
//  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 1";
//
//  Block1D *b1dt_hadron_delta_phiT = new Block1D(branchexpr, title, textitle, delta_phiT_edges, selection, kSignalTrueBin);
//
//  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
//  branchexpr = "CC1muXp0pi_reco_hadron_delta_phiT ; ";
//  // the title "title; unit" is used in plot in root style
//  // 
//  // the tex title "tex title; units" is used in latex format
//  // selection
//  selection = "CC1muXp0pi_Selected && CC1muXp0pi_num_proton_candidates > 1";
//
//  // only the name of branch and the selection is different from true.
//  Block1D *b1dr_hadron_delta_phiT = new Block1D(branchexpr, title, textitle, delta_phiT_edges, selection, kOrdinaryRecoBin);
//
//  vect_block.emplace_back(b1dt_hadron_delta_phiT, b1dr_hadron_delta_phiT);



  // event category
  CATEGORY = "CC1muXp0pi_EventCategory";
  background_index = {17, 18, 18, 20, 21, 22};

}


