import os
import argparse
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.mlab import psd
import wave
try:
    from thunderfish.eventdetection import detect_peaks
    has_thunderfish = True
except ImportError:
    has_thunderfish = False
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
        wf.seek(offset)  # offset of data chunk
        buffer = wf.read()
        dtype = 'i2'
        nchannels = 4
        data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
    rate = 100000
    return data, float(rate)


def plot_psds(path, channel, maxfreq, save):
    data, rate = load_wave(path)
    #data, rate = load_bin(path, 108)
    #data = np.array(data, dtype=np.double)
    pins = []
    if has_audioio:
        metadata, cues = metadata_wave(path)
        info = metadata['INFO']
        pins = info['PINS'].split(',')
    nchannels = data.shape[1]
    if channel >= 0:
        nchannels = 1
    if nchannels > 1:
        fig, axs = plt.subplots(2, nchannels//2, sharex=True)
        axs = axs.ravel()
    else:
        fig, ax = plt.subplots()
        axs = [ax]
    fig.set_size_inches(12, 6)
    fig.subplots_adjust(top=0.88, bottom=0.08, left=0.07, right=0.98,
                        hspace=0.3)
    fig.suptitle(os.path.basename(path), fontsize=16)
    colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
    tscale = 0.001
    funit = 'kHz'
    if maxfreq and maxfreq < 1200:
        tscale = 1
        funit = 'Hz'
    nfft = 1024*8
    thresh = 10 # dB
    for c in range(nchannels):
        ch = c
        if channel >= 0:
            ch = channel
        #data[:,ch] = 2**15*np.sin(2.0*np.pi*1000.0*np.arange(len(data))/rate) + 100*np.random.randn(len(data))
        pxx, freqs = psd(data[:,ch] - np.mean(data[:,ch]), Fs=rate, NFFT=nfft, noverlap=nfft//2)
        db = 10.0*np.log10(pxx/2**14/freqs[-1])
        #db = 10.0*np.log10(pxx/2**15)
        axs[c].plot(tscale*freqs, db, color=colors[c%len(colors)])
        if has_thunderfish:
            p, t = detect_peaks(db, thresh)
            if maxfreq:
                p = p[freqs[p] < maxfreq]
            axs[c].plot(tscale*freqs[p], db[p], 'o', color='gray', clip_on=False)
            for pi in p:
                axs[c].text(tscale*(freqs[pi]+40), db[pi]+0.4, '%.0fHz' % freqs[pi])
        cs = pins[ch] if pins else ch
        axs[c].set_title(f'channel {cs}')
        if c % 2 == 1 or nchannels == 1:
            axs[c].set_xlabel(f'Frequency [{funit}]')
        axs[c].set_ylabel('Power [dB rel max range]')
        if maxfreq:
            axs[c].set_xlim(0, tscale*maxfreq)
        axs[c].spines['top'].set_visible(False)
        axs[c].spines['right'].set_visible(False)
    if save:
        fig.savefig(os.path.splitext(path)[0] + '-spectra.png')
    else:
        plt.show()

    
if __name__ == '__main__':
    # command line arguments:
    parser = argparse.ArgumentParser(add_help=True,
        description='Plot power spectra of recorded signals.')
    parser.add_argument('-c', dest='channel', default=-1, type=int,
                        help='show trace of channel CHANNEL only',
                        metavar='CHANNEL')
    parser.add_argument('-f', dest='maxfreq', default=None, type=float,
                        help='Maximum frequency shown in the plot in Hertz',
                        metavar='MAXFREQ')
    parser.add_argument('-s', dest='save', action='store_true',
                        help='save plot to png file')
    parser.add_argument('file', nargs='+', type=str,
                        help='wave files containing the data to be shown')
    args = parser.parse_args()
    # options:
    channel = args.channel
    maxfreq = args.maxfreq
    save = args.save
    plt.rcParams['axes.xmargin'] = 0
    plt.rcParams['axes.ymargin'] = 0
    for path in args.file:
        plot_psds(path, channel, maxfreq, save)
