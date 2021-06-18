import os
import sys
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.mlab import psd
from thunderfish.eventdetection import detect_peaks
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
        wf.seek(offset)  # offset of data chunk
        buffer = wf.read()
        dtype = 'i2'
        nchannels = 4
        data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
    rate = 100000
    return data, float(rate)


def plot_psds(path, save):
    data, rate = load_wave(path)
    #data, rate = load_bin(path, 108)
    data = np.array(data, dtype=np.double)
    nchannels = data.shape[1]
    fig, axs = plt.subplots(2, nchannels//2, sharex=True)
    fig.subplots_adjust(top=0.88, right=0.96, hspace=0.2)
    fig.suptitle(os.path.basename(path), fontsize=16)
    axs = axs.ravel()
    nfft = 1024*4
    thresh = 5 # dB
    for c in range(data.shape[1]):
        #data[:,c] = 2**15*np.sin(2.0*np.pi*1000.0*np.arange(len(data))/rate) + 100*np.random.randn(len(data))
        pxx, freqs = psd(data[:,c] - np.mean(data[:,c]), Fs=rate, NFFT=nfft, noverlap=nfft//2)
        db = 10.0*np.log10(pxx/2**14/freqs[-1])
        #db = 10.0*np.log10(pxx/2**15)
        p, t = detect_peaks(db, thresh)
        axs[c].plot(0.001*freqs, db)
        axs[c].plot(0.001*freqs[p], db[p], 'or')
        for pi in p:
            axs[c].text(0.001*(freqs[pi]+10), db[pi]+0.3, '%.0fHz' % freqs[pi])
        axs[c].set_title('channel %d' % c)
        if c % 2 == 1:
            axs[c].set_xlabel('Frequency [kHz]')
        axs[c].set_ylabel('Power [dB rel max range]')
    if save:
        fig.savefig(os.path.splitext(path)[0] + '-spectra.png')
    else:
        plt.show()

    
if __name__ == '__main__':
    save = False
    for path in sys.argv[1:]:
        if path == '-s':
            save = True
            continue
        plot_psds(path, save)
