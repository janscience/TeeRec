import os
import argparse
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import wave
try:
    from audioio import metadata_wave
    has_audioio = True
except ImportError:
    has_audioio = False


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


def plot_hist(path, header, widh=30, gain=None, scale_bits=False,
              subtract_mean=True, tskip=0.0, show_filename=False,
              log_counts=False, plot=True, save=False):
    data, rate = load_wave(path)
    #data, rate = load_bin(path, 108)
    if data is None:
        print('file "%s" is empty!' % path)
        return
    if tskip > 0:
        i0 = int(tskip*rate)
        data = data[i0:,:]
    nchannels = data.shape[1]
    basename = os.path.basename(path)
    bits = 16
    pins = []
    convs = '-'
    sampls = '-'
    avrgs = -1
    if has_audioio:
        metadata, cues = metadata_wave(path)
        if 'INFO' in metadata:
            info = metadata['INFO']
            if 'PINS' in info:
                pins = info['PINS'].split(',')
            if 'BITS' in info:
                bits = int(info['BITS'])
            if 'CNVS' in info:
                convs = info['CNVS']
            if 'SMPS' in info:
                sampls = info['SMPS']
            if 'AVRG' in info:
                avrgs = int(info['AVRG'])
    else:
        parts = basename.split('-')
        rate = 1000*float(parts[-5][:3])
        bits = int(parts[-4][:2])
        convs = parts[-3][4:]
        sampls = parts[-2][5:]
        avrgs = int(parts[-1][4:6])
    vunit = ''
    unit = 'integer'
    scale = 1
    if gain is not None:
        scale = 1.66/2**15 * 1000.0/gain
        unit = 'mV'
        vunit = unit
        data = data*scale
        scale_bits = False
    bw = 1
    if basename[:10] != 'averaging-':
        bw = 2**(16-bits) if bits < 16 else 1
    if scale_bits:
        data = data/bw
        bw = 1
    if header:
        if show_filename:
            print(f'{"":<{width}} ', end='')
        if len(convs) > 1 and len(sampls) > 1 and avrgs > 0:
            print(f'{"recording":<32} ', end='')
        else:
            print(f'{"recording":<9} ', end='')
        cw = 7 if pins else 5
        print(f'{"stdev":<{nchannels*cw}}', end='')
        print(f'{"range":<{nchannels*cw}}')
        if show_filename:
            print(f'{"file":<{width}} ', end='')
        print('rate bits', end='')
        if len(convs) > 1 and len(sampls) > 1 and avrgs > 0:
            print(' convers  sampling avrg', end='')
        for c in range(nchannels):
            if pins:
                print(f' {pins[c]:<6s}', end='')
            else:
                print(f' c{c:<4d}', end='')
        for c in range(nchannels):
            if pins:
                print(f' {pins[c]:<6s}', end='')
            else:
                print(f' c{c:<4d}', end='')
        print()
    if show_filename:
        print(f'{basename:<{width}} ', end='')
    print(f'{0.001*rate:4.0f} {bits:4d}', end='')
    if len(convs) > 1 and len(sampls) > 1 and avrgs > 0:
        print(f' {convs:8s} {sampls:8s} {avrgs:4d}', end='')
    for c in range(nchannels):
        s = np.std(data[:,c])
        if pins:
            print(f' {s:6.1f}', end='')
        else:
            print(f' {s:4.1f}', end='')
    for c in range(nchannels):
        r = np.max(data[:,c]) - np.min(data[:,c])
        if pins:
            print(f' {r:6.0f}', end='')
        else:
            print(f' {r:4.0f}', end='')
    print()
    if not plot:
        return
    colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
    abins = np.arange(np.min(data), np.max(data) + 2, bw)*scale
    if nchannels > 1:
        fig, axs = plt.subplots(2, nchannels//2, sharex=True, sharey=True)
        axs = axs.ravel()
    else:
        fig, ax = plt.subplots()
        axs = [ax]
    fig.set_size_inches(8, 6)
    fig.subplots_adjust(top=0.85, bottom=0.1, left=0.1, right=0.96, hspace=0.3)
    if show_filename or len(convs) <= 1:
        fig.suptitle(basename, fontsize=14)
    else:
        fig.suptitle(f'{0.001*rate:.0f}kHz @ {bits}bits: {convs} conversion, {sampls} sampling, avrg={avrgs}', fontsize=14)
    for c in range(nchannels):
        m = np.mean(data[:,c])
        s = np.std(data[:,c])
        r = np.max(data[:,c]) - np.min(data[:,c])
        n, b = np.histogram(data[:,c], abins);
        nmax = np.max(n)
        i0 = max(0, np.argmax(n>0) - 1)
        i1 = min(len(n), len(n) - np.argmax(n[::-1]>0) + 1)
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
        cs = pins[c] if pins else c
        axs[c].set_title(f'channel {cs}')
        axs[c].spines['top'].set_visible(False)
        axs[c].spines['right'].set_visible(False)
        axs[c].text(0.95, 0.87, f'$\mu$={m:.1f}{vunit}',
                    ha='right', transform=axs[c].transAxes)
        axs[c].text(0.95, 0.75, f'$\sigma$={s:.1f}{vunit}',
                    ha='right', transform=axs[c].transAxes)
        axs[c].text(0.95, 0.63, f'range={r:.0f}{vunit}',
                    ha='right', transform=axs[c].transAxes)
        axs[c].set_ylabel('counts')
        if log_counts:
            axs[c].set_ylim(bottom=1)
            axs[c].set_yscale('log')
    axs[nchannels-1].set_xlabel(f'Amplitude [{unit}]')
    if save:
        fig.savefig(os.path.splitext(basename)[0] + '-noise.png')
    else:
        plt.show()

    
if __name__ == '__main__':
    # command line arguments:
    parser = argparse.ArgumentParser(add_help=True,
        description='Analyse noisyness of recorded zero-signals.')
    parser.add_argument('-g', dest='gain', default=None, type=float,
                        help='Gain of an amplifier (default no gain, i.e. raw integer recordings)',
                        metavar='GAIN')
    parser.add_argument('-b', dest='scale_bits', action='store_true',
                        help='scale back to single bits')
    parser.add_argument('-m', dest='subtract_mean', action='store_true',
                        help='subtract mean from data traces')
    parser.add_argument('-f', dest='show_filename', action='store_true',
                        help='show file name in plot title')
    parser.add_argument('-l', dest='log_counts', action='store_true',
                        help='logarithmic counts')
    parser.add_argument('-t', dest='tskip', default=0.0, type=float,
                        help='initial time to skip from analysis (default 0 seconds)',
                        metavar='TSKIP')
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
    gain = args.gain
    scale_bits = args.scale_bits
    subtract_mean = args.subtract_mean
    show_filename = args.show_filename
    log_counts = args.log_counts
    tskip = args.tskip
    plt.rcParams['axes.xmargin'] = 0
    plt.rcParams['axes.ymargin'] = 0
    # analyse:
    header = True
    bn = [len(os.path.basename(path)) for path in args.file]
    width = max(bn)
    for path in args.file:
        plot_hist(path, header, width, gain, scale_bits, subtract_mean,
                  tskip, show_filename, log_counts, plot, save)
        header = False
