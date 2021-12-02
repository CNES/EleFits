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

import argparse
from collections import defaultdict
import csv
import subprocess
import matplotlib.pyplot as plt
from pandas import DataFrame
import ElementsKernel.Logging as log


def scientificNotation(value):
    b, e = f'{float(value):.2e}'.split('e')
    if float(b)%1 == 0:
        b = int(float(b))
    return f'{b}e{int(e)}'


def makeCommand(testCase, output, results, log_level):
    """Run a test case specified as a dictionary with following keys:
    "Test setup", "HDU type", "HDU count", "Value count / HDU"
    """
    cmd = f'EleFitsRunBenchmark --log-level {log_level} --output {output} --res {results}'
    cmd += f' --setup "{testCase["Test setup"]}"'
    if testCase['HDU type'] == 'Image':
        cmd += f' --images {int(float(testCase["HDU count"]))} --pixels {int(float(testCase["Value count / HDU"]))}'
        # int(float(value)) allows value to be an integer in scientific notation
    if testCase['HDU type'] == 'Binary table':
        cmd += f' --tables {int(float(testCase["HDU count"]))} --rows {int(float(testCase["Value count / HDU"]))//10}'
    return cmd


def defineSpecificProgramOptions():
    parser = argparse.ArgumentParser()
    parser.add_argument('--tests', default=None,
            help='The set of test configurations as a TSV file '
            '(columns are: "Test setup", "HDU type", "HDU count", "Value count / HDU"). '
            'The result file can be used to rerun the same benchmark.')
    parser.add_argument('--output', default='/tmp/test.fits', help='The filename of the Fits files to be generated.')
    parser.add_argument('--res', default='/tmp/benchmark.csv', help='The filename of the benchmark results TSV.')
    parser.add_argument('--plot', default='/tmp/plot.eps', help='The filename of the benchmark results plotting file.')
    return parser


def mainMethod(args):

    logger = log.getLogger('EleFitsRunBatchBenchmark')

    if args.tests is not None:
        with open(args.tests, 'r') as f:
            for testCase in csv.DictReader(f, delimiter='\t'):
                logger.debug(testCase)
                cmd = makeCommand(testCase, args.output, args.res, 'INFO') # TODO get log level from args
                logger.info('')
                logger.info(cmd)
                logger.info('')
                subprocess.call(cmd, shell=True)

    if args.plot is not None:
        results = defaultdict(DataFrame)
        with open(args.res, 'r') as f:
            for testCase in csv.DictReader(f, delimiter='\t'):
                hduType = testCase["HDU type"]
                valueCount = int(testCase["Value count / HDU"])
                if hduType == 'Image':
                    shape = f'{scientificNotation(valueCount)} pixels'
                else:
                    shape = f'10 columns x {scientificNotation(valueCount/10)} rows'
                testName = f'{testCase["Mode"]} {hduType}\n({testCase["HDU count"]} HDUs x {shape})'
                testCaseSetup = testCase['Test setup']
                data = [float(s) for s in testCase["Samples (ms)"].split(',')]
                logger.debug(testName + ' ' + testCaseSetup + ': ' + str(data))
                results[testName][testCaseSetup] = data
        cols = 2
        rows = (len(results) + cols - 1) // cols # Equivalent to ceil(len(results)/2) for ints
        logger.info(f'Plotting {len(results)} graphs...')
        _, axes = plt.subplots(ncols=cols, nrows=rows, figsize=(5 * cols, 3 * rows), sharex='col', sharey='row')
        i = False # left column, since cols = 2
        j = 0
        for k, v in results.items():
            logger.info(f'- {k}')
            logger.debug(v.info())
            if cols == 1:
                ax = axes[j]
            elif rows == 1:
                ax = axes[int(i)]
            else:
                ax = axes[j, int(i)]
            ax.set_title(k)
            v.boxplot(vert=False, ax=ax, whis=(5, 95), showfliers=False)
            # plots 5th, 25th, 50th, 75th, 95th percentiles without outliers
            if j == rows - 1:
                ax.set_xlabel('Time (ms)')
            j += int(i)
            i = not i
        plt.tight_layout() # Avoids overlapping texts
        plt.savefig(args.plot, bbox_inches='tight') # 'tight' avoids having text outside the box
