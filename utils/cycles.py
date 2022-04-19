import os
import sys
import numpy as np
import matplotlib.pyplot as plt
import wave
from thunderfish.eventdetection import threshold_crossings


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


def analyze_periodicity(path):
    data, rate = load_wave(path)
    if data is None:
        return
    #data, rate = load_bin(path, 108)
    nchannels = data.shape[1]
    basename = os.path.basename(path)
    thresh = 0
    if nchannels > 1:
        fig, axs = plt.subplots(2, nchannels//2, sharex=True)
        axs = axs.ravel()
    else:
        fig, ax = plt.subplots()
        axs = [ax]
    fig.subplots_adjust(left=0.06, right=0.98, top=0.94, bottom=0.09)
    for c in range(nchannels):
        up, down = threshold_crossings(data[:,c], thresh)
        isi = np.diff(up)
        if len(isi) > 10:
            axs[c].hist(isi, -0.5 + np.arange(np.max(isi)+2))
        axs[c].set_title('channel %d' % c)
    plt.show()
    """
    ax.plot(data[:1000,c]);
    ax.plot(up, np.zeros(len(up))+thresh, 'o')
    ax.plot(down, np.zeros(len(down))+thresh, 'o')
    ax.set_xlim(0, 1000)
    print(np.diff(up[:-1][isi<maxipi]))
    for i in np.where(isi<maxipi)[0]:
        print(i, isi[i])
        k = up[i]
        plt.title('%s: %d isi=%d' % (basename, i, isi[i]))
        plt.plot(data[k-200:k+200,0])
        plt.plot(data[k-200:k+200,2])
        plt.plot(data[k-200:k+200,4])
        plt.plot(data[k-200:k+200,6])
        plt.show()
    """

if __name__ == '__main__':
    for path in sys.argv[1:]:
        analyze_periodicity(path)


