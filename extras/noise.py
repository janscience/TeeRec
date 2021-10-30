import os
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


def plot_hist(path, save, subtract_mean=True):
    data, rate = load_wave(path)
    #data, rate = load_bin(path, 108)
    if data is None:
        print('file "%s" is empty!' % path)
        return
    nchannels = data.shape[1]
    basename = os.path.basename(path)
    abins = np.arange(-2**15, 2**15+1, 1)
    if nchannels > 1:
        fig, axs = plt.subplots(2, nchannels//2, sharex=True)
        axs = axs.ravel()
    else:
        fig, ax = plt.subplots()
        axs = [ax]
    fig.subplots_adjust(top=0.88, right=0.96, hspace=0.2)
    fig.suptitle(basename, fontsize=16)
    for c in range(nchannels):
        m = int(np.mean(data[:,c]))
        s = int(np.std(data[:,c]))
        print('%s\t%2d\t%5d\t%5d' % (basename, c, m, s))
        n, b = np.histogram(data[:,c], abins);
        nmax = np.max(n)
        i0 = np.argmax(n>0)
        i1 = len(n) - np.argmax(n[::-1]>0)
        if subtract_mean:
            b -= m
            axs[c].fill_between(b[i0:i1], n[i0:i1]);
            axs[c].axvline(0, color='k')
            axs[c].plot([-s, +s], [0.1*nmax, 0.1*nmax], 'k', lw=3)
        else:
            axs[c].fill_between(b[i0:i1], n[i0:i1]);
            axs[c].axvline(m, color='k')
            axs[c].plot([m-s, m+s], [0.1*nmax, 0.1*nmax], 'k', lw=3)
        axs[c].set_title('channel %d' % c)
        if c % 2 == 1 or nchannels == 1:
            axs[c].set_xlabel('Amplitude [integer]')
        axs[c].text(0.95, 0.87, '$\mu$=%.0f' % m, ha='right', transform=axs[c].transAxes)
        axs[c].text(0.95, 0.75, '$\sigma$=%.0f' % s, ha='right', transform=axs[c].transAxes)
    if save:
        fig.savefig(os.path.splitext(path)[0] + '-noise.png')
    else:
        plt.show()

    
if __name__ == '__main__':
    save = False
    print('%s\t%2s\t%5s\t%5s' % ('file', 'channel', 'mean', 'std'))
    for path in sys.argv[1:]:
        if path == '-s':
            save = True
            continue
        plot_hist(path, save, True)
