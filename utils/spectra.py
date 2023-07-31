import os
import argparse
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.mlab import psd, window_none, window_hanning
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
        factor = 2.0**(sampwidth*8-1)
        if sampwidth == 1:
            dtype = 'u1'
            buffer = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
            data = buffer.astype('d')/factor - 1.0
        else:
            dtype = 'i%d' % sampwidth
            buffer = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
            data = buffer.astype('d')/factor
        wf.close()
        return data, float(rate)
    except EOFError:
        return None, None

    
def load_bin(filepath, rate=48000, nchannels=2, offset=0):
    with open(filepath, 'rb') as wf:
        wf.seek(offset); # offset of data chunk
        buffer = wf.read()
        dtype = 'i2'
        data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
    global has_audioio
    has_audioio = False
    return data, float(rate)


def unwrap(data, thresh=-0.01):
    """Fixes data that exceeded the -1 to 1 range.

    If data that exceed the range from -1.0 to 1.0 are stored in a wav file,
    they get wrapped around. This functions tries to undo this wrapping.

    From https://github.com/bendalab/audioio/blob/master/audioio/audioloader.py .
    
    Parameters
    ----------
    data: 1D or 2D ndarray
        Data to be fixed.

    Returns
    -------
    data: same as input data
        The fixed data.
    """
    if len(data.shape) > 1:
        for c in range(data.shape[1]):
            data[:,c] = unwrap(data[:,c])
    else:
        for k in range(1000):
            dd = (data[1:] < -thresh) & (np.diff(data) <= -1.0)
            du = (data[1:] > thresh) & (np.diff(data) >= 1.0)
            if np.sum(dd) == 0 and np.sum(du) == 0:
                break
            data[1:][dd] += 2.0
            data[1:][du] -= 2.0
    return data


def plot_psds(path, channel, maxfreq, maxdb, unwrapd, save):
    data, rate = load_wave(path)
    #data, rate = load_bin(path, 96000, 2, 0)
    #data = np.array(data, dtype=np.double)
    pins = []
    if has_audioio:
        metadata, cues = metadata_wave(path)
        if 'INFO' in metadata:
            info = metadata['INFO']
            if 'PINS' in info:
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
    nfft = 1024*32
    thresh = 10 # dB
    for c in range(nchannels):
        ch = c
        if channel >= 0:
            ch = channel
        #data[:,ch] = np.sin(2.0*np.pi*1000.37*np.arange(len(data))/rate) + 0.001*np.random.randn(len(data))
        trace = data[:,ch]
        if unwrapd:
            trace = unwrap(trace)
        pxx, freqs = psd(trace - np.mean(trace), Fs=rate, NFFT=nfft,
                         noverlap=nfft//2, window=window_hanning)
        db = pxx.copy()
        db[pxx <= 1e-20] = float('-inf')
        db[pxx > 1e-20] = 10.0*np.log10(pxx[pxx > 1e-20]*freqs[1])
        axs[c].plot(tscale*freqs, db, color=colors[c%len(colors)])
        if has_thunderfish:
            p, t = detect_peaks(db, thresh)
            if maxfreq:
                p = p[freqs[p] < maxfreq]
            axs[c].plot(tscale*freqs[p], db[p], 'o', color='gray', clip_on=False)
            for pi in p:
                axs[c].text(tscale*(freqs[pi]+40), db[pi]+0.4, '%.0fHz' % freqs[pi])
        cs = pins[ch] if ch < len(pins) else ch
        axs[c].set_title(f'channel {cs}')
        if c % 2 == 1 or nchannels == 1:
            axs[c].set_xlabel(f'Frequency [{funit}]')
        axs[c].set_ylabel('Power [dBFS]')
        if maxfreq:
            axs[c].set_xlim(0, tscale*maxfreq)
        if not maxdb is None:
            axs[c].set_ylim(top=maxdb)
        axs[c].spines['top'].set_visible(False)
        axs[c].spines['right'].set_visible(False)
    if save:
        if unwrapd:
            fig.savefig(os.path.splitext(path)[0] + '-unwrapped-spectra.png')
        else:
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
    parser.add_argument('-m', dest='maxdb', default=None, type=float,
                        help='Maximum power shown in the plot in decibel',
                        metavar='MAXDB')
    parser.add_argument('-u', dest='unwrap', action='store_true', 
                        help='Unwrap clipped data using unwrap() from audioio package')
    parser.add_argument('-s', dest='save', action='store_true',
                        help='save plot to png file')
    parser.add_argument('file', nargs='+', type=str,
                        help='wave files containing the data to be shown')
    args = parser.parse_args()
    # options:
    channel = args.channel
    maxfreq = args.maxfreq
    maxdb = args.maxdb
    unwrapd = args.unwrap
    save = args.save
    plt.rcParams['axes.xmargin'] = 0
    plt.rcParams['axes.ymargin'] = 0
    for path in args.file:
        plot_psds(path, channel, maxfreq, maxdb, unwrapd, save)
