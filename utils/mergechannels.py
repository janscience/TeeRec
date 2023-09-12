import argparse
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
    # command line arguments:
    parser = argparse.ArgumentParser(add_help=True,
        description='Take from each provided wave file one channel and merge them into a single wav file. That is, take from the first file the first channel, from the second file the second channel and so on.')
        #epilog='version %s by Benda-Lab (2015-%s)' % (__version__, __year__))
    #parser.add_argument('--version', action='version', version=__version__)
    parser.add_argument('-v', dest='verbose', action='store_true', 
                        help='print on console what is going on')
    parser.add_argument('-o', dest='outfile', default='merged.wav', type=str,
                        help='output file with the merged channels',
                        metavar='OUTFILE')
    parser.add_argument('files', nargs='+', type=str,
                        help='wave files')
    args = parser.parse_args()
    # options:
    outfile = args.outfile
    verbose = args.verbose
    # read channels:
    data = None
    sampwidth = 0
    rate = 0
    if verbose:
        print('merge')
    for c in range(len(args.files)):
        fname = args.files[c]
        if verbose:
            print(f'  channel {c:2d} from {fname}')
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
    if len(args.files) < data.shape[1]:
        data = data[:,:len(args.files)]
    # write merged channels:
    wave = wave.open(outfile, "w")
    wave.setparams((data.shape[1], sampwidth, rate, len(data),
                    "NONE", "not compressed"))
    wave.writeframesraw(data.reshape(-1, 1).tobytes())
    wave.close()
    if verbose:
        print(f'into file {outfile}')

    
        
