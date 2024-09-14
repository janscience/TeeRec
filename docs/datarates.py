import numpy as np


sampling_rates = [8, 16, 24, 48, 96, 192]
channels = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
bits = [16, 24, 32]


def format_bytes(n):
    conv = [[1e15, 'PB'],
            [1e12, 'TB'],
            [ 1e9, 'GB'],
            [ 1e6, 'MB'],
            [ 1e3, 'kB'],
            [ 1e0, 'B']
            ]
    for fac, unit in conv:
        if n >= fac:
            return f'{n/fac:.3g}{unit}'


def make_table(bits=16):
    first = True
    print(f'| {"channels":8s} | {"bits":4s} | {"sampling rate":13s} | {"data rate":9s} | {"per hour":8s} | {"per day":7s} |')
    print(f'| {":":->8s} | {":":->4s} | {":":->13s} | {":":->9s} | {":":->8s} | {":":->7s} |')
    for r in sampling_rates:
        if not first:
            print(f'| {"":8s} | {"":4s} | {"":13s} | {"":9s} | {"":8s} | {"":7s} |')
        else:
            first = False
        for c  in channels:
            data_rate = bits/8*c*r*1000
            data_rates = format_bytes(data_rate) + '/s'
            per_hour = format_bytes(data_rate*3600)
            per_day = format_bytes(data_rate*3600*24)
            print(f'| {c:8d} | {bits:4d} | {r:10d}kHz | {data_rates:>9s} | {per_hour:>8s} | {per_day:>7s} |')


for b in bits:
    print(f'## {b} bits')
    print()
    make_table(b)
    print()
    print()
