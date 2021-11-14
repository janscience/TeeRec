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


def plot_traces(path, channel, toffs, tmax, step, autoy, save):
    data, rate = load_wave(path)
    #data, rate = load_bin(path, 0)
    if data is None:
        print('file "%s" is empty!' % path)
        return
    basename = os.path.basename(path)
    fig, ax = plt.subplots(figsize=(12,6))
    fig.subplots_adjust(left=0.06, right=0.98, top=0.94, bottom=0.09)
    time = np.arange(len(data))/rate
    dtmax = time[-1] - toffs
    if dtmax > tmax:
        dtmax = tmax
    dtidx = int((toffs+dtmax)*rate)
    idx0 = int(toffs*rate)
    for c in range(data.shape[1]):
        if channel < 0 or c == channel:
            ax.plot(1000*time[idx0:dtidx], 1.0*data[idx0:dtidx,c]+step*c, '-',
                    label='%d' % c)
    maxy = 40000 + data.shape[1]*step
    if not autoy:
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
    channel = -1
    toffs = 0
    tmax = 2
    step = 0
    autoy = False
    save = False
    for path in sys.argv[1:]:
        if channel is None:
            channel = int(path)
            continue
        if tmax is None:
            tmax = float(path)
            continue
        if toffs is None:
            toffs = float(path)
            continue
        if step is None:
            step = float(path)
            continue
        if path == '-c':
            channel = None
            continue
        if path == '-o':
            toffs = None
            continue
        if path == '-t':
            tmax = None
            continue
        if path == '-S':
            step = None
            continue
        if path == '-a':
            autoy = True
            continue
        if path == '-s':
            save = True
            continue
        plot_traces(path, channel, toffs, tmax, step, autoy, save)
    if len(sys.argv) <= 1:
        print("Usage:")
        print("viewwave [-c CHANNEL] [-t TMAX] [-o TOFFS] [-S STEP] [-a] [-s] FILE")
        print()
        print("-c CHANNEL: show trace of channel CHANNEL only.")
        print("-t TMAX   : maximum time to show. Defaults to 2 seconds.")
        print("-o TOFFS  : show from this time on. Defaults to beginning of recording.")
        print("-S STEP   : shift each channel by STEP upwards.")
        print("-a        : auto scale y-axis.")
        print("-s        : save plot to png file.")
        print("FILE      : wave file with data to display.")
