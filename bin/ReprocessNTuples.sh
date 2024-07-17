#!/bin/bash

# Function to print usage information
usage() {
  echo "Usage: $0 [-i <input config fule>] [-o <out directory>] [-r runnumbers] [-s samples]"
  echo "  -i file        Specify the input config file."
  echo "  -o directory   Specify the output directory."
  echo "  -r runnumbers  Specify runnumbers to filter (comma-separated, e.g., 1,2,3)."
  echo "  -s samples     Specify runnumbers to filter "
  echo "                      run1 : numuMC,nueMC,dirtMC,extBNB,onBNB,openBNB,detVarCV,detVarLYDown,detVarLYRayleigh,detVarLYAttenuation,detVarSEC,detVarRecomb2,detVarModX,detVarModYZ,"
  exit 1
}

config_dir=${STV_ANALYSIS_DIR}/Configs
output_config=file_properties_cthorpe.txt
output_dir=../Output
filter_runnumbers=1,2,3
filter_samples="numuMC,nueMC,dirtMC,extBNB,onBNB,openBNB,detVarCV,detVarLYDown,detVarLYRayleigh,detVarLYAttenuation,detVarSEC,detVarRecomb2,detVarModX,detVarModYZ,detVarThetaModXZ,detVarThetaModYZ,neudetVarCV,neudetVarLYDown,neudetVarLYRayleigh,neudetVarLYAttenuation,neudetVarSEC,neudetVarRecomb2,neudetVarModX,neudetVarModYZ,neudetVarThetaModXZ,neudetVarThetaModYZ,dirtMCa,dirtMCb,numuMC,nueMC,dirtMC,extBNB,onBNB,openBNB,detVarCV,detVarCVExtra,detVarLYDown,detVarLYRayleigh,detVarLYAttenuation,detVarSEC,detVarRecomb2,detVarModX,detVarModYZ,detVarThetaModXZ,detVarThetaModYZ,neudetVarCV,neudetVarLYDown,neudetVarLYRayleigh,neudetVarLYAttenuation,neudetVarSEC,neudetVarRecomb2,neudetVarModX,neudetVarModYZ,neudetVarThetaModXZ,neudetVarThetaModYZ"
# Parse command-line arguments
while getopts ":i:o:r:s:" opt; do
  case ${opt} in
    i )
      input_file=$OPTARG
      ;;
    o )
      output_dir=$OPTARG
      ;;
    r )
      filter_runnumbers=$OPTARG
      ;;
    s )
      filter_samples=$OPTARG
      ;;
    \? )
      usage
      ;;
  esac
done

# Check if input file is provided
if [ -z "$input_file" ]; then
  usage
fi

# Convert comma-separated runnumbers to array
if [ -n "$filter_runnumbers" ]; then
  IFS=',' read -r -a runnumbers <<< "$filter_runnumbers"
fi
# Convert comma-separated runnumbers to array
if [ -n "$filter_runnumbers" ]; then
  IFS=',' read -r -a samples <<< "$filter_samples"
fi

# Function to check if a value is in an array
contains() {
  local array="$1[@]"
  local seeking=$2
  local in=1
  for element in "${!array}"; do
    if [[ $element == "$seeking" ]]; then
      in=0
      break
    fi
  done
  return $in
}

# Read and parse the file




if [ ! -f "$input_file" ]; then
  echo "Ntuple list file \"${input_file}\" not found"
  exit 1
fi

if [ ! -d "${output_dir}" ]; then
  echo "Output directory \"${output_dir}\" not found"
  exit 2
fi

output_dir=`realpath ${output_dir}`

echo "# `date`" > ${config_dir}/${output_config}
while IFS= read -r line; do
  # Skip comments and empty lines
  if [[ "$line" =~ ^# ]] || [[ -z "$line" ]]; then
  echo $line >> ${config_dir}/${output_config}
    continue
  fi
  
  # Split the line into parts
  parts=($line)
  file_name=${parts[0]}
  index=${parts[1]}
  sample_type=${parts[2]}
  num_events=${parts[3]:-}
  scaling_factor=${parts[4]:-}
  
  # Filter by runnumbers if specified
  if [ -n "$filter_runnumbers" ]; then
    if ! contains runnumbers "$index"; then
      continue
    fi
  fi
 
  # Filter by runnumbers if specified
  if [ -n "$filter_runnumbers" ]; then
    if ! contains samples "$sample_type"; then
      continue
    fi
  fi

  echo  -n "$sample_type,"
  # Print the parsed information
  echo "File Name: $file_name"
  echo "Index: $index"
  echo "Sample Type: $sample_type"
  echo "Number of Events: $num_events"
  echo "Scaling Factor: $scaling_factor"
  output_file_name="${output_dir}/stv-$(basename ${file_name})"
  echo "Output file name: ${output_file_name}"
  time ProcessNTuples ${file_name} ${output_file_name}
  echo "${output_file_name} $index $sample_type $num_events $scaling_factor" >> ${config_dir}/${output_config}
  echo "--------------------------------"

done < "$input_file"
echo 
