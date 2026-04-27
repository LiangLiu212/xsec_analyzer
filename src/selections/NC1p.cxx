// Standard library
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

// XSecAnalyzer
#include "XSecAnalyzer/Constants.hh"
#include "XSecAnalyzer/FiducialVolume.hh"
#include "XSecAnalyzer/Functions.hh"
#include "XSecAnalyzer/Selections/NC1p.hh"

// ---- Event category codes ------------------------------------------------
// 0  NC 1p signal
// 1  NC other
// 2  CC nu_mu
// 3  CC nu_e
// 4  Out of FV / other MC
// 5  Cosmic / EXT data
// --------------------------------------------------------------------------

// Hardcoded base directory for the run-period-specific BDT weights.
static const std::string BDT_BASE_DIR =
  "/exp/uboone/app/users/renlu23/v61_2022/analysis/BDT/dataset";

NC1p::NC1p( int run_period )
  : SelectionBase( "NC1p" )
  , bdt_reader_( nullptr )
  , run_period_( run_period )
{}

NC1p::~NC1p()
{
  delete bdt_reader_;
}

void NC1p::define_constants()
{
  // True FV matches the old analysis FV used for signal definition
  this->define_true_FV( 21.5, 234.85, -95.0, 95.0, 21.5, 966.8 );
  // Reco proton containment volume (matches old analysis PCV)
  this->define_reco_FV( 10.0, 246.35, -96.35, 96.35, 10.0, 1026.8 );

  // Build the TMVA BDT reader once
  bdt_reader_ = new TMVA::Reader( "!Color:!Silent" );
  bdt_reader_->AddVariable( "reco_length",  &bdt_len_       );
  bdt_reader_->AddVariable( "reco_theta",   &bdt_theta_     );
  bdt_reader_->AddVariable( "reco_phi",     &bdt_phi_       );
  bdt_reader_->AddVariable( "end_dedx_2",   &bdt_end_dedx2_ );
  bdt_reader_->AddVariable( "total_dedx_2", &bdt_total_dedx2_ );
  bdt_reader_->AddVariable( "reco_start_y", &bdt_starty_    );
  bdt_reader_->AddVariable( "reco_start_z", &bdt_startz_    );
  bdt_reader_->AddVariable( "reco_end_y",   &bdt_endy_      );
  bdt_reader_->AddVariable( "reco_end_z",   &bdt_endz_      );
  bdt_reader_->AddVariable( "chi2_p_0",     &bdt_chi2p0_    );
  bdt_reader_->AddVariable( "chi2_p_1",     &bdt_chi2p1_    );
  bdt_reader_->AddVariable( "chi2_p_2",     &bdt_chi2p2_    );
  bdt_reader_->AddVariable( "trk_dis",      &bdt_trkdis_    );

  // Resolve BDT weights path.
  // Priority: NC1P_BDT_DIR env var (full override) > hardcoded run-specific path.
  std::string bdt_path;
  const char* bdt_override = std::getenv( "NC1P_BDT_DIR" );
  if ( bdt_override ) {
    bdt_path = std::string(bdt_override) + "/TMVAClassification_BDTG.weights.xml";
  }
  else {
    bdt_path = BDT_BASE_DIR
      + "/weights_" + std::to_string( run_period_ )
      + "/TMVAClassification_BDTG.weights.xml";
  }

  std::cout << "[NC1p] Run period " << run_period_
            << " — loading BDT weights from:\n  " << bdt_path << '\n';
  bdt_reader_->BookMVA( "BDTG", bdt_path.c_str() );
}

void NC1p::define_category_map()
{
  categ_map_[0] = { "NC 1p signal",    kBlue      };
  categ_map_[1] = { "NC other",        kGreen + 2 };
  categ_map_[2] = { "CC #nu_{#mu}",    kRed       };
  categ_map_[3] = { "CC #nu_{e}",      kOrange    };
  categ_map_[4] = { "Out of FV/other", kGray      };
  categ_map_[5] = { "Cosmic/EXT",      kBlack     };
}

void NC1p::reset()
{
  sel_1p_                    = false;
  sel_proton_found_          = false;
  sel_bdt_passed_            = false;
  sig_nc_interaction_        = false;
  sig_one_proton_above_thresh_ = false;
  sig_vertex_in_fv_          = false;
  Reco_KE_       = BOGUS;
  Reco_CosTheta_ = BOGUS;
  Reco_Momentum_ = BOGUS;
  Reco_Q2_       = BOGUS;
  True_KE_       = BOGUS;
  True_CosTheta_ = BOGUS;
  True_Momentum_ = BOGUS;
  True_Q2_       = BOGUS;
  BDT_Score_     = BOGUS;
  proton_idx_    = BOGUS_INDEX;
}

void NC1p::define_output_branches()
{
  // Selection flags
  set_branch( sel_1p_,               "sel_1p"               );
  set_branch( sel_proton_found_,     "sel_proton_found"     );
  set_branch( sel_bdt_passed_,       "sel_bdt_passed"       );

  // Signal flags
  set_branch( sig_nc_interaction_,          "sig_nc_interaction"          );
  set_branch( sig_one_proton_above_thresh_, "sig_one_proton_above_thresh" );
  set_branch( sig_vertex_in_fv_,            "sig_vertex_in_fv"            );

  // BDT score and proton index
  set_branch( BDT_Score_,  "BDT_Score"  );
  set_branch( proton_idx_, "proton_idx" );

  // Reco observables
  set_branch( Reco_KE_,       "Reco_KE"       );
  set_branch( Reco_CosTheta_, "Reco_CosTheta" );
  set_branch( Reco_Momentum_, "Reco_Momentum" );
  set_branch( Reco_Q2_,       "Reco_Q2"       );

  // True observables
  set_branch( True_KE_,       "True_KE"       );
  set_branch( True_CosTheta_, "True_CosTheta" );
  set_branch( True_Momentum_, "True_Momentum" );
  set_branch( True_Q2_,       "True_Q2"       );
}

bool NC1p::define_signal( AnalysisEvent* ev )
{
  // is_mc_ is set from file_type in ProcessNTuples.C before apply_selection()
  // is called. For the old format, MC truth branches are only valid for MC
  // files, so we check is_mc_ to avoid reading garbage.
  if ( !ev->is_mc_ ) return false;

  // NC interaction
  sig_nc_interaction_ = ( ev->mc_nu_ccnc_ == NEUTRAL_CURRENT );
  if ( !sig_nc_interaction_ ) return false;

  // Vertex in true FV
  sig_vertex_in_fv_ = point_inside_FV( this->true_FV(),
    ev->mc_nu_vx_, ev->mc_nu_vy_, ev->mc_nu_vz_ );
  if ( !sig_vertex_in_fv_ ) return false;

  // Exactly one above-threshold proton (uses pre-computed flag from ntuple)
  sig_one_proton_above_thresh_ = ( ev->evt_gen_nc1p_ == 1 );

  return sig_nc_interaction_ && sig_vertex_in_fv_ && sig_one_proton_above_thresh_;
}

bool NC1p::selection( AnalysisEvent* ev )
{
  // --- Cut 1: pre-computed single-proton reco flag ---
  sel_1p_ = ( ev->evt_reco_1p_ == 1 );
  if ( !sel_1p_ ) return false;

  // --- Cut 2: find the proton candidate ---
  // Mirrors the track-selection loop in make_tree.C:
  //   is_reco_nc1p && isinFV, containment (using f2 positions), length,
  //   forward costheta, chi2_p_2 < 60
  if ( !ev->reco_length_v_ || ev->reco_length_v_->empty() ) return false;

  const auto& len_v       = *ev->reco_length_v_;
  const auto& theta_f2_v  = *ev->reco_theta_f2_v_;
  const auto& phi_f2_v    = *ev->reco_phi_f2_v_;
  const auto& sx_v        = *ev->reco_start_x_v_;
  const auto& sy_v        = *ev->reco_start_y_v_;
  const auto& sz_v        = *ev->reco_start_z_v_;
  const auto& ex_v        = *ev->reco_end_x_v_;
  const auto& ey_v        = *ev->reco_end_y_v_;
  const auto& ez_v        = *ev->reco_end_z_v_;
  const auto& sxf2_v      = *ev->reco_start_x_f2_v_;
  const auto& syf2_v      = *ev->reco_start_y_f2_v_;
  const auto& szf2_v      = *ev->reco_start_z_f2_v_;
  const auto& exf2_v      = *ev->reco_end_x_f2_v_;
  const auto& eyf2_v      = *ev->reco_end_y_f2_v_;
  const auto& ezf2_v      = *ev->reco_end_z_f2_v_;
  const auto& chi2p0_v    = *ev->chi2_p_0_v_;
  const auto& chi2p1_v    = *ev->chi2_p_1_v_;
  const auto& chi2p2_v    = *ev->chi2_p_2_v_;
  const auto& dedx2_v     = *ev->start_dedx_2_v_;
  const auto& totdedx2_v  = *ev->total_dedx_2_v_;
  const auto& mom_p_v     = *ev->reco_mom_proton_v_;
  const auto& nc1p_v      = *ev->is_reco_nc1p_;
  const auto& fv_v        = *ev->isinFV_;

  const int n_trk = static_cast<int>( len_v.size() );
  float trkdis = 999.f;
  proton_idx_  = BOGUS_INDEX;

  for ( int i = 0; i < n_trk; ++i ) {
    if ( !nc1p_v.at(i) || fv_v.at(i) <= 0 ) continue;

    // Containment cuts use f2 positions (identical to make_tree.C)
    float sx = sxf2_v.at(i), sy = syf2_v.at(i), sz = szf2_v.at(i);
    float ex = exf2_v.at(i), ey = eyf2_v.at(i), ez = ezf2_v.at(i);
    if ( sx < 10.f || sx > 246.35f ) continue;
    if ( ex < 10.f || ex > 246.35f ) continue;
    if ( sy < -96.35f || sy > 96.35f ) continue;
    if ( ey < -96.35f || ey > 96.35f ) continue;
    if ( sz < 10.f || sz > 1026.8f ) continue;
    if ( ez < 10.f || ez > 1026.8f ) continue;

    float len = len_v.at(i);
    if ( len < 1.2f || len > 200.f ) continue;

    float costheta = std::cos( theta_f2_v.at(i) );
    if ( costheta < 0.f ) continue;

    float c2 = chi2p2_v.at(i);
    if ( c2 < 0.f || c2 > 60.f ) continue;

    // Track-to-track distance (trkdis): minimum distance from this candidate's
    // f2 start to any other track's (non-f2) start or end.
    // Identical to the inner loop in make_tree.C lines 461-467.
    if ( n_trk > 1 ) {
      for ( int j = 0; j < n_trk; ++j ) {
        float d1 = std::sqrt( std::pow(sx - sx_v.at(j), 2)
                            + std::pow(sy - sy_v.at(j), 2)
                            + std::pow(sz - sz_v.at(j), 2) );
        float d2 = std::sqrt( std::pow(sx - ex_v.at(j), 2)
                            + std::pow(sy - ey_v.at(j), 2)
                            + std::pow(sz - ez_v.at(j), 2) );
        if ( d1 > 0.f && d2 > 0.f ) {
          float dmin = ( d1 < d2 ) ? d1 : d2;
          if ( dmin < trkdis ) trkdis = dmin;
        }
      }
    }

    proton_idx_ = i;
  }

  sel_proton_found_ = ( proton_idx_ != BOGUS_INDEX );
  if ( !sel_proton_found_ ) return false;

  const int ip = proton_idx_;

  // --- Cut 3: BDT ---
  // Populate BDT input variables exactly as make_tree.C does.
  // NOTE: dedx_end2 in the BDT = start_dedx_2 branch (historical naming).
  //       start_y/z and end_y/z use the non-f2 values (overwritten in make_tree.C).
  bdt_len_        = len_v.at(ip);
  bdt_theta_      = theta_f2_v.at(ip);   // f2 theta
  bdt_phi_        = phi_f2_v.at(ip);     // f2 phi
  bdt_end_dedx2_  = dedx2_v.at(ip);      // start_dedx_2 branch → BDT "end_dedx_2"
  bdt_total_dedx2_= totdedx2_v.at(ip);
  bdt_starty_     = sy_v.at(ip);         // non-f2 start y
  bdt_startz_     = sz_v.at(ip);         // non-f2 start z
  bdt_endy_       = ey_v.at(ip);         // non-f2 end y
  bdt_endz_       = ez_v.at(ip);         // non-f2 end z
  bdt_chi2p0_     = chi2p0_v.at(ip);
  bdt_chi2p1_     = chi2p1_v.at(ip);
  bdt_chi2p2_     = chi2p2_v.at(ip);
  bdt_trkdis_     = trkdis;

  BDT_Score_ = bdt_reader_->EvaluateMVA( "BDTG" );

  sel_bdt_passed_ = ( BDT_Score_ > BDT_CUT );
  return sel_bdt_passed_;
}

void NC1p::compute_reco_observables( AnalysisEvent* ev )
{
  if ( proton_idx_ == BOGUS_INDEX ) return;
  const int ip = proton_idx_;

  const float len = ev->reco_length_v_->at(ip);

  // Range-based kinetic energy (GeV), same formula as make_tree.C line 490
  Reco_KE_       = 31.3 * std::pow( len, 0.578 ) / 1000.0;
  Reco_Q2_       = Reco_KE_ * 2.0 * PROTON_MASS;
  Reco_CosTheta_ = std::cos( ev->reco_theta_f2_v_->at(ip) );
  Reco_Momentum_ = ev->reco_mom_proton_v_->at(ip);
}

void NC1p::compute_true_observables( AnalysisEvent* ev )
{
  if ( ev->evt_gen_nc1p_ != 1 ) return;

  True_KE_       = ev->evt_gen_nc1p_ke_;
  True_CosTheta_ = ev->evt_gen_nc1p_costheta_;
  True_Momentum_ = ev->evt_gen_nc1p_mom_;
  True_Q2_       = ev->evt_gen_nc1p_q2_gen_;
}

int NC1p::categorize_event( AnalysisEvent* ev )
{
  if ( !ev->is_mc_ ) {
    return 5; // EXT / cosmic data
  }

  // Vertex must be in true FV to be categorised as a physics category
  bool in_fv = point_inside_FV( this->true_FV(),
    ev->mc_nu_vx_, ev->mc_nu_vy_, ev->mc_nu_vz_ );
  if ( !in_fv ) return 4;

  const int ccnc  = ev->mc_nu_ccnc_;
  const int nupdg = std::abs( ev->mc_nu_pdg_ );

  if ( ccnc == NEUTRAL_CURRENT ) {
    return ( ev->evt_gen_nc1p_ == 1 ) ? 0 : 1;
  }
  // CC
  if ( nupdg == MUON_NEUTRINO  ) return 2;
  if ( nupdg == ELECTRON_NEUTRINO ) return 3;
  return 4;
}
