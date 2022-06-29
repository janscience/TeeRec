import os
import argparse
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import wave


def load_wave(filepath):
    try:
        wf = wave.open(filepath, 'r')
        nchannels, sampwidth, rate, nframes, comptype, compname = wf.getparams()
        buffer = wf.readframes(nframes)
        if sampwidth == 1:
            dtype = 'u1'
            data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
        else:
            dtype = 'i%d' % sampwidth
            data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
        wf.close()
        return data, float(rate)
    except EOFError:
        return None, None

    
def load_bin(filepath, offset=0):
    with open(filepath, 'rb') as wf:
        wf.seek(offset); # offset of data chunk
        buffer = wf.read()
        dtype = 'i2'
        nchannels = 4
        data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
    rate = 100000
    return data, float(rate)


def plot_hist(path, header, subtract_mean=True, plot=True, save=False):
    data, rate = load_wave(path)
    #data, rate = load_bin(path, 108)
    if data is None:
        print('file "%s" is empty!' % path)
        return
    nchannels = data.shape[1]
    basename = os.path.basename(path)
    parts = basename.split('-')
    rate = 1000*float(parts[1][:3])
    bits = int(parts[2][:2])
    convs = parts[3][4:]
    sampls = parts[4][5:]
    avrgs = int(parts[5][4:6])
    if header:
        print(f'rate bits convers  sampling avrg', end='')
        for c in range(nchannels):
            print(f' c{c:<3d}', end='')
        print()
    print(f'{0.001*rate:4.0f} {bits:4d} {convs:8s} {sampls:8s} {avrgs:4d}', end='')
    for c in range(nchannels):
        m = np.mean(data[:,c])
        s = np.std(data[:,c])
        print(f' {s:4.1f}', end='')
    print()
    if not plot:
        return
    colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
    abins = np.arange(-2**15, 2**15+1, 1)
    if nchannels > 1:
        fig, axs = plt.subplots(2, nchannels//2, sharex=True, sharey=True)
        axs = axs.ravel()
    else:
        fig, ax = plt.subplots()
        axs = [ax]
    fig.set_size_inches(8, 6)
    fig.subplots_adjust(top=0.85, bottom=0.1, left=0.1, right=0.96, hspace=0.3)
    fig.suptitle(f'{0.001*rate:.0f}kHz @ {bits}bits: {convs} conversion, {sampls} sampling, avrg={avrgs}', fontsize=14)
    for c in range(nchannels):
        m = np.mean(data[:,c])
        s = np.std(data[:,c])
        n, b = np.histogram(data[:,c], abins);
        nmax = np.max(n)
        i0 = np.argmax(n>0)
        i1 = len(n) - np.argmax(n[::-1]>0)
        if subtract_mean:
            b -= int(m)
            axs[c].fill_between(b[i0:i1], n[i0:i1], ec='none',
                                fc=colors[c%len(colors)]);
            axs[c].axvline(0, color='k')
            axs[c].plot([-s, +s], [0.1*nmax, 0.1*nmax], 'k', lw=3)
        else:
            axs[c].fill_between(b[i0:i1], n[i0:i1], ec='none',
                                fc=colors[c%len(colors)]);
            axs[c].axvline(m, color='k')
            axs[c].plot([m-s, m+s], [0.1*nmax, 0.1*nmax], 'k', lw=3)
        axs[c].set_title('channel %d' % c)
        axs[c].spines['top'].set_visible(False)
        axs[c].spines['right'].set_visible(False)
        axs[c].text(0.95, 0.87, '$\mu$=%.0f' % m, ha='right', transform=axs[c].transAxes)
        axs[c].text(0.95, 0.75, '$\sigma$=%.1f' % s, ha='right', transform=axs[c].transAxes)
    fig.text(0.55, 0.02, 'Amplitude [integer]', ha='center')
    if save:
        fig.savefig(os.path.splitext(basename)[0] + '-noise.png')
    else:
        plt.show()

    
if __name__ == '__main__':
    # command line arguments:
    parser = argparse.ArgumentParser(add_help=True,
        description='Analyse noisyness of recorded zero-signals.')
    parser.add_argument('-m', dest='subtract_mean', action='store_true',
                        help='subtract mean from data traces')
    parser.add_argument('-p', dest='plot', action='store_true',
                        help='plot distribution of samples')
    parser.add_argument('-s', dest='save', action='store_true',
                        help='save plot to file "-noise.png"')
    parser.add_argument('file', nargs='+', type=str,
                        help='wave files from the averaging sketch')
    args = parser.parse_args()
    # options:
    plot = args.plot
    save = args.save
    subtract_mean = args.subtract_mean
    plt.rcParams['axes.xmargin'] = 0
    plt.rcParams['axes.ymargin'] = 0
    # analyse:
    header = True
    for path in args.file:
        plot_hist(path, header, subtract_mean, plot, save)
        header = False
