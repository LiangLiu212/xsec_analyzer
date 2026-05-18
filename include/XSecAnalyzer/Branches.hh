#pragma once

// ROOT includes
#include "TTree.h"
#include "AnalysisEvent.hh"

void SetBranchAddress(TTree& etree, std::string BranchName, void* Variable) {
  etree.SetBranchAddress(BranchName.c_str(),Variable);
}

// Helper function to set branch addresses for reading information
// from the Event TTree
void set_event_branch_addresses(TTree& etree, AnalysisEvent& ev)
{
  // Reco PDG code of primary PFParticle in slice (i.e., the neutrino
  // candidate)
  SetBranchAddress(etree, "slpdg", &ev.nu_pdg_ );

  // Number of neutrino slices identified by the SliceID. Allowed values
  // are zero or one.
  //SetBranchAddress(etree,"nslice",&ev.nslice_);
  SetBranchAddress(etree, "nslice", &ev.nslice_ );

  // Topological score
  SetBranchAddress(etree, "topological_score", &ev.topological_score_ );
  SetBranchAddress(etree, "CosmicIP", &ev.cosmic_impact_parameter_ );
  //SetBranchAddress(etree, "CosmicIPAll3D", &ev.CosmicIPAll3D_ );

  // containment fraction
  SetBranchAddress(etree, "contained_fraction", &ev.contained_fraction_ );

  // Reconstructed neutrino vertex position (with corrections for
  // space charge applied)
  SetBranchAddress(etree, "reco_nu_vtx_sce_x", &ev.nu_vx_ );
  SetBranchAddress(etree, "reco_nu_vtx_sce_y", &ev.nu_vy_ );
  SetBranchAddress(etree, "reco_nu_vtx_sce_z", &ev.nu_vz_ );

  // Reconstructed object counts
  SetBranchAddress(etree, "n_pfps", &ev.num_pf_particles_ );
  SetBranchAddress(etree, "n_tracks", &ev.num_tracks_ );
  SetBranchAddress(etree, "n_showers", &ev.num_showers_ );

  // PFParticle properties
  set_object_input_branch_address( etree, "pfp_generation_v",
    ev.pfp_generation_ );

  set_object_input_branch_address( etree, "pfp_trk_daughters_v",
    ev.pfp_trk_daughters_count_ );

  set_object_input_branch_address( etree, "pfp_shr_daughters_v",
    ev.pfp_shr_daughters_count_ );

  set_object_input_branch_address( etree, "trk_score_v", ev.pfp_track_score_ );
  set_object_input_branch_address( etree, "pfpdg", ev.pfp_reco_pdg_ );
  set_object_input_branch_address( etree, "pfnhits", ev.pfp_hits_ );
  set_object_input_branch_address( etree, "pfnplanehits_U", ev.pfp_hitsU_ );
  set_object_input_branch_address( etree, "pfnplanehits_V", ev.pfp_hitsV_ );
  set_object_input_branch_address( etree, "pfnplanehits_Y", ev.pfp_hitsY_ );

  // Backtracked PFParticle properties
  set_object_input_branch_address( etree, "backtracked_pdg", ev.pfp_true_pdg_ );
  set_object_input_branch_address( etree, "backtracked_e", ev.pfp_true_E_ );
  set_object_input_branch_address( etree, "backtracked_px", ev.pfp_true_px_ );
  set_object_input_branch_address( etree, "backtracked_py", ev.pfp_true_py_ );
  set_object_input_branch_address( etree, "backtracked_pz", ev.pfp_true_pz_ );

  // Shower properties
  // These are excluded from some ntuples to ensure blindness for the LEE
  // analyses. We will skip them when not available.
  bool has_shower_branches = ( etree.GetBranch("shr_pfp_id_v") != nullptr );
  if ( has_shower_branches ) {
    set_object_input_branch_address( etree, "shr_pfp_id_v", ev.shower_pfp_id_ );
    set_object_input_branch_address( etree, "shr_start_x_v", ev.shower_startx_ );
    set_object_input_branch_address( etree, "shr_start_y_v", ev.shower_starty_ );
    set_object_input_branch_address( etree, "shr_start_z_v", ev.shower_startz_ );
    // Shower start distance from reco neutrino vertex (pre-calculated for
    // convenience)
    set_object_input_branch_address( etree, "shr_dist_v",
      ev.shower_start_distance_ );
  }
  else {
    // When the shower information is not available, delete the owned vectors
    // to signal that the associated branches should not be written to the
    // output TTree
    ev.shower_pfp_id_.reset( nullptr );
    ev.shower_startx_.reset( nullptr );
    ev.shower_starty_.reset( nullptr );
    ev.shower_startz_.reset( nullptr );
    ev.shower_start_distance_.reset( nullptr );
  }

  // primary shower
  SetBranchAddress(etree, "shr_id", &ev.shr_id_ );
  SetBranchAddress(etree, "shr_score", &ev.shr_score_ );
  SetBranchAddress(etree, "shr_energy_cali", &ev.shr_energy_cali_ );
  SetBranchAddress(etree, "hits_ratio", &ev.hits_ratio_ );
  SetBranchAddress(etree, "shrmoliereavg", &ev.shrmoliereavg_ );
  SetBranchAddress(etree, "shr_distance", &ev.shr_distance_ );
  SetBranchAddress(etree, "shr_tkfit_gap10_dedx_Y", &ev.shr_tkfit_gap10_dedx_Y_ );
  SetBranchAddress(etree, "shr_tkfit_2cm_dedx_Y", &ev.shr_tkfit_2cm_dedx_Y_ );

  // Track properties
  set_object_input_branch_address( etree, "trk_pfp_id_v", ev.track_pfp_id_ );
  set_object_input_branch_address( etree, "trk_len_v", ev.track_length_ );
  set_object_input_branch_address( etree, "trk_sce_start_x_v", ev.track_startx_ );
  set_object_input_branch_address( etree, "trk_sce_start_y_v", ev.track_starty_ );
  set_object_input_branch_address( etree, "trk_sce_start_z_v", ev.track_startz_ );

  // Track start distance from reco neutrino vertex (pre-calculated for
  // convenience)
  set_object_input_branch_address( etree, "trk_distance_v",
    ev.track_start_distance_ );

  set_object_input_branch_address( etree, "trk_sce_end_x_v", ev.track_endx_ );
  set_object_input_branch_address( etree, "trk_sce_end_y_v", ev.track_endy_ );
  set_object_input_branch_address( etree, "trk_sce_end_z_v", ev.track_endz_ );

  set_object_input_branch_address( etree, "trk_dir_x_v", ev.track_dirx_ );
  set_object_input_branch_address( etree, "trk_dir_y_v", ev.track_diry_ );
  set_object_input_branch_address( etree, "trk_dir_z_v", ev.track_dirz_ );

  set_object_input_branch_address( etree, "trk_theta_v", ev.track_theta_ );
  set_object_input_branch_address( etree, "trk_phi_v", ev.track_phi_ );

  set_object_input_branch_address( etree, "trk_energy_proton_v",
    ev.track_kinetic_energy_p_ );

  set_object_input_branch_address( etree, "trk_range_muon_mom_v",
    ev.track_range_mom_mu_ );

  set_object_input_branch_address( etree, "trk_mcs_muon_mom_v",
    ev.track_mcs_mom_mu_ );

  // Some ntuples exclude the old proton chi^2 PID score. Only include it
  // in the output if this branch is available.
  bool has_chipr = ( etree.GetBranch("trk_pid_chipr_v") != nullptr );
  if ( has_chipr ) {
    set_object_input_branch_address( etree, "trk_pid_chipr_v",
      ev.track_chi2_proton_ );
  }
  else {
    ev.track_chi2_proton_.reset( nullptr );
  }

  // Log-likelihood-based particle ID information
  set_object_input_branch_address( etree, "trk_llr_pid_v", ev.track_llr_pid_ );

  set_object_input_branch_address( etree, "trk_llr_pid_u_v",
    ev.track_llr_pid_U_ );

  set_object_input_branch_address( etree, "trk_llr_pid_v_v",
    ev.track_llr_pid_V_ );

  set_object_input_branch_address( etree, "trk_llr_pid_y_v",
    ev.track_llr_pid_Y_ );

  set_object_input_branch_address( etree, "trk_llr_pid_score_v",
    ev.track_llr_pid_score_ );

  // MC truth information for the neutrino
  SetBranchAddress(etree, "nu_pdg", &ev.mc_nu_pdg_ );
  SetBranchAddress(etree, "true_nu_vtx_x", &ev.mc_nu_vx_ );
  SetBranchAddress(etree, "true_nu_vtx_y", &ev.mc_nu_vy_ );
  SetBranchAddress(etree, "true_nu_vtx_z", &ev.mc_nu_vz_ );
  SetBranchAddress(etree, "nu_e", &ev.mc_nu_energy_ );
  SetBranchAddress(etree, "ccnc", &ev.mc_nu_ccnc_ );
  SetBranchAddress(etree, "interaction", &ev.mc_nu_interaction_type_ );

  //=============================================
  //DB Added to match Samantha's Signal defintion

  SetBranchAddress(etree, "true_nu_vtx_sce_x", &ev.mc_nu_sce_vx_ );
  SetBranchAddress(etree, "true_nu_vtx_sce_y", &ev.mc_nu_sce_vy_ );
  SetBranchAddress(etree, "true_nu_vtx_sce_z", &ev.mc_nu_sce_vz_ );

  //=============================================

  // MC truth information for the final-state primary particles
  set_object_input_branch_address( etree, "mc_pdg", ev.mc_nu_daughter_pdg_ );
  set_object_input_branch_address( etree, "mc_E", ev.mc_nu_daughter_energy_ );
  set_object_input_branch_address( etree, "mc_px", ev.mc_nu_daughter_px_ );
  set_object_input_branch_address( etree, "mc_py", ev.mc_nu_daughter_py_ );
  set_object_input_branch_address( etree, "mc_pz", ev.mc_nu_daughter_pz_ );

  // GENIE and other systematic variation weights
  bool has_genie_mc_weights = ( etree.GetBranch("weightSpline") != nullptr );
  if ( has_genie_mc_weights ) {
    SetBranchAddress(etree, "weightSpline", &ev.spline_weight_ );
    SetBranchAddress(etree, "weightTune", &ev.tuned_cv_weight_ );
    SetBranchAddress(etree, "ppfx_cv", &ev.ppfx_cv_weight_ );
  }

  bool has_weight_map = ( etree.GetBranch("weights") != nullptr );
  if ( has_weight_map ) {
    set_object_input_branch_address( etree, "weights", ev.mc_weights_map_ );
  }
  else {
    ev.mc_weights_map_.reset( nullptr );
  }

  // Purity and completeness of the backtracked hits in the neutrino slice
  bool has_pfp_backtracked_purity = ( etree.GetBranch("nu_purity_from_pfp")
    != nullptr );
  if ( has_pfp_backtracked_purity ) {

    SetBranchAddress(etree, "nu_completeness_from_pfp",
      &ev.nu_completeness_from_pfp_ );

    SetBranchAddress(etree, "nu_purity_from_pfp", &ev.nu_purity_from_pfp_ );

  }

  // truth particle multiplicity information
  SetBranchAddress(etree, "nelec", &ev.mc_nelec_ ); // Number electrons
  SetBranchAddress(etree, "npi0", &ev.mc_npi0_ ); // Number neutral pions
  SetBranchAddress(etree, "elec_e", &ev.mc_elec_e_ ); // Electron energy
}

// Sets branch addresses for the OLD NC1p ntuple format (SingleProtonAna/tree).
// Maps old branch names onto existing AnalysisEvent members where semantics
// match, and into the NC1p-specific members otherwise.
void set_event_branch_addresses_nc1p(TTree& etree, AnalysisEvent& ev)
{
  // --- Truth branches: absent in data (BNB/EXT) files ---
  // evt_gen_nc1p is a computed NC1p truth variable; present only in MC files.
  // Standard neutrino truth branches (mc_nupdg etc.) exist in BNB/EXT files too
  // but are filled with dummy values, so they cannot be used as a sentinel.
  bool has_mc_truth = ( etree.GetBranch("evt_gen_nc1p") != nullptr );
  if ( has_mc_truth ) {
    SetBranchAddress(etree, "mc_nupdg",   &ev.mc_nu_pdg_   );
    SetBranchAddress(etree, "mc_ccnc",    &ev.mc_nu_ccnc_  );
    SetBranchAddress(etree, "mc_nu_vtxx", &ev.mc_nu_vx_    );
    SetBranchAddress(etree, "mc_nu_vtxy", &ev.mc_nu_vy_    );
    SetBranchAddress(etree, "mc_nu_vtxz", &ev.mc_nu_vz_    );
    SetBranchAddress(etree, "mc_enu",     &ev.mc_nu_energy_);

    // CV weights – default to 1.0 when absent (common in older MC files).
    bool has_mc_wgts = ( etree.GetBranch("mc_wgt_v4a") != nullptr );
    if ( has_mc_wgts ) {
      SetBranchAddress(etree, "mc_wgt_v4a",    &ev.spline_weight_    );
      SetBranchAddress(etree, "mc_wgt_tunedcv",&ev.tuned_cv_weight_  );
    }
    else {
      ev.spline_weight_    = 1.f;
      ev.tuned_cv_weight_  = 1.f;
    }

    // NC1p-specific truth members
    SetBranchAddress(etree, "evt_gen_nc1p",          &ev.evt_gen_nc1p_        );
    SetBranchAddress(etree, "evt_gen_nc1p_ke",        &ev.evt_gen_nc1p_ke_     );
    SetBranchAddress(etree, "evt_gen_nc1p_costheta",  &ev.evt_gen_nc1p_costheta_);
    SetBranchAddress(etree, "evt_gen_nc1p_mom",       &ev.evt_gen_nc1p_mom_    );
    SetBranchAddress(etree, "evt_gen_nc1p_q2_gen",    &ev.evt_gen_nc1p_q2_gen_ );
    SetBranchAddress(etree, "mc_n_proton",            &ev.mc_n_proton_         );
    SetBranchAddress(etree, "mc_mode",                &ev.mc_mode_             );

    if ( etree.GetBranch("mc_n_threshold_muon")   != nullptr )
      SetBranchAddress(etree, "mc_n_threshold_muon",   &ev.mc_n_threshold_muon_  );
    if ( etree.GetBranch("mc_n_threshold_proton") != nullptr )
      SetBranchAddress(etree, "mc_n_threshold_proton", &ev.mc_n_threshold_proton_);
    if ( etree.GetBranch("mc_n_threshold_pionpm") != nullptr )
      SetBranchAddress(etree, "mc_n_threshold_pionpm", &ev.mc_n_threshold_pionpm_);
    if ( etree.GetBranch("mc_n_threshold_pion0")  != nullptr )
      SetBranchAddress(etree, "mc_n_threshold_pion0",  &ev.mc_n_threshold_pion0_ );
  }

  // --- Reco selection flag ---
  SetBranchAddress(etree, "evt_reco_1p", &ev.evt_reco_1p_ );

  // --- Per-track reco branches ---
  set_object_input_branch_address(etree, "is_reco_nc1p",    ev.is_reco_nc1p_   );
  set_object_input_branch_address(etree, "isinFV",          ev.isinFV_         );
  set_object_input_branch_address(etree, "reco_length",     ev.reco_length_v_  );
  set_object_input_branch_address(etree, "reco_theta_f2",   ev.reco_theta_f2_v_);
  set_object_input_branch_address(etree, "reco_phi_f2",     ev.reco_phi_f2_v_  );
  set_object_input_branch_address(etree, "reco_start_x",    ev.reco_start_x_v_ );
  set_object_input_branch_address(etree, "reco_start_y",    ev.reco_start_y_v_ );
  set_object_input_branch_address(etree, "reco_start_z",    ev.reco_start_z_v_ );
  set_object_input_branch_address(etree, "reco_end_x",      ev.reco_end_x_v_   );
  set_object_input_branch_address(etree, "reco_end_y",      ev.reco_end_y_v_   );
  set_object_input_branch_address(etree, "reco_end_z",      ev.reco_end_z_v_   );
  set_object_input_branch_address(etree, "reco_start_x_f2", ev.reco_start_x_f2_v_);
  set_object_input_branch_address(etree, "reco_start_y_f2", ev.reco_start_y_f2_v_);
  set_object_input_branch_address(etree, "reco_start_z_f2", ev.reco_start_z_f2_v_);
  set_object_input_branch_address(etree, "reco_end_x_f2",   ev.reco_end_x_f2_v_  );
  set_object_input_branch_address(etree, "reco_end_y_f2",   ev.reco_end_y_f2_v_  );
  set_object_input_branch_address(etree, "reco_end_z_f2",   ev.reco_end_z_f2_v_  );
  set_object_input_branch_address(etree, "chi2_p_0",        ev.chi2_p_0_v_     );
  set_object_input_branch_address(etree, "chi2_p_1",        ev.chi2_p_1_v_     );
  set_object_input_branch_address(etree, "chi2_p_2",        ev.chi2_p_2_v_     );
  set_object_input_branch_address(etree, "start_dedx_2",    ev.start_dedx_2_v_ );
  set_object_input_branch_address(etree, "total_dedx_2",    ev.total_dedx_2_v_ );
  set_object_input_branch_address(etree, "reco_mom_proton", ev.reco_mom_proton_v_);

  // --- Blip cluster branches (absent from some files) ---
  bool has_blip = ( etree.GetBranch("blip_x") != nullptr );
  if ( has_blip ) {
    set_object_input_branch_address(etree, "blip_x", ev.blip_x_);
    set_object_input_branch_address(etree, "blip_y", ev.blip_y_);
    set_object_input_branch_address(etree, "blip_z", ev.blip_z_);
  }
  else {
    ev.blip_x_.reset( nullptr );
    ev.blip_y_.reset( nullptr );
    ev.blip_z_.reset( nullptr );
  }

  // --- Systematic weight branches ---
  bool has_genie = ( etree.GetBranch("evtwgt_genie_multisim_weight") != nullptr );
  if ( has_genie ) {
    set_object_input_branch_address(etree, "evtwgt_genie_multisim_funcname",
      ev.evtwgt_genie_multisim_funcname_);
    set_object_input_branch_address(etree, "evtwgt_genie_multisim_weight",
      ev.evtwgt_genie_multisim_weight_);
  }
  else {
    ev.evtwgt_genie_multisim_funcname_.reset( nullptr );
    ev.evtwgt_genie_multisim_weight_.reset( nullptr );
  }

  bool has_flux = ( etree.GetBranch("evtwgt_flux_multisim_weight") != nullptr );
  if ( has_flux ) {
    set_object_input_branch_address(etree, "evtwgt_flux_multisim_weight",
      ev.evtwgt_flux_multisim_weight_);
  }
  else {
    ev.evtwgt_flux_multisim_weight_.reset( nullptr );
  }

  bool has_g4 = ( etree.GetBranch("evtwgt_g4_multisim_weight") != nullptr );
  if ( has_g4 ) {
    set_object_input_branch_address(etree, "evtwgt_g4_multisim_weight",
      ev.evtwgt_g4_multisim_weight_);
  }
  else {
    ev.evtwgt_g4_multisim_weight_.reset( nullptr );
  }

  // Signal the absence of a `weights` map branch so the output setup does not
  // try to read one from the input tree.
  ev.mc_weights_map_.reset( nullptr );
}

// Populates ev.mc_weights_map_ from the old-format weight branches after a
// call to GetEntry().  Must be called on every event; uses operator[] on the
// map so that vectors are created on first call and updated in-place
// subsequently (preserving the addresses needed by the output TTree).
void build_nc1p_weight_map(AnalysisEvent& ev)
{
  // Allocate the map if set_event_branch_addresses_nc1p reset it to nullptr
  if ( !ev.mc_weights_map_ ) {
    ev.mc_weights_map_.reset( new std::map<std::string, std::vector<double>>() );
  }
  auto& wmap = *ev.mc_weights_map_;

  // CV weights stored as single-element vectors.
  // Keys must match SPLINE_WEIGHT_NAME and TUNE_WEIGHT_NAME (without "weight_"
  // prefix, since set_event_output_branch_addresses prepends it).
  wmap["splines_general_Spline"]      = { static_cast<double>(ev.spline_weight_)     };
  wmap["TunedCentralValue_UBGenie"]   = { static_cast<double>(ev.tuned_cv_weight_)   };

  // GENIE multisim weights: one entry per function, 100 universes each.
  if ( ev.evtwgt_genie_multisim_weight_ && ev.evtwgt_genie_multisim_funcname_ ) {
    const auto& funcnames = *ev.evtwgt_genie_multisim_funcname_;
    const auto& weights   = *ev.evtwgt_genie_multisim_weight_;
    for ( size_t l = 0; l < weights.size(); ++l ) {
      const std::string key = "genie_multisim_" + funcnames.at(l);
      auto& vec = wmap[key];
      vec = weights.at(l);
    }
  }

  // Flux multisim weights: 13 variations × 100 universes.
  if ( ev.evtwgt_flux_multisim_weight_ ) {
    const auto& weights = *ev.evtwgt_flux_multisim_weight_;
    for ( size_t l = 0; l < weights.size(); ++l ) {
      const std::string key = "flux_multisim_" + std::to_string(l);
      auto& vec = wmap[key];
      vec = weights.at(l);
    }
  }

  // G4 reinteraction multisim weights: sub-indices 1, 3, 4, 5.
  if ( ev.evtwgt_g4_multisim_weight_ ) {
    const auto& weights = *ev.evtwgt_g4_multisim_weight_;
    static const std::vector<int> G4_INDICES = { 1, 3, 4, 5 };
    for ( int var = 0; var < static_cast<int>(G4_INDICES.size()); ++var ) {
      int idx = G4_INDICES.at(var);
      if ( idx < static_cast<int>(weights.size()) ) {
        const std::string key = "g4_multisim_" + std::to_string(var);
        auto& vec = wmap[key];
        vec = weights.at(idx);
      }
    }
  }
}

// Helper function to set branch addresses for the output TTree.
// Set is_mc=false for data files to suppress MC-only branches (weights,
// neutrino truth, backtracked, and MC daughter branches).
void set_event_output_branch_addresses(TTree& out_tree, AnalysisEvent& ev,
  bool create = false, bool is_mc = true)
{
  // Signal definition flags
  set_output_branch_address( out_tree, "is_mc", &ev.is_mc_, create, "is_mc/O" );

  if ( is_mc ) {
  // Event weights
  set_output_branch_address( out_tree, "spline_weight",
    &ev.spline_weight_, create, "spline_weight/F" );

  set_output_branch_address( out_tree, "tuned_cv_weight",
    &ev.tuned_cv_weight_, create, "tuned_cv_weight/F" );

  set_output_branch_address( out_tree, "ppfx_cv_weight",
    &ev.ppfx_cv_weight_, create, "ppfx_cv_weight/F" );

  if (useNuMI) {
    set_output_branch_address( out_tree, "normalisation_weight",
      &ev.normalisation_weight_, create, "normalisation_weight/F" );
  }

  // If MC weights are available, prepare to store them in the output TTree
  if ( ev.mc_weights_map_ ) {

    // Make separate branches for the various sets of systematic variation
    // weights in the map
    for ( auto& pair : *ev.mc_weights_map_ ) {

      // skip duplicate unecessary weights in NuMI files
      if (useNuMI) {
        // BNB flux weights
        if (pair.first == "flux_all") continue;

        // extra PPFX weights, use multi-sim instead
        if (pair.first == "ppfx_mippk_PPFXMIPPKaon" ||
    		  pair.first == "ppfx_mipppi_PPFXMIPPPion" ||
    		  pair.first == "ppfx_other_PPFXOther" ||
    		  pair.first == "ppfx_targatt_PPFXTargAtten" ||
    		  pair.first == "ppfx_think_PPFXThinKaon" ||
    		  pair.first == "ppfx_thinmes_PPFXThinMeson" ||
    		  pair.first == "ppfx_thinn_PPFXThinNuc" ||
    		  pair.first == "ppfx_thinna_PPFXThinNucA" ||
    		  pair.first == "ppfx_thinnpi_PPFXThinNeutronPion" ||
    		  pair.first == "ppfx_thinpi_PPFXThinPion" ||
    		  pair.first == "ppfx_totabs_PPFXTotAbsorp"
    		) continue;
      }

      // Prepend "weight_" to the name of the vector of weights in the map
      std::string weight_branch_name = "weight_" + pair.first;

      // Store a pointer to the vector of weights (needed to set the branch
      // address properly) in the temporary map of pointers
      ev.mc_weights_ptr_map_[ weight_branch_name ] = &pair.second;

      // Set the branch address for this vector of weights
      set_object_output_branch_address< std::vector<double> >( out_tree,
        weight_branch_name, ev.mc_weights_ptr_map_.at(weight_branch_name),
        create );
    }
  }
  } // end if (is_mc) [weights]

  // Backtracked neutrino purity and completeness
  set_output_branch_address( out_tree, "nu_completeness_from_pfp",
    &ev.nu_completeness_from_pfp_, create, "nu_completeness_from_pfp/F" );

  set_output_branch_address( out_tree, "nu_purity_from_pfp",
    &ev.nu_purity_from_pfp_, create, "nu_purity_from_pfp/F" );

  // Number of neutrino slices identified by the SliceID
  set_output_branch_address( out_tree, "nslice", &ev.nslice_, create,
    "nslice/I" );

  // *** Branches copied directly from the input ***

  // Cosmic rejection parameters for numu CC inclusive selection
  set_output_branch_address( out_tree, "topological_score",
    &ev.topological_score_, create, "topological_score/F" );

  set_output_branch_address( out_tree, "CosmicIP",
    &ev.cosmic_impact_parameter_, create, "CosmicIP/F" );

  // contained fraction
  set_output_branch_address( out_tree, "contained_fraction",
    &ev.contained_fraction_, create, "contained_fraction/F" );

  // Reconstructed neutrino vertex position
  set_output_branch_address( out_tree, "reco_nu_vtx_sce_x",
    &ev.nu_vx_, create, "reco_nu_vtx_sce_x/F" );

  set_output_branch_address( out_tree, "reco_nu_vtx_sce_y",
    &ev.nu_vy_, create, "reco_nu_vtx_sce_y/F" );

  set_output_branch_address( out_tree, "reco_nu_vtx_sce_z",
    &ev.nu_vz_, create, "reco_nu_vtx_sce_z/F" );

  if ( is_mc ) {
  // MC truth information for the neutrino
  set_output_branch_address( out_tree, "mc_nu_pdg", &ev.mc_nu_pdg_,
    create, "mc_nu_pdg/I" );

  set_output_branch_address( out_tree, "mc_nu_vtx_x", &ev.mc_nu_vx_,
    create, "mc_nu_vtx_x/F" );

  set_output_branch_address( out_tree, "mc_nu_vtx_y", &ev.mc_nu_vy_,
    create, "mc_nu_vtx_y/F" );

  set_output_branch_address( out_tree, "mc_nu_vtx_z", &ev.mc_nu_vz_,
    create, "mc_nu_vtx_z/F" );

  set_output_branch_address( out_tree, "mc_nu_energy", &ev.mc_nu_energy_,
    create, "mc_nu_energy/F" );

  set_output_branch_address( out_tree, "mc_ccnc", &ev.mc_nu_ccnc_,
    create, "mc_ccnc/I" );

  set_output_branch_address( out_tree, "mc_interaction",
    &ev.mc_nu_interaction_type_, create, "mc_interaction/I" );
  } // end if (is_mc) [neutrino truth]

  // PFParticle properties
  set_object_output_branch_address< std::vector<unsigned int> >( out_tree,
    "pfp_generation_v", ev.pfp_generation_, create );

  set_object_output_branch_address< std::vector<unsigned int> >( out_tree,
    "pfp_trk_daughters_v", ev.pfp_trk_daughters_count_, create );

  set_object_output_branch_address< std::vector<unsigned int> >( out_tree,
    "pfp_shr_daughters_v", ev.pfp_shr_daughters_count_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_score_v", ev.pfp_track_score_, create );

  set_object_output_branch_address< std::vector<int> >( out_tree,
    "pfpdg", ev.pfp_reco_pdg_, create );

  set_object_output_branch_address< std::vector<int> >( out_tree,
    "pfnhits", ev.pfp_hits_, create );

  set_object_output_branch_address< std::vector<int> >( out_tree,
    "pfnplanehits_U", ev.pfp_hitsU_, create );

  set_object_output_branch_address< std::vector<int> >( out_tree,
    "pfnplanehits_V", ev.pfp_hitsV_, create );

  set_object_output_branch_address< std::vector<int> >( out_tree,
    "pfnplanehits_Y", ev.pfp_hitsY_, create );

  if ( is_mc ) {
  // Backtracked PFParticle properties
  set_object_output_branch_address< std::vector<int> >( out_tree,
    "backtracked_pdg", ev.pfp_true_pdg_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "backtracked_e", ev.pfp_true_E_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "backtracked_px", ev.pfp_true_px_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "backtracked_py", ev.pfp_true_py_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "backtracked_pz", ev.pfp_true_pz_, create );
  } // end if (is_mc) [backtracked]

  // Shower properties
  // For some ntuples, reconstructed shower information is excluded.
  // In such cases, skip writing these branches to the output TTree.
  if ( ev.shower_startx_ ) {
    set_object_output_branch_address< std::vector<float> >( out_tree,
      "shr_start_x_v", ev.shower_startx_, create );

    set_object_output_branch_address< std::vector<float> >( out_tree,
      "shr_start_y_v", ev.shower_starty_, create );

    set_object_output_branch_address< std::vector<float> >( out_tree,
      "shr_start_z_v", ev.shower_startz_, create );

    // Shower start distance from reco neutrino vertex (pre-calculated for
    // convenience)
    set_object_output_branch_address< std::vector<float> >( out_tree,
      "shr_dist_v", ev.shower_start_distance_, create );
  }
  // primary shower
  set_output_branch_address( out_tree, "shr_id", &ev.shr_id_, create, "shr_id/I" );
  set_output_branch_address( out_tree, "shr_score", &ev.shr_score_, create, "shr_score/F" );
  set_output_branch_address( out_tree, "shr_energy_cali", &ev.shr_energy_cali_, create, "shr_energy_cali/F" );
  set_output_branch_address( out_tree, "hits_ratio", &ev.hits_ratio_, create, "hits_ratio/F" );
  set_output_branch_address( out_tree, "shrmoliereavg", &ev.shrmoliereavg_, create, "shrmoliereavg/F" );
  set_output_branch_address( out_tree, "shr_distance", &ev.shr_distance_, create, "shr_distance/F" );
  set_output_branch_address( out_tree, "shr_tkfit_gap10_dedx_Y", &ev.shr_tkfit_gap10_dedx_Y_, create, "shr_tkfit_gap10_dedx_Y/F" );
  set_output_branch_address( out_tree, "shr_tkfit_2cm_dedx_Y", &ev.shr_tkfit_2cm_dedx_Y_, create, "shr_tkfit_2cm_dedx_Y/F" );

  // Track properties
  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_len_v", ev.track_length_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_sce_start_x_v", ev.track_startx_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_sce_start_y_v", ev.track_starty_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_sce_start_z_v", ev.track_startz_, create );

  // Track start distance from reco neutrino vertex (pre-calculated for
  // convenience)
  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_distance_v", ev.track_start_distance_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_sce_end_x_v", ev.track_endx_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_sce_end_y_v", ev.track_endy_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_sce_end_z_v", ev.track_endz_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_dir_x_v", ev.track_dirx_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_dir_y_v", ev.track_diry_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_dir_z_v", ev.track_dirz_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_energy_proton_v", ev.track_kinetic_energy_p_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_range_muon_mom_v", ev.track_range_mom_mu_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_mcs_muon_mom_v", ev.track_mcs_mom_mu_, create );

  // Some ntuples exclude the old chi^2 proton PID score. Only include it in
  // the output if it is available.
  if ( ev.track_chi2_proton_ ) {
    set_object_output_branch_address< std::vector<float> >( out_tree,
      "trk_pid_chipr_v", ev.track_chi2_proton_, create );
  }

  // Log-likelihood-based particle ID information
  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_llr_pid_v", ev.track_llr_pid_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_llr_pid_u_v", ev.track_llr_pid_U_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_llr_pid_v_v", ev.track_llr_pid_V_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_llr_pid_y_v", ev.track_llr_pid_Y_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree,
    "trk_llr_pid_score_v", ev.track_llr_pid_score_, create );

  if ( is_mc ) {
  // MC truth information for the final-state primary particles
  set_object_output_branch_address< std::vector<int> >( out_tree, "mc_pdg",
    ev.mc_nu_daughter_pdg_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree, "mc_E",
    ev.mc_nu_daughter_energy_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree, "mc_px",
    ev.mc_nu_daughter_px_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree, "mc_py",
    ev.mc_nu_daughter_py_, create );

  set_object_output_branch_address< std::vector<float> >( out_tree, "mc_pz",
    ev.mc_nu_daughter_pz_, create );
  } // end if (is_mc) [MC daughters]
}
