/*
 * =====================================================================================
 *
 *       Filename:  make_config.cxx
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/15/24 14:13:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Liang Liu (L. Liu), liangliu@fnal.gov
 *		    Fermi National Accelerator Laboratory
 *  Collaboration:  GENIE
 *
 * =====================================================================================
 */


#ifndef MAKE_CONFIG_H
#define MAKE_CONFIG_H
#include "HistUtils.hh"
#include <stdexcept>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include "TLine.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TParameter.h"
#include "FilePropertiesManager.hh"



//#include "SliceBinning.hh"
//#include "UniverseMaker.hh"
//#include "HistUtils.hh"
//#include "ConfigMakerUtils.hh"

// BinType (int)
// string branchexpr
// string branchexpr
// std::map< >
// selection
// branch unit
// textbranch unit
//

#include "TNamed.h"

class Block: public TNamed{
  public:
    Block(const char *name, const char *title, const int i):fName(name), fTitle(title),
    binType(i)
  {  }
    ~Block(){}
    virtual Int_t GetNBinsX() = 0;
    virtual Int_t GetNBinsY(const int idx) = 0;
    virtual TString GetBinDef(const int idx)  = 0;
    virtual TString GetBinDef(const int x, const int y)  = 0;
    virtual Int_t GetBinType() = 0;
    virtual Double_t GetBinXLow(const int i) = 0;
    virtual Double_t GetBinXHigh(const int i) = 0;

    virtual TString GetXName() = 0;
    virtual TString GetXNameUnit() = 0;
    virtual TString GetXTitle() = 0;
    virtual TString GetXTitleUnit() = 0;
    virtual TString GetXTexTitle() = 0;
    virtual TString GetXTexTitleUnit() = 0;
    virtual TString GetYName() = 0;
    virtual TString GetYNameUnit()  = 0;
    virtual TString GetYTitle() = 0;
    virtual TString GetYTitleUnit() = 0;
    virtual TString GetYTexTitle() = 0;
    virtual TString GetYTexTitleUnit() = 0;
    virtual TString GetSelection() = 0;



    virtual std::vector<double> GetVector() = 0;
    virtual std::vector<double> GetVector(const int i) = 0;
    virtual std::map<double, std::vector<double>> GetMap() = 0;
    virtual bool Is1D() = 0;
    virtual bool Is2D() = 0;

  protected:
    TString fName;
    TString fTitle;
    Int_t binType;
    std::vector<TString> binDef;
    TString xName;
    TString xNameUnit;
    TString xTitle;
    TString xTitleUnit;
    TString xTexTitle;
    TString xTexTitleUnit;
    TString yName;
    TString yNameUnit;
    TString yTitle;
    TString yTitleUnit;
    TString yTexTitle;
    TString yTexTitleUnit;
   
};


class Block1D: public Block{
  public:
    Block1D(const char *name, const char *title,
        std::vector<double> block1d, const char *selection, const int i): Block(name, title, i), fName(name), fTitle(title),
    fblock(block1d), fselection(selection), binType(i)
  { Init(); }

    Block1D(const char *name, const char *title, const char *textitle,
        std::vector<double> block1d, const char *selection, const int i): Block(name, title, i), fName(name), fTitle(title), fTexTitle(textitle),
    fblock(block1d), fselection(selection), binType(i)
  { Init(); }



    ~Block1D(){}
    Int_t GetNBinsX(){return binDef.size();}
    Int_t GetNBinsY(const int idx) {return 0;}
    TString GetBinDef(const int idx){return binDef[idx];}
    TString GetBinDef(const int x, const int y) {}
    Int_t GetBinType(){return int(binType);}

    Double_t GetBinXLow(const int i) {return -9999999;}
    Double_t GetBinXHigh(const int i) {return -9999999;}

    TString GetXName() {return xName;}
    TString GetXNameUnit() {return xNameUnit;}
    TString GetXTitle() {return xTitle;}
    TString GetXTitleUnit() {return xTitleUnit;}
    TString GetXTexTitle() {return xTexTitle;}
    TString GetXTexTitleUnit() {return xTexTitleUnit;}
    TString GetYName() {return yName;}
    TString GetYNameUnit() {return yNameUnit;}
    TString GetYTitle() {return yTitle;}
    TString GetYTitleUnit() {return yTitleUnit;}
    TString GetYTexTitle() {return yTexTitle;}
    TString GetYTexTitleUnit() {return yTexTitleUnit;}

    TString GetSelection() {return fselection;}

    std::vector<double> GetVector() {return fblock;}
    std::map<double, std::vector<double>> GetMap() {}
    std::vector<double> GetVector(const int i) {}
    bool Is1D() {return true;}
    bool Is2D() {return false;}


  protected:

    TString fName;
    TString fTitle;
    TString fTexTitle;
    std::vector<double> fblock;
    TString fselection;
    Int_t binType;

    TString xName;
    TString xNameUnit;
    TString xTitle;
    TString xTitleUnit;
    TString xTexTitle;
    TString xTexTitleUnit;

    TString yName;
    TString yNameUnit;
    TString yTitle;
    TString yTitleUnit;
    TString yTexTitle;
    TString yTexTitleUnit;

    std::vector<TString> binDef;

  private:
    void Init();
    void SetTitle(const char *title);
    void SetTexTitle(const char *title);
    void SetName(const char *name);
};



class Block2D: public Block{
  public:
    Block2D(const char *name, const char *title,
        std::map< double, std::vector<double> > block2d, TString selection, int i):Block(name, title, i),
    fName(name), fTitle(title), fblock(block2d), fselection(selection), binType(i)
  {Init();}

    Block2D(const char *name, const char *title, const char *textitle,
        std::map< double, std::vector<double> > block2d, TString selection, int i):Block(name, title, i),
    fName(name), fTitle(title), fTexTitle(textitle), fblock(block2d), fselection(selection), binType(i)
  {Init();}

    ~Block2D(){}
    Int_t GetNBinsX(){return fblock_vv_.size();}
    Int_t GetNBinsY(const int idx) {return (fblock_vv_[idx].size() - 1);}
    TString GetBinDef(const int x, const int y){
      int index = 0;
      for(auto i = 0; i < x; i++){
        index += (fblock_vv_[i].size() - 1);
      }
      index += y;
      return GetBinDef(index);
    }
    TString GetBinDef(const int idx){return binDef[idx];}
    Int_t GetBinType(){return int(binType);}

    Double_t GetBinXLow(const int i) {return xbin[i];}
    Double_t GetBinXHigh(const int i) {return xbin[i+1];}

    TString GetXName() {return xName;}
    TString GetXNameUnit() {return xNameUnit;}
    TString GetXTitle() {return xTitle;}
    TString GetXTitleUnit() {return xTitleUnit;}
    TString GetXTexTitle() {return xTexTitle;}
    TString GetXTexTitleUnit() {return xTexTitleUnit;}
    TString GetYName() {return yName;}
    TString GetYNameUnit() {return yNameUnit;}
    TString GetYTitle() {return yTitle;}
    TString GetYTitleUnit() {return yTitleUnit;}
    TString GetYTexTitle() {return yTexTitle;}
    TString GetYTexTitleUnit() {return yTexTitleUnit;}
    TString GetSelection() {return fselection;}

    std::vector<double> GetVector() { }
    std::vector<double> GetVector(const int i) {return fblock_vv_[i]; }
    std::map<double, std::vector<double>> GetMap() {return fblock;}
    bool Is1D() {return false;}
    bool Is2D() {return true;}

  private:


    TString fName;
    TString fTitle;
    TString fTexTitle;
    std::map< double, std::vector<double> >  fblock;
    std::vector<std::vector<double>> fblock_vv_;
    std::vector<double> xbin;
    TString fselection;
    Int_t binType;

    TString xName;
    TString xNameUnit;
    TString xTitle;
    TString xTitleUnit;
    TString xTexTitle;
    TString xTexTitleUnit;

    TString yName;
    TString yNameUnit;
    TString yTitle;
    TString yTitleUnit;
    TString yTexTitle;
    TString yTexTitleUnit;

    std::vector<TString> binDef;

  private:
    void Init();
    void SetTitle(const char *title);
    void SetTexTitle(const char *title);
    void SetName(const char *name);
};


class BlockTrueReco{
  public:
    BlockTrueReco(Block *btrue, Block *breco): block_true_(btrue), block_reco_(breco){}
    ~BlockTrueReco(){}
    Block *block_true_;
    Block *block_reco_;
};


class MakeConfig{
  public:
    MakeConfig();
    ~MakeConfig();

    void BinScheme();
    void Print();
    void ResPlots();

  private:

    std::vector<BlockTrueReco> vect_block;
    TString TREE;
    TString DIRECTORY;
    TString CATEGORY;
    std::vector<int> background_index;
// The anticipated POT to use when scaling the MC prediction in the expected
// reco events plot. This will help ensure that all choices of reco binning
// are informed by the expected statistical uncertainties when the full dataset
// is analyzed.
static constexpr double EXPECTED_POT = 6.790e20; // Full dataset for Runs 1-3

// Number of true bins to use when plotting true distributions in a given
// reco bin
static constexpr int DEFAULT_TRUE_BINS = 100;

// ROOT integer code for Arial font
static constexpr int FONT_STYLE = 62; // Arial

// When bins have zero content, set them to this very small value so that the
// colz style will still paint them
static constexpr double REALLY_SMALL = 1e-11;

// Dummy counter used to ensure that each histogram generated by this
// function has a unique name to use with TTree::Draw()
static int hist_count;

    std::set<int> RUNS;

    void make_res_plots( const std::string& branchexpr,
        const std::string& variable_title, const std::string& selection,
        const std::set<int>& runs, std::vector<double> bin_low_edges,
        bool show_bin_plots = true,
        bool show_smear_numbers = false,
        int num_true_bins = DEFAULT_TRUE_BINS,
        const std::string& mc_branchexpr = "",
        const std::string& signal_cuts = "mc_is_signal",
        const std::string& mc_event_weight = DEFAULT_MC_EVENT_WEIGHT );


    // Overloaded version that uses a fixed number of equal-width bins
    void make_res_plots( const std::string& branchexpr,
        const std::string& variable_title, const std::string& selection,
        const std::set<int>& runs,
        double xmin, double xmax, int Nbins,
        bool show_bin_plots = true,
        bool show_smear_numbers = false,
        int num_true_bins = DEFAULT_TRUE_BINS,
        const std::string& mc_branchexpr = "",
        const std::string& signal_cuts = "mc_is_signal",
        const std::string& mc_event_weight = DEFAULT_MC_EVENT_WEIGHT );

    void make_res_plots( const std::string& rmm_config_file_name,
        const std::set<int>& runs,
        const std::string& universe_branch_name = "TunedCentralValue_UBGenie",
        size_t universe_index = 0u,
        bool show_smear_numbers = false );

};


#endif

