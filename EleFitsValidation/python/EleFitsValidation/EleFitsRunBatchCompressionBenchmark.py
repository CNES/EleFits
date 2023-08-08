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


def scientificNotation(value):
    b, e = f'{float(value):.2e}'.split('e')
    if float(b) % 1 == 0:
        b = int(float(b))
    return f'{b}e{int(e)}'

def str2bool(v):
  return v.lower() in ("yes", "true", "t", "1")

def makeCommand(testCase, output, results, log_level):
    """Run a test case specified as a dictionary with following keys:
    "Filename", "Comptype"
    "Test setup", "HDU type", "HDU count", "Value count / HDU"
    """
    # FIXME: add tiling
    cmd = f'EleFitsRunCompressionBenchmark "{testCase["Filename"]}" {output}'
    cmd += f' --case "{testCase["Comptype"]}"'
    cmd += f' --res {results} --log-level {log_level}'
    if str2bool(testCase["Lossy"]):
        cmd += f' --lossy'
    if str2bool(testCase["ExtGZIP"]):
        cmd += f' --extGZIP'
    return cmd


def defineSpecificProgramOptions():
    parser = argparse.ArgumentParser()
    parser.add_argument('--tests', default=None,
                        help='The set of test configurations as a TSV file '
                        '(columns are: "Filename", "Comptype", "Lossy", "ExtGZIP). '
                        'The result file can be used to rerun the same benchmark.')
    parser.add_argument('--output', default='/tmp/test.fits', help='The filename of the FITS files to be generated.')
    parser.add_argument('--res', default='/tmp/compressionBenchmark.csv', help='The filename of the benchmark results TSV.')
    #parser.add_argument('--plot', default='/tmp/plot.eps', help='The filename of the benchmark results plotting file.')
    return parser


def mainMethod(args):

    logger = log.getLogger('EleFitsRunBatchCompressionBenchmark')

    if args.tests is not None:
        with open(args.tests, 'r') as f:
            for testCase in csv.DictReader(f, delimiter='\t'):
                logger.debug(testCase)
                cmd = makeCommand(testCase, args.output, args.res, 'INFO')  # TODO get log level from args
                logger.info('')
                logger.info(cmd)
                logger.info('')
                subprocess.call(cmd, shell=True)

    # if args.plot is not None:
    #     results = defaultdict(DataFrame)
    #     with open(args.res, 'r') as f:
    #         for testCase in csv.DictReader(f, delimiter='\t'):
    #             hduType = testCase["HDU type"]
    #             valueCount = int(testCase["Value count / HDU"])
    #             if hduType == 'Image':
    #                 shape = f'{scientificNotation(valueCount)} pixels'
    #             else:
    #                 shape = f'10 columns x {scientificNotation(valueCount/10)} rows'
    #             testName = f'{testCase["Mode"]} {hduType}\n({testCase["HDU count"]} HDUs x {shape})'
    #             testCaseSetup = testCase['Test setup']
    #             data = [float(s) for s in testCase["Samples (ms)"].split(',')]
    #             logger.debug(testName.replace('\n', ' ') + ' ' + testCaseSetup + ': ' + str(data))
    #             results[testName][testCaseSetup] = data
    #     cols = 2
    #     rows = (len(results) + cols - 1) // cols  # Equivalent to ceil(len(results)/2) for ints
    #     logger.info(f'Plotting {len(results)} graphs ({cols} columns x {rows} rows)...')
    #     _, axes = plt.subplots(ncols=cols, nrows=rows, figsize=(5 * cols, 3 * rows))
    #     # FIXME Cannot sharex, sharey anymore in EDEN3
    #     # plt.subplots(ncols=cols, nrows=rows, figsize=(5 * cols, 3 * rows), sharex='col', sharey='row')
    #     # See bug with pandas+matplotlib:
    #     # https://github.com/pandas-dev/pandas/issues/36918
    #     i = False  # left column, since cols = 2
    #     j = 0
    #     for k, v in results.items():
    #         logger.info(f'Ploting ({j}, {int(i)}): ' + k.replace('\n', ' '))
    #         if cols == 1:
    #             ax = axes[j]
    #         elif rows == 1:
    #             ax = axes[int(i)]
    #         else:
    #             ax = axes[j, int(i)]
    #         ax.set_title(k)
    #         v.boxplot(vert=False, ax=ax, whis=(5, 95), showfliers=False)
    #         # plots 5th, 25th, 50th, 75th, 95th percentiles without outliers
    #         if j == rows - 1:
    #             ax.set_xlabel('Time (ms)')
    #         j += int(i)
    #         i = not i
    #     plt.tight_layout()  # Avoids overlapping texts
    #     plt.savefig(args.plot, bbox_inches='tight')  # 'tight' avoids having text outside the box
    #     logger.info(f'Saved plot as: {args.plot}')