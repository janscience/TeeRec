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


def plot_traces(path, tmax, step, save):
    data, rate = load_wave(path)
    #data, rate = load_bin(path, 0)
    if data is None:
        print('file "%s" is empty!' % path)
        return
    basename = os.path.basename(path)
    fig, ax = plt.subplots(figsize=(12,6))
    fig.subplots_adjust(left=0.06, right=0.98, top=0.94, bottom=0.09)
    time = np.arange(len(data))/rate
    dtmax = time[-1]
    if dtmax > tmax:
        dtmax = tmax
    max_idx = int(dtmax*rate)
    for c in range(data.shape[1]):
        ax.plot(1000*time[:max_idx], 1.0*data[:max_idx,c]+step*c, '-',
                label='%d' % c)
    maxy = 40000 + data.shape[1]*step
    ax.set_ylim(-40000, maxy)
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
    step = 0
    tmax = 2
    for path in sys.argv[1:]:
        if tmax is None:
            tmax = float(path)
            continue
        if step is None:
            step = float(path)
            continue
        if path == '-s':
            save = True
            continue
        if path == '-t':
            tmax = None
            continue
        if path == '-S':
            step = None
            continue
        plot_traces(path, tmax, step, save)
    if len(sys.argv) <= 1:
        print("Usage:")
        print("viewwave [-t TMAX] [-S STEP] [-s] FILE")
        print()
        print("-t TMAX: maximum time to show. Defaults to 2 seconds.")
        print("-S STEP: shift each channel by STEP upwards.")
        print("-s     : save plot to png file.")
        print("FILE   : wave file with data to display.")
