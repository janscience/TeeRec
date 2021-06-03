import sys
import numpy as np
import matplotlib.pyplot as plt
import wave


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
    m = np.mean(data[:,c])
    s = np.std(data[:,c])
    print('%s\t%5.0f\t%5.0f' % (basename, m, s))
    return
    print('file: %s, channel %d: mean=%.0f, std=%.0f' % (basename, c, m, s))
    fig, ax = plt.subplots(figsize=(12,6))
    fig.subplots_adjust(left=0.06, right=0.98, top=0.94, bottom=0.09)
    abins = np.arange(-2**15, 2**15, 2*16)
    n, b = np.histogram(data[:,c], abins);
    nmax = np.max(n)
    ax.fill_between(b[:-1][n>1], n[n>1], 1);
    ax.axvline(m, color='k')
    ax.plot([m-s, m+s], [0.1*nmax, 0.1*nmax], 'k', lw=3)
    ax.set_title(basename)
    ax.set_yscale('log')
    ax.set_ylim(1, 1.1*nmax)
    ax.set_xlabel('Amplitude [bits]')
    #fig.savefig(basename.split('.')[0] + '-noise.png')
    plt.show()

    
for path in sys.argv[1:]:
    load_n_plot(path)
