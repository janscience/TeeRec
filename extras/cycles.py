import sys
import numpy as np
import matplotlib.pyplot as plt
import wave
from thunderfish.eventdetection import threshold_crossings


def load_wave(filepath, verbose=0):
    wf = wave.open(filepath, 'r')   # 'with' is not supported by wave
    (nchannels, sampwidth, rate, nframes, comptype, compname) = wf.getparams()
    if verbose > 1:
        # this should be a separate function with the sndheader module and for all audio formats
        print('channels       : %d' % nchannels)
        print('bytes          : %d' % sampwidth)
        print('sampling rate  : %g' % rate)
        print('frames         : %d' % nframes)
        print('compression type: %s' % comptype)
        print('compression name: %s' % compname)
    buffer = wf.readframes(nframes)
    if sampwidth == 1:
        dtype = 'u1'
        data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
    else:
        dtype = 'i%d' % sampwidth
        data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
    wf.close()
    return data, float(rate)


def load_n_plot(path):
    try:
        data, rate = load_wave(path, verbose=0)
    except EOFError:
        return
    basename = path.split('/')[-1]
    c = 0
    thresh = 5000
    """
    c = 2
    thresh = 0
    """
    up, down = threshold_crossings(data[:,c], thresh)
    isi = np.diff(up)
    fig, ax = plt.subplots(figsize=(12,6))
    fig.subplots_adjust(left=0.06, right=0.98, top=0.94, bottom=0.09)
    ax.hist(isi, -0.5 + np.arange(np.max(isi)+2))
    """
    ax.plot(data[:1000,c]);
    ax.plot(up, np.zeros(len(up))+thresh, 'o')
    ax.plot(down, np.zeros(len(down))+thresh, 'o')
    ax.set_xlim(0, 1000)
    """
    #fig.savefig(basename.split('.')[0] + '-noise.png')
    plt.show()
    print(np.diff(up[:-1][isi<40]))
    for i in np.where(isi<40)[0]:
        print(i, isi[i])
        k = up[i]
        plt.title('%s: %d isi=%d' % (basename, i, isi[i]))
        plt.plot(data[k-200:k+200,0])
        plt.plot(data[k-200:k+200,2])
        plt.plot(data[k-200:k+200,4])
        plt.show()

    
for path in sys.argv[1:]:
    load_n_plot(path)
