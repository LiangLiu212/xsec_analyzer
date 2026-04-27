#pragma once

// TMVA
#include "TMVA/Reader.h"

// XSecAnalyzer
#include "XSecAnalyzer/Selections/SelectionBase.hh"

// NC1p neutral-current single-proton selection for the old SingleProtonAna
// ntuple format.  The selection mirrors the logic in the legacy make_tree.C:
//   1. evt_reco_1p == 1
//   2. Find the proton candidate track (is_reco_nc1p && isinFV, containment
//      and quality cuts identical to make_tree.C)
//   3. BDT score > 0.2 (BDTG from TMVA)
//
// BDT weights path (hardcoded base, run-period specific):
//   /exp/uboone/app/users/renlu23/v61_2022/analysis/BDT/dataset/
//     weights_<run_period>/TMVAClassification_BDTG.weights.xml
//
// Pass the run period via the selection name in SelectionFactory:
//   "NC1p_1"  → run period 1
//   "NC1p_4"  → run period 4
//   "NC1p"    → defaults to run period 1
//
// If the environment variable NC1P_BDT_DIR is set it overrides the path
// entirely (used for non-standard installations or local tests).
class NC1p : public SelectionBase {

public:

  explicit NC1p( int run_period = 1 );
  ~NC1p();

  virtual int  categorize_event( AnalysisEvent* event )           override final;
  virtual void compute_reco_observables( AnalysisEvent* event )   override final;
  virtual void compute_true_observables( AnalysisEvent* event )   override final;
  virtual void define_category_map()                              override final;
  virtual void define_constants()                                 override final;
  virtual void define_output_branches()                           override final;
  virtual bool define_signal( AnalysisEvent* event )              override final;
  virtual void reset()                                            override final;
  virtual bool selection( AnalysisEvent* event )                  override final;

private:

  // --- Selection cut flags written to the output tree ---
  bool sel_1p_;               // evt_reco_1p == 1
  bool sel_proton_found_;     // valid proton candidate found after quality cuts
  bool sel_bdt_passed_;       // BDT score > BDT_CUT

  // --- Signal definition flags ---
  bool sig_nc_interaction_;
  bool sig_one_proton_above_thresh_;
  bool sig_vertex_in_fv_;

  // --- Reconstructed observables ---
  double Reco_KE_;
  double Reco_CosTheta_;
  double Reco_Momentum_;
  double Reco_Q2_;

  // --- True observables ---
  double True_KE_;
  double True_CosTheta_;
  double True_Momentum_;
  double True_Q2_;

  // --- BDT score ---
  float BDT_Score_;

  // --- Selected proton track index ---
  int proton_idx_;

  // --- TMVA ---
  TMVA::Reader* bdt_reader_;
  // BDT input variables (floats; pointers registered with TMVA::Reader)
  float bdt_len_;
  float bdt_theta_;
  float bdt_phi_;
  float bdt_end_dedx2_;    // = start_dedx_2 of the candidate (BDT naming legacy)
  float bdt_total_dedx2_;
  float bdt_starty_;
  float bdt_startz_;
  float bdt_endy_;
  float bdt_endz_;
  float bdt_chi2p0_;
  float bdt_chi2p1_;
  float bdt_chi2p2_;
  float bdt_trkdis_;

  static constexpr float BDT_CUT = 0.2f;

  // Run period (1–5), selects the matching BDT weights subdirectory
  int run_period_;
};
