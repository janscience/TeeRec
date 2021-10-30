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
        nchannels = 1
        data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
    rate = 80000
    return data, float(rate)


def plot_traces(path, save):
    data, rate = load_wave(path)
    #data, rate = load_bin(path, 0)
    if data is None:
        print('file "%s" is empty!' % path)
        return
    basename = os.path.basename(path)
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
        #ax.plot(1000.0*time[:], ascale*data[:,c]+1000*c, '-', label='%d' % c)
        ax.plot(ascale*data[:,c]+1000*c, '-', label='%d' % c)
    ax.set_ylim(-40000, 40000)
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Amplitude [integer]')
    if data.shape[1] > 1:
        ax.legend(loc='lower right')
    ax.set_title(basename, fontsize=16)
    if save:
        fig.savefig(os.path.splitext(path)[0] + '-traces.png')
    else:
        plt.show()

    
if __name__ == '__main__':
    save = False
    for path in sys.argv[1:]:
        if path == '-s':
            save = True
            continue
        plot_traces(path, save)
