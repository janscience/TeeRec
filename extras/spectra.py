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
        wf.seek(offset); # offset of data chunk
        buffer = wf.read()
        dtype = 'i2'
        nchannels = 4
        data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
    rate = 100000
    return data, float(rate)


def plot_psds(path):
    data, rate = load_wave(path)
    #data, rate = load_bin(path, 108)
    data = np.array(data, dtype=np.double)
    nchannels = data.shape[1]
    fig, axs = plt.subplots(2, nchannels//2, sharex=True)
    axs = axs.ravel()
    nfft = 1024*4
    thresh = 5 # dB
    print(rate)
    for c in range(data.shape[1]):
        #data[:,c] = 2**15*np.sin(2.0*np.pi*1000.0*np.arange(len(data))/rate) + 100*np.random.randn(len(data))
        pxx, freqs = psd(data[:,c] - np.mean(data[:,c]), Fs=rate, NFFT=nfft, noverlap=nfft//2)
        db = 10.0*np.log10(pxx/2**14/freqs[-1])
        #db = 10.0*np.log10(pxx/2**15)
        p, t = detect_peaks(db, thresh)
        axs[c].plot(freqs, db)
        axs[c].plot(freqs[p], db[p], 'or')
        for pi in p:
            axs[c].text(freqs[pi]+10, db[pi]+0.3, '%.0fHz' % freqs[pi])
        axs[c].set_title('channel %d' % c)
        axs[c].set_xlabel('Frequency [Hz]')
        axs[c].set_ylabel('Power [dB rel max range]')
    fig.tight_layout()
    plt.show()

    
for path in sys.argv[1:]:
    plot_psds(path)
