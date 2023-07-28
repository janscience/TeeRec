import sys
import numpy as np
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
        return data, (nchannels, sampwidth, rate, nframes, comptype, compname)
    except EOFError:
        return None, None


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('usage: mergechannels.py -o OUTFILE file1 file2 ...')
        print()
        print('Take from each provided wav file one channel and merge them into OUTFILE.')

    # get name of out file:
    outfile = 'out.wav'
    for k in range(len(sys.argv)):
        if sys.argv[k] == '-o':
            outfile = sys.argv[k+1]
            del sys.argv[k:k+2]
            break
    # read channels:
    data = None
    sampwidth = 0
    rate = 0
    for c in range(len(sys.argv[1:])):
        fname = sys.argv[1 + c]
        fdata, params = load_wave(fname)
        if data is None:
            data = fdata.copy()
            sampwidth = params[1]
            rate = params[2]
        else:
            if np.abs(params[2] - rate) > 1:
                print(rate, params[2])
                print('WARNING: sampling rates do not match!')
            if params[1] != sampwidth:
                print('WARNING: sampling rates do not match!')
            if len(fdata) < len(data):
                data = data[:len(fdata),:]
            data[:,c] = fdata[:len(data),c]
    # write merged channels:
    wave = wave.open(outfile, "w")
    wave.setparams((data.shape[1], sampwidth, rate, len(data),
                    "NONE", "not compressed"))
    wave.writeframesraw(data.reshape(-1, 1).tobytes())
    wave.close()

    
        
