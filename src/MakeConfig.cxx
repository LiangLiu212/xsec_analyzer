/*
 * =====================================================================================
 *
 *       Filename:  MakeConfig.cxx
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/16/24 13:19:29
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

#include <iostream>
#include <vector>
#include <map>
#include <stdexcept>

#include "SliceBinning.hh"
#include "UniverseMaker.hh"
#include "HistUtils.hh"
#include "ConfigMakerUtils.hh"


using namespace std;

MakeConfig::MakeConfig(){
}
MakeConfig::~MakeConfig(){
  cout << "free make config" << endl;
  for(int i = 0; i < vect_block.size(); i++){
    delete vect_block.at(i).block_true_;
    delete vect_block.at(i).block_reco_;
  }
}


void MakeConfig::Print(){


  SliceBinning sb;
  std::vector< TrueBin > true_bins;
  std::vector< RecoBin > reco_bins;


  for(int i = 0; i < vect_block.size(); i++){

    if(!vect_block.at(i).block_true_->GetYTitle().CompareTo("Events")){

      sb.slice_vars_.emplace_back(vect_block.at(i).block_true_->GetXTitle().Data(),
          "", vect_block.at(i).block_true_->GetXTitleUnit().Data(), "");
      int var_idx = find_slice_var_index(vect_block.at(i).block_true_->GetXTitle().Data(), sb.slice_vars_);

      size_t num_bins = vect_block.at(i).block_true_->GetNBinsX();
      cout << "vector : " << vect_block.at(i).block_true_->GetVector().size() << endl;

      auto& slice = add_slice(sb, vect_block.at(i).block_true_->GetVector(), var_idx);

      for(int j = 0; j < vect_block.at(i).block_true_->GetNBinsX(); j++){
        slice.bin_map_[ j + 1 ].insert( true_bins.size() );
        true_bins.emplace_back( vect_block.at(i).block_true_->GetBinDef(j).Data(), kSignalTrueBin, i);

      }
      for(int j = 0; j < vect_block.at(i).block_reco_->GetNBinsX(); j++){
        reco_bins.emplace_back( vect_block.at(i).block_reco_->GetBinDef(j).Data(), kOrdinaryRecoBin, i);
      }
    }
    else{
      sb.slice_vars_.emplace_back(vect_block.at(i).block_true_->GetXTitle().Data(),
          "", vect_block.at(i).block_true_->GetXTitleUnit().Data(), "");
      sb.slice_vars_.emplace_back(vect_block.at(i).block_true_->GetYTitle().Data(),
          "", vect_block.at(i).block_true_->GetYTitleUnit().Data(), "");


      int xvar_idx = find_slice_var_index(vect_block.at(i).block_true_->GetXTitle().Data(), sb.slice_vars_);
      int yvar_idx = find_slice_var_index(vect_block.at(i).block_true_->GetYTitle().Data(), sb.slice_vars_);

      for(int j = 0; j < vect_block.at(i).block_true_->GetNBinsX(); j++){
        double xlow = vect_block.at(i).block_true_->GetBinXLow(j);
        double xhigh = vect_block.at(i).block_true_->GetBinXHigh(j);
        auto& slice = add_slice(sb, vect_block.at(i).block_true_->GetVector(j),
            xvar_idx, yvar_idx, xlow, xhigh);
        for(int k = 0; k < vect_block.at(i).block_true_->GetNBinsY(j); k++){
          slice.bin_map_[ k + 1 ].insert( true_bins.size() );
          true_bins.emplace_back(vect_block.at(i).block_true_->GetBinDef(j, k).Data(), kSignalTrueBin, i);
          reco_bins.emplace_back(vect_block.at(i).block_reco_->GetBinDef(j, k).Data(), kOrdinaryRecoBin, i);
        }
      }
    }


  }

  for ( const auto& tb : true_bins ) cout  << tb << '\n';
  for ( const auto& tb : reco_bins ) cout  << tb << '\n';

  cout << sb << endl;
}




void Block1D::Init(){
  this->SetTitle(fTitle.Data());
  this->SetName(fName.Data());

  for ( size_t b = 0u; b < fblock.size() - 1; ++b ) {
    double low = fblock.at( b );
    double high = fblock.at( b + 1u );
    TString bin_def = "";
    if(fselection.Length()!=0){
      bin_def = fselection + " && ";
    }
    bin_def += xName + Form(" >= %f && ", low) + xName + Form(" >= %f", high);
    binDef.push_back(bin_def);
  }
}

void Block1D::SetTitle(const char *title){
  fTitle = title;
  fTitle.ReplaceAll("#;",2,"#semicolon",10);
  std::vector<TString> str_container;
  TString str1 = fTitle;
  Int_t isc = str1.Index(";");
  Int_t lns = str1.Length();
  while(isc >=0){
    str_container.push_back(str1(0,isc));
    str1 = str1(isc+1, lns);
    isc = str1.Index(";");
    lns = str1.Length();
  }
  str_container.push_back(str1);
  if(str_container.size() == 2u){
    xTitle = str_container[0];
    xTitleUnit = str_container[1];
    yTitle = "Events";
    yTitleUnit = "";
    str_container.clear();
  }
  else if(str_container.size() == 1u){
    xTitle = str_container[0];
    xTitleUnit = "";
    yTitle = "Events";
    yTitleUnit = "";
    str_container.clear();
  }
  else
    throw std::runtime_error("Wrong title -> " + fTitle + ". The format of the title of 1D block must be <branch title>; <unit>.");

}


void Block1D::SetName(const char *name){
  fName = name;
  fName.ReplaceAll("#;",2,"#semicolon",10);
  std::vector<TString> str_container;
  TString str1 = fName;
  Int_t isc = str1.Index(";");
  Int_t lns = str1.Length();
  while(isc >=0){
    str_container.push_back(str1(0,isc));
    str1 = str1(isc+1, lns);
    isc = str1.Index(";");
    lns = str1.Length();
  }
  str_container.push_back(str1);
  if(str_container.size() == 2u){
    xName = str_container[0];
    xNameUnit = str_container[1];
    yName = "Events";
    yNameUnit = "";
    str_container.clear();
  }
  else if(str_container.size() == 1u){
    xName = str_container[0];
    xNameUnit = "";
    yName = "Events";
    yNameUnit = "";
    str_container.clear();
  }
  else
    throw std::runtime_error("Wrong name -> " + fName + ". The format of the name of 1D block must be <branch name>; <unit>.");
}

void Block2D::Init(){
  this->SetTitle(fTitle.Data());
  this->SetName(fName.Data());

  for (auto iter = fblock.cbegin(); iter != fblock.cend(); ++iter ) {
    // Get an iterator to the map element after the current one. Due to the
    // automatic sorting, this is guaranteed to contain the upper edge of the
    // current delta_pT bin
    xbin.push_back(iter->first);
    auto next = iter;
    ++next;
    if(next == fblock.cend()) continue;
    double slice_low = iter->first;
    double slice_high = next->first;

    fblock_vv_.push_back(iter->second);

    for(size_t b = 0u; b < iter->second.size() - 1; b++){
      double bin_low = iter->second.at(b);
      double bin_high = iter->second.at(b + 1u);
      TString bin_def = "";
      if(fselection.Length()!=0){
        bin_def = fselection + " && ";
      }
      bin_def += xName + Form(" >= %f && ", slice_low) + xName + Form(" >= %f && ", slice_high)
        + yName + Form(" >= %f && ", bin_low) + yName + Form(" >= %f ", bin_high);
      binDef.push_back(bin_def);
    }
  }
}


void Block2D::SetName(const char *name){
  fName = name;
  fName.ReplaceAll("#;",2,"#semicolon",10);

  std::vector<TString> str_container;

  TString str1 = fName;
  Int_t isc = str1.Index(";");
  Int_t lns = str1.Length();
  if(isc < 0)
    throw std::runtime_error("Wrong name -> " + fName + ". The format of the name of 2D block must be <branch name>; <unit>; <y branch name>; <y unit>.");
  while(isc >=0){
    str_container.push_back(str1(0,isc));
    str1 = str1(isc+1, lns);
    isc = str1.Index(";");
    lns = str1.Length();
  }
  str_container.push_back(str1);
  if(str_container.size() == 4u){
    xName = str_container[0];
    xNameUnit = str_container[1];
    yName =  str_container[2];
    yNameUnit =  str_container[3];
    str_container.clear();
  }
  else if(str_container.size() == 2u){
    xName = str_container[0];
    yName =  str_container[1];
    xNameUnit = "";
    yNameUnit = "";
    str_container.clear();
  }
  else
    throw std::runtime_error("Wrong name -> " + fName + ". The format of the name of 2D block must be <branch name>; <unit>; <y branch name>; <y unit>.");
}



void Block2D::SetTitle(const char *title){
  fTitle = title;
  fTitle.ReplaceAll("#;",2,"#semicolon",10);
  std::vector<TString> str_container;

  TString str1 = fTitle;
  Int_t isc = str1.Index(";");
  Int_t lns = str1.Length();
  if(isc < 0)
    throw std::runtime_error("Wrong title -> " + fTitle + ". The format of the title of 2D block must be <branch title>; <unit>; <y branch title>; <y unit>.");
  while(isc >=0){
    str_container.push_back(str1(0,isc));
    str1 = str1(isc+1, lns);
    isc = str1.Index(";");
    lns = str1.Length();
  }
  str_container.push_back(str1);
  if(str_container.size() == 4u){
    xTitle = str_container[0];
    xTitleUnit = str_container[1];
    yTitle =  str_container[2];
    yTitleUnit =  str_container[3];
    str_container.clear();
  }
  else if(str_container.size() == 2u){
    xTitle = str_container[0];
    yTitle =  str_container[1];
    xTitleUnit = "";
    yTitleUnit = "";
    str_container.clear();
  }
  else
    throw std::runtime_error("Wrong title -> " + fTitle + ". The format of the title of 2D block must be <branch title>; <unit>; <y branch title>; <y unit>.");
}




