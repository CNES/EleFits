# Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
# This file is part of EleFits <github.com/CNES/EleFits>
# SPDX-License-Identifier: LGPL-3.0-or-later

import argparse
from collections import defaultdict
import csv
import subprocess
import matplotlib.pyplot as plt
from pandas import DataFrame
import ElementsKernel.Logging as log


def scientific_notation(value):
    b, e = f'{float(value):.2e}'.split('e')
    if float(b) % 1 == 0:
        b = int(float(b))
    return f'{b}e{int(e)}'


def make_command(test_case, output, results, log_level):
    """Run a test case specified as a dictionary with following keys:
    "Test setup", "HDU type", "HDU count", "Value count / HDU"
    """
    cmd = f'EleFitsRunBenchmark --log-level {log_level} --output {output} --res {results}'
    cmd += f' --setup "{test_case["Test setup"]}"'
    if test_case['HDU type'] == 'Image':
        cmd += f' --images {int(float(test_case["HDU count"]))} --pixels {int(float(test_case["Value count / HDU"]))}'
        # int(float(value)) allows value to be an integer in scientific notation
    if test_case['HDU type'] == 'Binary table':
        cmd += f' --tables {int(float(test_case["HDU count"]))} --rows {int(float(test_case["Value count / HDU"]))//10}'
    return cmd


def defineSpecificProgramOptions():
    parser = argparse.ArgumentParser()
    parser.add_argument('--tests', default=None,
                        help='The set of test configurations as a TSV file '
                        '(columns are: "Test setup", "HDU type", "HDU count", "Value count / HDU"). '
                        'The result file can be used to rerun the same benchmark.')
    parser.add_argument('--output', default='/tmp/test.fits', help='The filename of the FITS files to be generated.')
    parser.add_argument('--res', default='/tmp/benchmark.csv', help='The filename of the benchmark results TSV.')
    parser.add_argument('--plot', default='/tmp/plot.eps', help='The filename of the benchmark results plotting file.')
    return parser


def mainMethod(args):

    logger = log.getLogger('EleFitsRunBatchBenchmark')

    if args.tests is not None:
        with open(args.tests, 'r') as f:
            for test_case in csv.DictReader(f, delimiter='\t'):
                logger.debug(test_case)
                cmd = make_command(test_case, args.output, args.res, 'INFO')  # TODO get log level from args
                logger.info('')
                logger.info(cmd)
                logger.info('')
                subprocess.call(cmd, shell=True)

    if args.plot is not None:
        results = defaultdict(DataFrame)
        with open(args.res, 'r') as f:
            for test_case in csv.DictReader(f, delimiter='\t'):
                hdu_type = test_case["HDU type"]
                value_count = int(test_case["Value count / HDU"])
                if hdu_type == 'Image':
                    shape = f'{scientific_notation(value_count)} pixels'
                else:
                    shape = f'10 columns x {scientific_notation(value_count/10)} rows'
                test_name = f'{test_case["Mode"]} {hdu_type}\n({test_case["HDU count"]} HDUs x {shape})'
                test_case_setup = test_case['Test setup']
                data = [float(s) for s in test_case["Samples (ms)"].split(',')]
                logger.debug(test_name.replace('\n', ' ') + ' ' + test_case_setup + ': ' + str(data))
                results[test_name][test_case_setup] = data
        cols = 2
        rows = (len(results) + cols - 1) // cols  # Equivalent to ceil(len(results)/2) for ints
        logger.info(f'Plotting {len(results)} graphs ({cols} columns x {rows} rows)...')
        _, axes = plt.subplots(ncols=cols, nrows=rows, figsize=(5 * cols, 3 * rows))
        # FIXME Cannot sharex, sharey anymore in EDEN3
        # plt.subplots(ncols=cols, nrows=rows, figsize=(5 * cols, 3 * rows), sharex='col', sharey='row')
        # See bug with pandas+matplotlib:
        # https://github.com/pandas-dev/pandas/issues/36918
        i = False  # left column, since cols = 2
        j = 0
        for k, v in results.items():
            logger.info(f'Ploting ({j}, {int(i)}): ' + k.replace('\n', ' '))
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
        plt.tight_layout()  # Avoids overlapping texts
        plt.savefig(args.plot, bbox_inches='tight')  # 'tight' avoids having text outside the box
        logger.info(f'Saved plot as: {args.plot}')
