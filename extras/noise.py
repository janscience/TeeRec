import sys
import numpy as np
import matplotlib.pyplot as plt
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


def plot_hist(path):
    data, rate = load_wave(path)
    #data, rate = load_bin(path, 108)
    nchannels = data.shape[1]
    basename = path.split('/')[-1]
    abins = np.arange(-2**15, 2**15, 2*16)
    fig, axs = plt.subplots(2, nchannels//2, sharex=True)
    for c in range(data.shape[1]):
        m = np.mean(data[:,c])
        s = np.std(data[:,c])
        print('%s\t%2d\t%5.0f\t%5.0f' % (basename, c, m, s))
        n, b = np.histogram(data[:,c], abins);
        nmax = np.max(n)
        axs[c].fill_between(b[:-1][n>1], n[n>1], 1);
        axs[c].axvline(m, color='k')
        axs[c].plot([m-s, m+s], [0.1*nmax, 0.1*nmax], 'k', lw=3)
        axs[c].set_title('channel %d' % c)
        #axs[c].set_yscale('log')
        #axs[c].set_ylim(1, 1.1*nmax)
        axs[c].set_xlabel('Amplitude [bits]')
        axs[c].text(0.95, 0.9, '$\sigma$=%.0fbits' % s, ha='right', transform=axs[c].transAxes)
    #fig.savefig(basename.split('.')[0] + '-noise.png')
    plt.show()

    
if __name__ == '__main__':
    print('%s\t%2s\t%5s\t%5s' % ('file', 'channel', 'mean', 'std'))
    for path in sys.argv[1:]:
        plot_hist(path)
