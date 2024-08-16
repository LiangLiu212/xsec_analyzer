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
#include <stdexcept>
#include <vector>
#include <map>
#include <iostream>

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
    virtual TString GetYName() = 0;
    virtual TString GetYNameUnit()  = 0;
    virtual TString GetYTitle() = 0;
    virtual TString GetYTitleUnit() = 0;
    virtual std::vector<double> GetVector() = 0;
    virtual std::vector<double> GetVector(const int i) = 0;
    virtual std::map<double, std::vector<double>> GetMap() = 0;

  protected:
    TString fName;
    TString fTitle;
    Int_t binType;
    std::vector<TString> binDef;
    TString xName;
    TString xNameUnit;
    TString xTitle;
    TString xTitleUnit;
    TString yName;
    TString yNameUnit;
    TString yTitle;
    TString yTitleUnit;
};


class Block1D: public Block{
  public:
    Block1D(const char *name, const char *title,
        std::vector<double> block1d, const char *selection, const int i): Block(name, title, i), fName(name), fTitle(title),
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
    TString GetYName() {return yName;}
    TString GetYNameUnit() {return yNameUnit;}
    TString GetYTitle() {return yTitle;}
    TString GetYTitleUnit() {return yTitleUnit;}
    std::vector<double> GetVector() {return fblock;}
    std::map<double, std::vector<double>> GetMap() {}
    std::vector<double> GetVector(const int i) {}


  protected:

    TString fName;
    TString fTitle;
    std::vector<double> fblock;
    TString fselection;
    Int_t binType;

    TString xName;
    TString xNameUnit;
    TString xTitle;
    TString xTitleUnit;

    TString yName;
    TString yNameUnit;
    TString yTitle;
    TString yTitleUnit;

    std::vector<TString> binDef;

  private:
    void Init();
    void SetTitle(const char *title);
    void SetName(const char *name);
};



class Block2D: public Block{
  public:
    Block2D(const char *name, const char *title,
        std::map< double, std::vector<double> > block2d, TString selection, int i):Block(name, title, i),
    fName(name), fTitle(title), fblock(block2d), fselection(selection), binType(i)
  {Init();}
    ~Block2D(){}
    Int_t GetNBinsX(){return fblock_vv_.size();}
    Int_t GetNBinsY(const int idx) {return fblock_vv_[idx].size();}
    TString GetBinDef(const int x, const int y){
      int index = 0;
      for(auto i = 0; i < x; i++){
        index += fblock_vv_[i].size();
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
    TString GetYName() {return yName;}
    TString GetYNameUnit() {return yNameUnit;}
    TString GetYTitle() {return yTitle;}
    TString GetYTitleUnit() {return yTitleUnit;}
    std::vector<double> GetVector() { }
    std::vector<double> GetVector(const int i) {return fblock_vv_[i]; }
    std::map<double, std::vector<double>> GetMap() {return fblock;}

  private:


    TString fName;
    TString fTitle;
    std::map< double, std::vector<double> >  fblock;
    std::vector<std::vector<double>> fblock_vv_;
    std::vector<double> xbin;
    TString fselection;
    Int_t binType;

    TString xName;
    TString xNameUnit;
    TString xTitle;
    TString xTitleUnit;

    TString yName;
    TString yNameUnit;
    TString yTitle;
    TString yTitleUnit;

    std::vector<TString> binDef;

  private:
    void Init();
    void SetTitle(const char *title);
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

  private:
    std::vector<BlockTrueReco> vect_block;
};


#endif

/*
void make_config(){


}

*/
