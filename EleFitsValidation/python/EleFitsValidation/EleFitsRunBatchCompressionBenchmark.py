# Copyright (C) 2019-2023, CNES and contributors (for the Euclid Science Ground Segment)
# This file is part of EleFits <github.com/CNES/EleFits>
# SPDX-License-Identifier: LGPL-3.0-or-later

import argparse
# from collections import defaultdict
import csv
import subprocess
# import matplotlib.pyplot as plt
# from pandas import DataFrame
import ElementsKernel.Logging as log


def scientific_notation(value):
    b, e = f'{float(value):.2e}'.split('e')
    if float(b) % 1 == 0:
        b = int(float(b))
    return f'{b}e{int(e)}'


def str2bool(v):
    return v.lower() in ("yes", "true", "t", "1")


def make_command(test_case, output, results, log_level):
    """Run a test case specified as a dictionary with following keys:
    "Filename", "Comptype"
    "Test setup", "HDU type", "HDU count", "Value count / HDU"
    """
    # FIXME: add tiling
    cmd = f'EleFitsRunCompressionBenchmark "{test_case["Filename"]}" {output}'
    cmd += f' --case "{test_case["Comptype"]}"'
    cmd += f' --res {results} --log-level {log_level}'
    if str2bool(test_case["Lossy"]):
        cmd += f' --lossy'
    if str2bool(test_case["ExtGZIP"]):
        cmd += f' --extGZIP'
    return cmd


def defineSpecificProgramOptions():
    parser = argparse.ArgumentParser()
    parser.add_argument('--tests', default=None,
                        help='The set of test configurations as a TSV file '
                        '(columns are: "Filename", "Comptype", "Lossy", "ExtGZIP). '
                        'The result file can be used to rerun the same benchmark.')
    parser.add_argument('--output', default='/tmp/test.fits', help='The filename of the FITS files to be generated.')
    parser.add_argument('--res', default='/tmp/compressionBenchmark.csv',
                        help='The filename of the benchmark results TSV.')
    # parser.add_argument('--plot', default='/tmp/plot.eps', help='The filename of the benchmark results plotting file.')
    return parser


def mainMethod(args):

    logger = log.getLogger('EleFitsRunBatchCompressionBenchmark')

    if args.tests is not None:
        with open(args.tests, 'r') as f:
            for test_case in csv.DictReader(f, delimiter='\t'):
                logger.debug(test_case)
                cmd = make_command(test_case, args.output, args.res, 'INFO')  # TODO get log level from args
                logger.info('')
                logger.info(cmd)
                logger.info('')
                subprocess.call(cmd, shell=True)
