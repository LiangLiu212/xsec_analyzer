#!/bin/bash

# Sets up the local environment for working with the STV analysis scripts
source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh
setup uboonecode v08_00_00_52 -q e17:prof
#setup uboonecode v08_00_00_78 -q e17:prof
export GXMLPATH=/pnfs/uboone/resilient/users/liangliu/genie_pythia:${GXMLPATH}

# Finds the directory where this script is located. This method isn't
# foolproof. See https://stackoverflow.com/a/246128/4081973 if you need
# something more robust for edge cases (e.g., you're calling the script using
# symlinks).
THIS_DIRECTORY="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export STV_ANALYSIS_DIR=${THIS_DIRECTORY}
export PATH=${PATH}:${STV_ANALYSIS_DIR}/bin
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${STV_ANALYSIS_DIR}/lib
