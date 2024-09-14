import numpy as np


sampling_rates = [8, 16, 24, 48, 96, 192] # kHz
channels = [1, 2, 4, 8, 16, 32, 64, 128, 256]
bits = [16, 24, 32]
capacities = [128, 256, 512, 1024] # GB


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


def rate_table(bits=16):
    for c  in channels:
        print(f'| {"channels":8s} | {"bits":4s} | {"sampling rate":13s} | {"data rate":9s} | {"per hour":8s} | {"per day":7s} |')
        print(f'| {":":->8s} | {":":->4s} | {":":->13s} | {":":->9s} | {":":->8s} | {":":->7s} |')
        for r in sampling_rates:
            data_rate = bits/8*c*r*1000
            data_rates = format_bytes(data_rate) + '/s'
            per_hour = format_bytes(data_rate*3600)
            per_day = format_bytes(data_rate*3600*24)
            print(f'| {c:8d} | {bits:4d} | {r:10d}kHz | {data_rates:>9s} | {per_hour:>8s} | {per_day:>7s} |')
        print()


def format_capacity(giga_byte):
    conv = [[ 1024, 'TB'],
            [ 1, 'GB']
            ]
    for fac, unit in conv:
        if giga_byte >= fac:
            return f'{giga_byte/fac:.0f}{unit}'


def format_seconds(secs):
    conv = [[ 3600*24, 'days'],
            [ 3600, 'h'],
            [ 60, 'min'],
            [ 1, 's']
            ]
    for fac, unit in conv:
        if secs >= fac:
            return f'{secs/fac:.1f}{unit}'


def days_table(bits=16):
    for r in sampling_rates:
        s = f'| {"capacity":8s} |'
        for c  in channels:
            cs = f'{c} channels @ {r}kHz'
            s += f' {cs:>21s} |'
        print(s)
        s = f'| {":":->8s} |'
        for c  in channels:
            s += f' {":":->21s} |'
        print(s)
        for p  in capacities:
            ps = format_capacity(p)
            s = f'| {ps:>8s} |'
            for c  in channels:
                data_rate = bits/8*c*r*1000
                days = format_seconds(p*1e9/data_rate)
                s += f' {days:>21s} |'
            print(s)
        print()


print('''# Data rates

The following tables given an overview on expected data rates in
dependence on bit resolution, number of channels, and sampling rate.
Use these numbers for selecting appropriate storing capacities and
write speeds of SD cards.

''')
for b in bits:
    print(f'## {b} bit resolution')
    print()
    rate_table(b)
    print()

print('''## SD card capacities

Data rates are mostly not that high (<10MB/s). Class 10 cards (min
10MB/s) or UHS-I cards with U1 speed class (10MB/s) should be
sufficient.

''')
days_table(16)
print()
print('''## SD card prices

| capacity |  costs |
| -------: | -----: |
| 128GB    |  14 €  |
| 256GB    |  25 €  |
| 512GB    |  46 €  |
|   1TB    | 100 €  |

(prices from 2024)
''')
