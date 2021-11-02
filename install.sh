#!/bin/bash
#
# Copyright (C) 2012-2020 Euclid Science Ground Segment
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3.0 of the License, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#

# Adapted from bash script template from https://github.com/leogtzr/minimal-safe-bash-template/blob/main/template-v1.sh

set -o errexit
set -o nounset
set -o pipefail

readonly script_name="${0##*/}"
readonly error_parsing_options=81

trap clean_up ERR EXIT SIGINT SIGTERM

# By default build in /tmp
build_dir="/tmp/build_elefits"
parallel_build=1

build_dir_option_flag=0
j_option_flag=0

usage() {
    cat <<USAGE_TEXT
Usage: ${script_name} [-h | --help] [-d | --build_dir] [-j | --parallel]
DESCRIPTION
    Installation script for Elefits (downloads, builds and installs Elements first, then EleFits)
    OPTIONS:
    -h, --help
        Print this help and exit.
    -d, --build_dir
        Build directory (/tmp/build_elefits by default).
    -j, --parallel
        Specifies the number of jobs to run simultaneously.
USAGE_TEXT
}

clean_up() {
    trap - ERR EXIT SIGINT SIGTERM
    # Remove temporary build directories
    echo "Remove ${build_dir}..."
    if [ -d "${build_dir}" ]; then rm -Rf ${build_dir}; fi

}

parse_user_options() {
    local -r args=("${@}")
    local opts

    # The following code works perfectly for 
    opts=$(getopt --options d:,h,j: --long help:,build_dir,parallel -- "${args[@]}" 2> /dev/null) || {
        usage
        die "error: parsing options" "${error_parsing_options}"
    }

    eval set -- "${opts}"

    while true; do
    case "${1}" in

        --help|-h)
            usage

            exit 0
            shift
            ;;

        -d|--build_dir)
            build_dir_option_flag=1
            readonly d_arg="${2}"
            shift
            shift
            ;;
        -j|--parallel)
            j_option_flag=1
            readonly j_arg="${2}"
            echo "parallel build with nb_jobs= ${2}"
            shift
            shift
            ;;

        --)
            shift
            break
            ;;
        *)
            break
            ;;
    esac
    done
}

parse_user_options "${@}"

if ((build_dir_option_flag)); then
    echo "Using --build_dir option -> arg: [${d_arg}]"
    build_dir="${d_arg}"
fi

if ((j_option_flag)); then
    echo "Using -j option -> arg: [${j_arg}]"
    parallel_build="${j_arg}"
fi

mkdir -p "${build_dir}"
cd "${build_dir}"


cd "${build_dir}"
git clone https://github.com/astrorama/Elements.git
cd Elements
git checkout 5.14.0
mkdir build ; cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
make -j "${parallel_build}"
make install


cd "${build_dir}"
git clone -b release-4.0 https://github.com/CNES/EleFits.git
cd EleFits
mkdir build ; cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_PREFIX_PATH=/usr/local ..
make -j "${parallel_build}"
make install

exit 0
