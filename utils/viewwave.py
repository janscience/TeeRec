import os
import argparse
import numpy as np
import matplotlib.pyplot as plt
import wave
try:
    from audioio import metadata
    has_audioio = True
except ImportError:
    has_audioio = False


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

    
def load_bin(filepath, rate=48000, nchannels=2, offset=0):
    with open(filepath, 'rb') as wf:
        wf.seek(offset); # offset of data chunk
        buffer = wf.read()
        dtype = 'i2'
        data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
    global has_audioio
    has_audioio = False
    return data, float(rate)


def unwrap(data, thresh=-0.01):
    """Fixes data that exceeded the -1 to 1 range.

    If data that exceed the range from -1.0 to 1.0 are stored in a wav file,
    they get wrapped around. This functions tries to undo this wrapping.

    From https://github.com/bendalab/audioio/blob/master/audioio/audioloader.py .
    
    Parameters
    ----------
    data: 1D or 2D ndarray
        Data to be fixed.

    Returns
    -------
    data: same as input data
        The fixed data.
    """
    if len(data.shape) > 1:
        for c in range(data.shape[1]):
            data[:,c] = unwrap(data[:,c])
    else:
        for k in range(1000):
            dd = (data[1:] < -thresh) & (np.diff(data) <= -1.0)
            du = (data[1:] > thresh) & (np.diff(data) >= 1.0)
            if np.sum(dd) == 0 and np.sum(du) == 0:
                break
            data[1:][dd] += 2.0
            data[1:][du] -= 2.0
    return data


def plot_traces(path, channel, toffs, tmax, step, gain, unwrapd, raw,
                diff, autoy, metadata_title, save):
    data, rate = load_wave(path)
    #data, rate = load_bin(path, 48000, 4, 4)
    if data is None:
        print('file "%s" is empty!' % path)
        return
    if diff:
        if unwrapd:
            data0 = 2**15*unwrap(data[:, 0]/2**15)
            data1 = 2**15*unwrap(data[:, 1]/2**15)
            data = data1 - data0
        else:
            data = data[:, 1] - data[:, 0]
        data = data.reshape(len(data), 1)
    basename = os.path.basename(path)
    pins = []
    title = basename
    gains = ''
    if has_audioio:
        meta_data = metadata(path)
        if 'INFO' in meta_data:
            info = meta_data['INFO']
            if 'PINS' in info:
                pins = info['PINS'].split(',')
            if 'GAIN' in info:
                gains = info['GAIN']
            if metadata_title:
                if 'BITS' in info and 'CNVS' in info and 'SMPS' in info and 'AVRG' in info:
                    title = f"{0.001*rate:.0f}kHz @ {info['BITS']}bits: {info['CNVS']} conversion, {info['SMPS']} sampling, avrg={info['AVRG']}"
                elif 'BITS' in info:
                    title = f"{0.001*rate:.1f}kHz@{info['BITS']}bits"
    fig, ax = plt.subplots(figsize=(12,6))
    fig.subplots_adjust(left=0.07, right=0.98, top=0.94, bottom=0.09)
    colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
    time = np.arange(len(data))/rate
    dtmax = time[-1] - toffs
    if dtmax > tmax:
        dtmax = tmax
    dtidx = int((toffs+dtmax)*rate)
    idx0 = int(toffs*rate)
    scale = 1.0
    unit = 'integer'
    offs = 0
    if raw:
        offs = 2**15
    if gain and gains:
        unit = ''
        scale = 1.0
        for i in range(len(gains)):
            if not gains[i].isdecimal() and gains[i] != '.' and gains[i] != 'e':
                unit = gains[i:]
                scale = float(gains[:i])/2**15
                break
    for c in range(data.shape[1]):
        if channel < 0 or c == channel:
            label = f'{c}'
            if c < len(pins):
                label = pins[c]
            trace = data[:,c]
            if gain and unwrapd:
                trace = trace/2**15
                trace = unwrap(trace)
                trace *= 2**15
            trace = trace[idx0:dtidx]
            ax.plot(1000*time[idx0:dtidx], scale*trace+scale*(offs+step*c),
                    '-', color=colors[c%len(colors)], label=label)
    maxy = 40000 + data.shape[1]*step
    if not autoy:
        if raw:
            ax.set_ylim(0, 2*maxy*scale)
        else:
            ax.set_ylim(-40000*scale, maxy*scale)
    ax.set_xlabel('Time [ms]')
    ax.set_ylabel('Recording [%s]' % unit)
    if data.shape[1] > 1:
        ax.legend(loc='lower right')
    ax.set_title(title, fontsize=16)
    if save:
        if gain and unwrapd:
            fig.savefig(os.path.splitext(path)[0] + '-unwrapped-traces.png')
        else:
            fig.savefig(os.path.splitext(path)[0] + '-traces.png')
    else:
        plt.show()

    
if __name__ == '__main__':
    # command line arguments:
    parser = argparse.ArgumentParser(add_help=True,
        description='Display a segment of a wave file.')
        #epilog='version %s by Benda-Lab (2015-%s)' % (__version__, __year__))
    #parser.add_argument('--version', action='version', version=__version__)
    parser.add_argument('-c', dest='channel', default=-1, type=int,
                        help='show trace of channel CHANNEL only',
                        metavar='CHANNEL')
    parser.add_argument('-o', dest='toffs', default=0.0, type=float,
                        help='show from this time on (default 0, i.e. beginning of recording)',
                        metavar='OFFS')
    parser.add_argument('-t', dest='tmax', default=1.0, type=float,
                        help='maximum time to show (default 1 second)',
                        metavar='TMAX')
    parser.add_argument('-S', dest='step', default=0, type=int,
                        help='shift each channel by STEP integers upwards',
                        metavar='STEP')
    parser.add_argument('-g', dest='gain', action='store_true', 
                        help='Use gain stored in metadata for proper scaling (default no gain, i.e. raw integer recordings)')
    parser.add_argument('-r', dest='raw', action='store_true',
                        help='raw voltage readings without offset from 0 to 3.3V')
    parser.add_argument('-a', dest='autoy', action='store_true',
                        help='auto scale y-axis')
    parser.add_argument('-i', dest='metadata', action='store_true',
                        help='write info section from metadata into title of plot')
    parser.add_argument('-u', dest='unwrap', action='store_true', 
                        help='Unwrap clipped data using unwrap() from audioio package')
    parser.add_argument('-d', dest='diff', action='store_true', 
                        help='Take the difference between the first two channels')
    parser.add_argument('-s', dest='save', action='store_true',
                        help='save plot to png file')
    parser.add_argument('file', nargs='+', type=str,
                        help='wave files containing the data to be shown')
    args = parser.parse_args()
    # options:
    channel = args.channel
    toffs = args.toffs
    tmax = args.tmax
    step = args.step
    gain = args.gain
    unwrapd = args.unwrap
    if unwrapd:
        gain = True
    raw = args.raw
    diff = args.diff
    autoy = args.autoy
    metadata_title = args.metadata
    save = args.save
    # load and plot:
    plt.rcParams['axes.xmargin'] = 0
    plt.rcParams['axes.ymargin'] = 0.02
    for path in args.file:
        plot_traces(path, channel, toffs, tmax, step, gain, unwrapd, raw,
                    diff, autoy, metadata_title, save)
