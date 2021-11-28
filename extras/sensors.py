import argparse
import numpy as np
import pandas as pd
import datetime as dt
import matplotlib.pyplot as plt


def sensors(path):
    data = pd.read_csv(path, sep=',')
    data['time'] = pd.to_datetime(data['time'])
    data['time'] = pd.Series(data['time'].dt.to_pydatetime(), dtype=object)
    header = list(data)[1:]
    # plot data:
    colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
    fig, axs = plt.subplots(len(header), 1, squeeze=False, sharex=True);
    for k, hn in enumerate(header):
        axs[k,0].plot(np.array(data['time']), np.array(data[hn]),
                      '-', color=colors[k%len(colors)])
        name, unit = hn.split('/')
        if len(unit) > 0:
            axs[k,0].set_ylabel('%s [%s]' % (name, unit))
        else:
            axs[k,0].set_ylabel(hn)
    plt.setp(axs[-1,0].get_xticklabels(), ha='right',
             rotation=30, rotation_mode='anchor')
    plt.show()
    

if __name__ == '__main__':
    # command line arguments:
    parser = argparse.ArgumentParser(add_help=True,
        description='Display sensor data.')
        #epilog='version %s by Benda-Lab (2015-%s)' % (__version__, __year__))
    #parser.add_argument('--version', action='version', version=__version__)
    parser.add_argument('file', nargs=1, type=str,
                        help='csv file containing the sensor data to be shown')
    args = parser.parse_args()
    # options:
    path = args.file[0]
    # run:
    sensors(path)
