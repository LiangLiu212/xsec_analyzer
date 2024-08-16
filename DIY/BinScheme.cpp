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


  vect_block.clear();

  // proton momentum
  std::vector<double> block1d = {0., 0.06, 0.12, 0.18, 0.24, 0.32, 0.4, 0.48, 0.55, 0.68, 0.75, 0.9};
  Block1D *b1dt = new Block1D("branchexpr_t ; GeV_c", "proton mom; GeV/c", block1d, "is_signal", 1);
  Block1D *b1dr = new Block1D("branchexpr_r ; GeV_c", "proton mom; GeV/c", block1d, "is_signal", 1);
  vect_block.emplace_back(b1dt, b1dr);

  // muon momentum
  std::vector<double> block1dm = {0., 0.06, 0.12, 0.18, 0.24, 0.32, 0.4, 0.48, 0.55, 0.68, 0.75, 0.9};
  Block1D *b1dmt = new Block1D("branchexpr_t ; GeV_c", "proton mom; GeV/c", block1dm, "is_signal", 1);
  Block1D *b1dmr = new Block1D("branchexpr_r ; GeV_c", "proton mom; GeV/c", block1dm, "is_signal", 1);
  vect_block.emplace_back(b1dmt, b1dmr);


  // 2d proton mom : proton cos theta
  std::map< double, std::vector<double> > block2d = {
    { 0.0, { 0., 25., 60., 95., 120., 145., 165., 180. } },
    { 0.2, { 0., 25., 60., 95., 120., 145., 165., 180. } },
    { 0.3, { 0., 25., 60., 95., 120., 145., 165., 180. } },
    { 0.4, { 0., 25., 60., 95., 120., 145., 165., 180. } },
    {0.5, {}}
  };
  Block2D *b2dt = new Block2D("a1; a2", "mom;GeV; costheta; ", block2d, "selected", 1);
  Block2D *b2dr = new Block2D("a1; a2", "mom;GeV; costheta; ", block2d, "selected", 1);
  vect_block.emplace_back(b2dt, b2dr);

}


