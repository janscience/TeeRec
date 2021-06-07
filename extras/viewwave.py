import sys
import numpy as np
import matplotlib.pyplot as plt
import wave
#from audioio import load_audio


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
    #factor = 2.0**(sampwidth*8-1)
    if sampwidth == 1:
        dtype = 'u1'
        data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
        #data = buffer.astype('d')/factor - 1.0
    else:
        dtype = 'i%d' % sampwidth
        data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
        #data = buffer.astype('d')/factor
    wf.close()
    return data, float(rate)


def load_bin(filepath, verbose=0):
    with open(filepath, 'rb') as wf:
        wf.seek(108); # offset of data chunk
        buffer = wf.read()
        dtype = 'i2'
        nchannels = 4
        data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
    rate = 100000
    return data, float(rate)


def load_n_plot(path):
    #data, rate = load_audio(path, verbose=2)
    """
    try:
        data, rate = load_wave(path, verbose=0)
    except EOFError:
        return
    """
    data, rate = load_wave(path, verbose=0)
    #data, rate = load_bin(path, verbose=0)
    basename = path.split('/')[-1]
    #print(data.shape)
    fig, ax = plt.subplots(figsize=(12,6))
    fig.subplots_adjust(left=0.06, right=0.98, top=0.94, bottom=0.09)
    tmin = 0.0
    trange = 0.1
    #trange = 5.0
    time = np.arange(len(data))/rate
    sel = (time>tmin) & (time<tmin+trange)
    #ascale = 1000.0*0.5*3.3/100.0
    ascale = 1.0
    for c in range(data.shape[1]):
        #ax.plot(1000.0*(time[sel]-tmin), ascale*data[sel,c]+1000*c, '-', label='%d' % c)
        ax.plot(1000.0*time[:10000], ascale*data[:10000,c]+1000*c, '-', label='%d' % c)
    #ax.set_ylim(-1.1, 1.1)
    ax.set_title(basename)
    ax.set_xlabel('Time [ms]')
    #ax.set_ylabel('Amplitude [mV]')
    if data.shape[1] > 1:
        ax.legend(loc='lower right')
    fig.savefig(basename.split('.')[0] + '.png')
    plt.show()

    
for path in sys.argv[1:]:
    load_n_plot(path)
