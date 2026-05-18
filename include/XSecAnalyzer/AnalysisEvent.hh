#pragma once

// STV analysis includes
#include "TreeUtils.hh"
#include "FiducialVolume.hh"
#include "Constants.hh"

#include <vector>
#include <map>

#include "TVector3.h"

class AnalysisEvent{
public:
  AnalysisEvent() {}
  ~AnalysisEvent() {}

  // Event scores needed for numu CC selection
  float topological_score_ = BOGUS;
  float cosmic_impact_parameter_ = BOGUS;

  // Variables needed for nue CC selection
  float contained_fraction_ = BOGUS;
  
  // Backtracked purity and completeness of hits (MC only)
  float nu_completeness_from_pfp_ = BOGUS;
  float nu_purity_from_pfp_ = BOGUS;

  // Reco PDG code of the neutrino candidate
  int nu_pdg_ = BOGUS_INT;

  // Number of neutrino slices identified by the SliceID. Allowed values
  // are zero or one.
  int nslice_ = BOGUS_INT;

  // Reco neutrino vertex coordinates (cm). Space charge corrections have
  // been applied for these.
  float nu_vx_ = BOGUS;
  float nu_vy_ = BOGUS;
  float nu_vz_ = BOGUS;

  // Reconstructed object counts
  int num_pf_particles_ = BOGUS_INT;
  int num_tracks_ = BOGUS_INT;
  int num_showers_ = BOGUS_INT;

  // PFParticle properties
  MyPointer< std::vector<unsigned int> > pfp_generation_;
  MyPointer< std::vector<unsigned int> > pfp_trk_daughters_count_;
  MyPointer< std::vector<unsigned int> > pfp_shr_daughters_count_;

  MyPointer< std::vector<float> > pfp_track_score_;

  // Reco PDG code assigned by Pandora
  MyPointer< std::vector<int> > pfp_reco_pdg_;

  // Total number of wire plane hits associated with each PFParticle
  MyPointer< std::vector<int> > pfp_hits_;

  // Number of hits on the three individual planes
  // (Y is the collection plane)
  MyPointer< std::vector<int> > pfp_hitsU_;
  MyPointer< std::vector<int> > pfp_hitsV_;
  MyPointer< std::vector<int> > pfp_hitsY_;

  // True PDG code found using the backtracker
  MyPointer< std::vector<int> > pfp_true_pdg_;

  // True 4-momentum components found using the backtracker
  MyPointer< std::vector<float> > pfp_true_E_;
  MyPointer< std::vector<float> > pfp_true_px_;
  MyPointer< std::vector<float> > pfp_true_py_;
  MyPointer< std::vector<float> > pfp_true_pz_;

  // Shower properties
  MyPointer< std::vector<unsigned long> > shower_pfp_id_;
  MyPointer< std::vector<float> > shower_startx_;
  MyPointer< std::vector<float> > shower_starty_;
  MyPointer< std::vector<float> > shower_startz_;
  MyPointer< std::vector<float> > shower_start_distance_;

  // Primary shower
  int shr_id_ = BOGUS_INT;
  float shr_energy_cali_ = BOGUS;
  float shr_score_ = BOGUS;
  int shrsubclusters_ = BOGUS_INT;
  float hits_ratio_ = BOGUS;
  float shrmoliereavg_ = BOGUS;
  float shr_distance_ = BOGUS;
  float shr_tkfit_gap10_dedx_Y_ = BOGUS;
  float shr_tkfit_2cm_dedx_Y_ = BOGUS;

  // Track properties
  MyPointer< std::vector<unsigned long> > track_pfp_id_;
  MyPointer< std::vector<float> > track_length_;
  MyPointer< std::vector<float> > track_startx_;
  MyPointer< std::vector<float> > track_starty_;
  MyPointer< std::vector<float> > track_startz_;
  MyPointer< std::vector<float> > track_start_distance_;
  MyPointer< std::vector<float> > track_endx_;
  MyPointer< std::vector<float> > track_endy_;
  MyPointer< std::vector<float> > track_endz_;
  MyPointer< std::vector<float> > track_dirx_;
  MyPointer< std::vector<float> > track_diry_;
  MyPointer< std::vector<float> > track_dirz_;
  MyPointer< std::vector<float> > track_theta_;
  MyPointer< std::vector<float> > track_phi_;

  // Proton *kinetic* energy using range-based momentum reconstruction
  MyPointer< std::vector<float> > track_kinetic_energy_p_;

  MyPointer< std::vector<float> > track_range_mom_mu_;
  MyPointer< std::vector<float> > track_mcs_mom_mu_;
  MyPointer< std::vector<float> > track_chi2_proton_;

  // Log-likelihood ratio particle ID information

  // Product of muon/proton log-likelihood ratios from all wire three planes
  MyPointer< std::vector<float> > track_llr_pid_;

  // Individual wire plane muon/proton log-likelihood ratios
  MyPointer< std::vector<float> > track_llr_pid_U_;
  MyPointer< std::vector<float> > track_llr_pid_V_;
  MyPointer< std::vector<float> > track_llr_pid_Y_;

  // Rescaled overall PID score (all three planes) that lies
  // on the interval [-1, 1]
  MyPointer< std::vector<float> > track_llr_pid_score_;

  // True neutrino PDG code
  int mc_nu_pdg_ = BOGUS_INT;

  // True neutrino vertex coordinates (cm)
  float mc_nu_vx_ = BOGUS;
  float mc_nu_vy_ = BOGUS;
  float mc_nu_vz_ = BOGUS;

  float mc_nu_sce_vx_ = BOGUS;
  float mc_nu_sce_vy_ = BOGUS;
  float mc_nu_sce_vz_ = BOGUS;

  // True neutrino 4-momentum
  float mc_nu_energy_ = BOGUS;

  // Whether the event is CC (0) or NC (1)
  int mc_nu_ccnc_ = false;

  // Interaction mode (QE, MEC, etc.)
  int mc_nu_interaction_type_ = BOGUS_INT;

  // Final-state particle PDG codes and energies (post-FSIs)
  MyPointer< std::vector<int> > mc_nu_daughter_pdg_;
  MyPointer< std::vector<float> > mc_nu_daughter_energy_;
  MyPointer< std::vector<float> > mc_nu_daughter_px_;
  MyPointer< std::vector<float> > mc_nu_daughter_py_;
  MyPointer< std::vector<float> > mc_nu_daughter_pz_;

  // General systematic weights
  MyPointer< std::map< std::string, std::vector<double> > > mc_weights_map_;
  // Map of pointers used to set output branch addresses for the elements
  // of the weights map. Hacky, but it works.
  // TODO: revisit this to make something more elegant
  std::map< std::string, std::vector<double>* > mc_weights_ptr_map_;

  // GENIE weights
  float spline_weight_ = DEFAULT_WEIGHT;
  float tuned_cv_weight_ = DEFAULT_WEIGHT;
  float ppfx_cv_weight_ = DEFAULT_WEIGHT;
  float normalisation_weight_ = DEFAULT_WEIGHT;

  // NuMI: beamline geometry weights
  // beamline variations
  bool beamlineVarWeightsPresent_ = false;
  // true nu angle from numi beamline 
  float nu_angle;
  // variations
  std::vector<double> Horn_2kA;
  std::vector<double> Horn1_x_3mm;
  std::vector<double> Horn1_y_3mm;
  std::vector<double> Beam_spot_1_1mm;
  std::vector<double> Beam_spot_1_5mm;
  std::vector<double> Horn2_x_3mm;
  std::vector<double> Horn2_y_3mm;
  std::vector<double> Horns_0mm_water;
  std::vector<double> Horns_2mm_water;
  std::vector<double> Beam_shift_x_1mm;
  std::vector<double> Beam_shift_y_1mm;
  std::vector<double> Target_z_7mm;

  // Signal definition requirements
  bool is_mc_ = false;

  // truth electron information
  int mc_nelec_ = BOGUS_INT;
  int mc_npi0_ = BOGUS_INT;
  float mc_elec_e_ = BOGUS;

  //================================================================================================================
  // ** NC1p old-format ntuple members (SingleProtonAna/tree) **

  // Truth: pre-computed NC1p signal flag and true proton kinematics
  int   evt_gen_nc1p_        = BOGUS_INT;
  float evt_gen_nc1p_ke_     = BOGUS;
  float evt_gen_nc1p_costheta_ = BOGUS;
  float evt_gen_nc1p_mom_    = BOGUS;
  float evt_gen_nc1p_q2_gen_ = BOGUS;

  // Truth: additional MC info needed for categorisation
  int mc_n_proton_            = BOGUS_INT;
  int mc_mode_                = BOGUS_INT;
  int mc_n_threshold_muon_    = BOGUS_INT;
  int mc_n_threshold_proton_  = BOGUS_INT;
  int mc_n_threshold_pionpm_  = BOGUS_INT;
  int mc_n_threshold_pion0_   = BOGUS_INT;

  // Reco: pre-computed NC1p selection flag
  int evt_reco_1p_ = BOGUS_INT;

  // Reco: per-track vectors used for proton candidate finding and BDT
  MyPointer< std::vector<bool>  > is_reco_nc1p_;
  MyPointer< std::vector<int>   > isinFV_;
  MyPointer< std::vector<float> > reco_length_v_;
  MyPointer< std::vector<float> > reco_theta_f2_v_;
  MyPointer< std::vector<float> > reco_phi_f2_v_;
  MyPointer< std::vector<float> > reco_start_x_v_;
  MyPointer< std::vector<float> > reco_start_y_v_;
  MyPointer< std::vector<float> > reco_start_z_v_;
  MyPointer< std::vector<float> > reco_end_x_v_;
  MyPointer< std::vector<float> > reco_end_y_v_;
  MyPointer< std::vector<float> > reco_end_z_v_;
  MyPointer< std::vector<float> > reco_start_x_f2_v_;
  MyPointer< std::vector<float> > reco_start_y_f2_v_;
  MyPointer< std::vector<float> > reco_start_z_f2_v_;
  MyPointer< std::vector<float> > reco_end_x_f2_v_;
  MyPointer< std::vector<float> > reco_end_y_f2_v_;
  MyPointer< std::vector<float> > reco_end_z_f2_v_;
  MyPointer< std::vector<float> > chi2_p_0_v_;
  MyPointer< std::vector<float> > chi2_p_1_v_;
  MyPointer< std::vector<float> > chi2_p_2_v_;
  MyPointer< std::vector<float> > start_dedx_2_v_;   // NB: labelled "end_dedx_2" in BDT
  MyPointer< std::vector<float> > total_dedx_2_v_;
  MyPointer< std::vector<float> > reco_mom_proton_v_;

  // Reco: blip cluster positions (used for upstream-blip cut)
  MyPointer< std::vector<float> > blip_x_;
  MyPointer< std::vector<float> > blip_y_;
  MyPointer< std::vector<float> > blip_z_;

  // NC1p old-format systematic weight branches
  MyPointer< std::vector<std::string>           > evtwgt_genie_multisim_funcname_;
  MyPointer< std::vector<std::vector<double> >  > evtwgt_genie_multisim_weight_;
  MyPointer< std::vector<std::vector<double> >  > evtwgt_flux_multisim_weight_;
  MyPointer< std::vector<std::vector<double> >  > evtwgt_g4_multisim_weight_;

  //================================================================================================================
  // ** Reconstructed observables **

};
