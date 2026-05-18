# source setup_stv.sh

PROCESSED_NTUPLE_DIR="/exp/uboone/app/users/dinupa/test/dev_v26_04_24/xsec_analyzer/data"
UNIV_OUTPUT_FILE=${PROCESSED_NTUPLE_DIR}"/Universes.root"

MEASUREMENT_OUTPUT_FILE="/exp/uboone/app/users/dinupa/test/dev_v26_04_24/xsec_analyzer/outputs"
UNF_MEAS_OUTPUT_FILE=${MEASUREMENT_OUTPUT_FILE}"/UnfoldedCrossSection.root"

NTUPLE_CONFIG="./configs/files_to_process_nc1p.txt"
FPM_CONFIG="./configs/file_properties_nc1p.txt"
BIN_CONFIG="./configs/nc1p_bin_config.txt"
XSEC_CONFIG="./configs/xsec_config.txt"
SLICE_CONFIG="./configs/nc1p_slice_config.txt"
SYST_CONFIG="./configs/systcalc.conf"

# mkdir -p "${MEASUREMENT_OUTPUT_FILE}"

./scripts/ReprocessNTuples.sh ${PROCESSED_NTUPLE_DIR} NC1p ${NTUPLE_CONFIG}
# ./scripts/UniverseMaker.sh ${FPM_CONFIG} ${BIN_CONFIG} ${UNIV_OUTPUT_FILE}
# ./scripts/PlotSlices.sh ${FPM_CONFIG} ${SYST_CONFIG} ${SLICE_CONFIG} ${UNIV_OUTPUT_FILE} ${MEASUREMENT_OUTPUT_FILE}
# ./scripts/Unfolder.sh ${XSEC_CONFIG} ${SLICE_CONFIG} ${UNF_MEAS_OUTPUT_FILE}
