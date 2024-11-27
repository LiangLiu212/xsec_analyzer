// XSecAnalyzer includes
#include "XSecAnalyzer/Binning/Muon_Pmu_Study_CC0Pi.hh"

Muon_Pmu_Study_CC0Pi::Muon_Pmu_Study_CC0Pi() : BinSchemeBase( "Muon_Pmu_Study_CC0Pi" ) {}

void Muon_Pmu_Study_CC0Pi::DefineBlocks() {

  /////// Set some standard variables before managing the blocks

  // TTree name for the post-processed ntuples
  ntuple_ttree_name_ = "stv_tree";

  // Run numbers to use when plotting migration matrices
  runs_to_use_ = { 1,2,3 };

  // Prefix for the output bin and slice configuration text files
  out_config_prefix_ = "Muon_1D_Pmu_v3_";

  // Selection to use with this binning scheme
  selection_name_ = "JOINTCC0pi";

  // TDirectory file name to use when producing the univmake output histograms
  out_tdir_name_ = "JOINTCC0pi";

  /////// Define the blocks of bins in both true and reco space
  std::string branchexpr_reco_scheme1,
  branchexpr_true_scheme2, 
  branchexpr_true_scheme1,
  branchexpr_reco_scheme2, 
  branchexpr_sideband, 
  title, textitle,
  selection_true, selection_reco,branchexpr;

  selection_true = "mc_is_cc0pi_signal";
  selection_reco = "sel_CC0pi && sel_muon_contained";

  // First block: cos_theta_mu in 2D
  std::vector<double> Pmu_2d = { 0.1, 0.24, 0.3, 0.38, 0.48, 0.7, 0.85, 1.28, 1.58, 2.0};
  std::vector<double> trk_len_v_edges = {0, 10, 20, 30, 40, 50, 60, 80, 100 ,120, 140, 160, 180, 200, 250, 300, 350};
  std::vector<double> costheta = {-1.00, -0.50, 0.00, 0.27, 0.45, 0.62, 0.76, 0.86, 0.94, 1.0};

   branchexpr = "mc_p3_mu.Mag();GeV/c";
    title = "P_{#mu};GeV/c";
  // the tex title "tex title; units" is used in latex format
  textitle = "P_{\\mu};GeV/c";
  Block1D *b1dt_Pmu_true = new Block1D(branchexpr, title, textitle, Pmu_2d, selection_true, kSignalTrueBin);
  branchexpr = "p3_mu.Mag();GeV/c";
  // only the name of branch and the selection is different from true.
  Block1D *b1dt_Pmu_reco = new Block1D(branchexpr, title, textitle, Pmu_2d, selection_reco, kOrdinaryRecoBin);
  vect_block.emplace_back(b1dt_Pmu_true,b1dt_Pmu_reco);
  
  branchexpr = "mc_p3_mu.CosTheta();";
  title = "cos#theta;";
  // the tex title "tex title; units" is used in latex format
  textitle = "\\cos\\theta;";
  Block1D *b1dt_cos_true = new Block1D(branchexpr, title, textitle, costheta, selection_true, kSignalTrueBin);
 branchexpr = "p3_mu.CosTheta();";
  // only the name of branch and the selection is different from true.
  Block1D *b1dt_cos_reco = new Block1D(branchexpr, title, textitle, costheta, selection_reco, kOrdinaryRecoBin);
  vect_block.emplace_back(b1dt_cos_true,b1dt_cos_reco);
  
 
   // Block2D *b2d_TrackLength_true = new Block2D(branchexpr_reco_scheme1,
   //   "muon mom; GeV/c; TrkLenght_{#mu};cm ", 
   //   "P_{\\mu}; GeV/c; TrkLengh_{\\mu};cm ",
   //   MUON_2D_BIN_EDGES_trackLength, selection_true, kSignalTrueBin);

  //vect_block.emplace_back(b2d_TrackLength_true, b2d_TrackLength_reco);



  
   // Block2D *b2d_TrackLength_true_inclusive = new Block2D(branchexpr_reco_scheme1,
   //   "costheta; GeV/c; TrkLenght_{#mu};cm ", 
   //   "cos#theta;; TrkLengh_{\\mu};cm ",
   //   MUON_2D_BIN_EDGES_inclusive_trklenght, selection_true, kSignalTrueBin);
      
  //vect_block.emplace_back(b2d_TrackLength_true_inclusive, b2d_TrackLength_reco_inclusive);


  // the title "title; unit" is used in plot in root style
  title ="cos#theta; p_{#mu}; GeV/c;";
  // the tex title "tex title; units" is used in latex format
  textitle = "P_{\\mu}; GeV/c; \\cos\\theta; ";
  // selection

//
//  vect_block.emplace_back(b2dmuon_mom_costheta_true_scheme2, b2dmuon_mom_costheta_reco_scheme2);
 branchexpr_sideband = "trk_len_v[ muon_candidate_idx ];cm";
   // the title "title; unit" is used in plot in root style
  title ="TrkLength_{#mu};cm";
  // the tex title "tex title; units" is used in latex format
  textitle = "TrkLength_{\\mu};cm";


  Block1D *b1r_sideband_trklengh = new Block1D(branchexpr_sideband, title, textitle, trk_len_v_edges, selection_reco, kOrdinaryRecoBin);
  //vect_sideband.emplace_back(b1r_sideband_trklengh);
  vect_block.emplace_back(b1r_sideband_trklengh,b1r_sideband_trklengh);

/*
  std::map< double, std::vector<double> > MUON_2D_BIN_EDGES_inclusive_trklenght = {
// the 2D binning of inclusive but took the max limit of 2 GeV and not 2.5 GeV which its given  
{ -1.00, 
    {0, 10, 20, 30, 40, 50, 60, 70, 80, 100, 350} },
{ -0.50,
    {0, 10, 20, 30, 40, 50, 60, 70, 80, 100, 350}},
{ 0.00, 
    { 0, 10, 20, 30, 40, 50, 60, 70, 80, 100, 350 }},
 { 0.27, 
     trk_len_v_edges },
 { 0.45, 
     trk_len_v_edges },
 { 0.62, 
     trk_len_v_edges },
 { 0.76, 
     trk_len_v_edges },
 { 0.86, 
     trk_len_v_edges },
 { 0.94, 
     trk_len_v_edges },
 { 1.0, {} }
};

 branchexpr_reco_scheme1 = "p3_mu.CosTheta();GeV/c;trk_len_v[ muon_candidate_idx ];cm";


    Block2D *b2d_TrackLength_reco_inclusive= new Block2D(branchexpr_reco_scheme1,
      "costheta; GeV/c; TrkLenght_{#mu};cm ", 
      "cos#theta;; TrkLengh_{\\mu};cm ",
      MUON_2D_BIN_EDGES_inclusive_trklenght, selection_reco, kOrdinaryRecoBin);

vect_sideband.emplace_back(b2d_TrackLength_reco_inclusive);
 //vect_block.emplace_back(b2d_TrackLength_reco_inclusive,b2d_TrackLength_reco_inclusive);
*/
/*
    branchexpr_reco_scheme1 = "p3_mu.Mag();GeV/c;trk_len_v[ muon_candidate_idx ];cm";
    std::map< double, std::vector<double> > MUON_2D_BIN_EDGES_trackLength = {
    { 0.1, {0, 10, 20, 30, 40, 50, 60, 70, 80, 100, 350} },
    { 0.24, {0, 10, 20, 30, 40, 50, 60, 70, 80, 100, 350} },
    { 0.3,  {0, 10, 20, 30, 40, 50, 60, 70, 80, 100, 350} },
    { 0.38, trk_len_v_edges },
    { 0.48, trk_len_v_edges },
    { 0.7,  trk_len_v_edges },
    { 0.85, trk_len_v_edges },
    { 1.28, {0,50, 80, 100, 120, 140, 160, 180, 200, 250, 300, 350}},
    { 1.58, {0,50, 80, 100, 120, 140, 160, 180, 200, 250, 300, 350} },
    { 2.0, {} }
    };
  
    Block2D *b2d_TrackLength_reco = new Block2D(branchexpr_reco_scheme1,
      "muon mom; GeV/c; TrkLenght_{#mu};cm", 
      "P_{\\mu}; GeV/c; TrkLengh_{\\mu};cm",
      MUON_2D_BIN_EDGES_trackLength, selection_reco, kSidebandRecoBin);
    //vect_sideband.emplace_back(b2d_TrackLength_reco);

  vect_block.emplace_back(b2d_TrackLength_reco,b2d_TrackLength_reco);
  */
 
  
  /*
  std::map< double, std::vector<double> > multiply_Pmu  = {
  {-0.5, Pmu_2d}, 
  {0.5, Pmu_2d},
  {1.5, Pmu_2d},
  {2.5, Pmu_2d},
  {3.5, {} }
  }; 
  
  
     branchexpr_reco_scheme1 = "sel_num_proton_candidates;;p3_mu.Mag();GeV/c";
      Block2D *b2d_Mulitply_pmu_reco = new Block2D(branchexpr_reco_scheme1,
      "Nprotons;;P_{#mu};GeV/c", 
      "Nprotons;;P_{#mu};GeV/c",
      multiply_Pmu, selection_reco, kOrdinaryRecoBin);

     vect_block.emplace_back(b2d_Mulitply_pmu_reco,b2d_Mulitply_pmu_reco);

    //vect_sideband.emplace_back(b2d_Mulitply_pmu_reco);

    std::map< double, std::vector<double> > multiply_trklen = {
  {-0.5, trk_len_v_edges}, 
  {0.5, trk_len_v_edges},
  {1.5, trk_len_v_edges},
  {2.5, trk_len_v_edges},
  {3.5, {} }
  }; 
  
   branchexpr_reco_scheme1 = "sel_num_proton_candidates;;trk_len_v[ muon_candidate_idx ];cm";

        Block2D *b2d_Mulitply_trklen_reco = new Block2D(branchexpr_reco_scheme1,
      "Nprotons;;trkLen;cm", 
      "Nprotons;;trkLen;cm",
      multiply_trklen, selection_reco, kOrdinaryRecoBin);
 
 //vect_sideband.emplace_back(b2d_Mulitply_trklen_reco);
 
   vect_block.emplace_back(b2d_Mulitply_trklen_reco,b2d_Mulitply_trklen_reco);
*/

/*
    branchexpr_reco_scheme1 = "trk_len_v[ muon_candidate_idx ];cm;p3_mu.Mag();";
       std::map< double, std::vector<double> >trackLength_BIN_EDGES_Pmu = {
  {0,Pmu_2d},
  {10,Pmu_2d},
  {25,Pmu_2d},
  {40,Pmu_2d},
  {60,Pmu_2d},
  {80,Pmu_2d},
  {100,Pmu_2d},
  {120, Pmu_2d},
  {140,Pmu_2d},
  {160,Pmu_2d},
  {180,Pmu_2d},
  {200,Pmu_2d},
  {250,Pmu_2d},
  {300,Pmu_2d},
  {300, {} }
  };
  
  
     Block2D *b2d_trackLength_BIN_EDGES_Pmu = new Block2D(branchexpr_reco_scheme1,
      "trkLen;cm;P_{#mu};GeV/c", 
      "trkLen;cm;P_{#mu};GeV/c",
      trackLength_BIN_EDGES_Pmu, selection_reco, kOrdinaryRecoBin);

vect_block.emplace_back(b2d_trackLength_BIN_EDGES_Pmu, b2d_trackLength_BIN_EDGES_Pmu);

*/

  // CATEGORY is the branchexpr
  // background_index is vector of background categories.
  CATEGORY = "category";
  background_index = {0, 5, 7, 8, 9, 10, 11};



 //vect_sideband.emplace_back(b2d_TrackLength_reco);


/*

 will leave out for now
 std::vector< double > Mult= {-0.5, 0.5, 1.5, 2.5, 3.5};

  // the branch name of the truth of proton momentum in stv root files; unit is optional with format "branch name; unit"
  branchexpr = "CC1muXp0pi_true_lead_proton_delta_pT ; GeV/c";
  // the title "title; unit" is used in plot in root style
  title = "leading proton #Delta p_{T}; GeV/c";
  // the tex title "tex title; units" is used in latex format
  textitle = "leading proton \\Delta \\p_{T}; GeV/c";
  // selection
  selection = "CC1muXp0pi_MC_Signal && CC1muXp0pi_nProtons_in_Momentum_range > 0";

  Block1D *b1dt_delta_pT = new Block1D(branchexpr, title, textitle, delta_pT_1D_edges, selection, kSignalTrueBin);

*/
/*





  

  
   
    
    vect_sideband.emplace_back(b2d_Mulitply_TrackLength_reco);
    
  
      std::map< double, std::vector<double> > multiply_costheta  = {
  {-0.5, costheta}, 
  {0.5, costheta},
  {1.5, costheta},
  {2.5, costheta},
  {3.5, {} }
  }; 
   branchexpr_reco_scheme1 = "sel_num_proton_candidates;p3_mu.CosTheta();";
      Block2D *b2d_Mulitply_cos_reco = new Block2D(branchexpr_reco_scheme1,
      "Nprotons;;cos#theta;", 
      "Nprotons;;cos#theta;",
      multiply_costheta, selection_reco, kSidebandRecoBin);
  vect_sideband.emplace_back(b2d_Mulitply_cos_reco);
  
  
  
  
  

  
  
  vect_sideband.emplace_back(b2d_Mulitply_trklen_reco);  

  
  

  
  vect_sideband.emplace_back(b2d_Mulitply_pmu_reco);
  
  
  */
  

}
