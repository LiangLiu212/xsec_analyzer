/*
 * =====================================================================================
 *
 *       Filename:  test.C
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/16/24 14:16:27
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

int main(){
  MakeConfig mm;
  mm.BinScheme();
  mm.Print();
  return 0;
}


