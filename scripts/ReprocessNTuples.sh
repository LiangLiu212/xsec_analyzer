#!/bin/bash

# Number of expected command-line arguments
num_expected=3

if [ "$#" -ne "$num_expected" ]; then
  echo "Usage: ./ReprocessNTuples.sh OUTPUT_DIRECTORY SELECTION_NAMES NTUPLE_LIST_FILE"
  exit 1
fi

output_dir=$1
selections=$2
ntuple_list_file=$3

if [ ! -f "$ntuple_list_file" ]; then
  echo "Ntuple list file \"${ntuple_list_file}\" not found"
  exit 1
fi

if [ ! -d "${output_dir}" ]; then
  echo "Output directory \"${output_dir}\" not found"
  exit 2
fi

input_files=()
# Loop over each line of the ntuple list file, preserving whole lines as
# single array elements so that multi-column formats (file type run_id) work.
while IFS= read -r line; do
  # Select lines that do not begin with a '#' character and contain at least
  # one non-whitespace character. These are assumed to be input file names.
  if [[ ! $line = \#* ]] && [[ $line = *[^[:space:]]* ]]; then
    input_files+=("${line}")
  fi
done < "${ntuple_list_file}"

# Calculate total number of input files
total_files=${#input_files[@]}
echo "Total number of files = "${total_files}

counter=0
# Loop over each input file
for file in "${input_files[@]}"
do
    input_file_name=$( echo "$file" | awk '{print $1}' )
    input_file_type=$( echo "$file" | awk '{print $2}' )
    # Optional third column: run ID. When present, "NC1p" in the selection
    # name is replaced with "NC1p_<run_id>" so that each file loads the
    # correct run-period BDT weights (e.g. run_id=2 → NC1p_2).
    input_run_id=$( echo "$file" | awk 'NF>=3{print $3}' )
    output_file_name="${output_dir}/xsec-ana-$(basename ${input_file_name})"

    cur_selections=${selections}
    if [ -n "${input_run_id}" ]; then
        cur_selections=$(echo "${selections}" | sed "s/NC1p/NC1p_${input_run_id}/g")
    fi

    echo "Starting file:"${counter}"/"${total_files}
    echo "Input file name: "${input_file_name}
    echo "Input file type: "${input_file_type}
    echo "Run ID: "${input_run_id}
    echo "Selections: "${cur_selections}
    echo "Output file name: "${output_file_name}

    date
    time ProcessNTuples ${input_file_name} ${input_file_type} ${cur_selections} ${output_file_name}
    date
    counter=$((counter + 1))
done
