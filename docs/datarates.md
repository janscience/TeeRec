# Data rates

The following tables given an overview on expected data rates in
dependence on bit resolution, number of channels, and sampling rate.
Use these numbers for selecting appropriate storing capacities and
write speeds of SD cards.

Keep in mind, that data rates are also limited by the bus used to
access the SD card. The Teensy 4.1 builtin SD card slot is accessed
via an SDIO bus that writes 4 bits per clock cylce at once. Data
rates of more than 20MB/s are possible with this bus.

When using an external SD card slot that is accessed via SPI bus, then
only one bit per clock cycle is written. Consequently, the maximum
data rate goes down by a factor of four. In addition, because of bad
cables etc. the maximum clock rate might also need to go down to
ensure reliable data transmission. In the end, on a SPI driven SD
card, maximum data rates are not more than 5MHz/s.

In addition, bus speed is also limited by by CPU speed. Here are some
measurements with a SanDisk 512GB U1 SD card:

| CPU speed | write speed |
| --------: | ----------: |
|    600MHz |    18.4MB/s |
|    450MHz |    18.1MB/s |
|    150MHz |    17.7MB/s |
|     24MHz |     9.6MB/s |


## 16 bit resolution

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|        1 |   16 |          8kHz |    16kB/s |   57.6MB |  1.38GB |
|        1 |   16 |         16kHz |    32kB/s |    115MB |  2.76GB |
|        1 |   16 |         24kHz |    48kB/s |    173MB |  4.15GB |
|        1 |   16 |         48kHz |    96kB/s |    346MB |  8.29GB |
|        1 |   16 |         96kHz |   192kB/s |    691MB |  16.6GB |
|        1 |   16 |        192kHz |   384kB/s |   1.38GB |  33.2GB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|        2 |   16 |          8kHz |    32kB/s |    115MB |  2.76GB |
|        2 |   16 |         16kHz |    64kB/s |    230MB |  5.53GB |
|        2 |   16 |         24kHz |    96kB/s |    346MB |  8.29GB |
|        2 |   16 |         48kHz |   192kB/s |    691MB |  16.6GB |
|        2 |   16 |         96kHz |   384kB/s |   1.38GB |  33.2GB |
|        2 |   16 |        192kHz |   768kB/s |   2.76GB |  66.4GB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|        4 |   16 |          8kHz |    64kB/s |    230MB |  5.53GB |
|        4 |   16 |         16kHz |   128kB/s |    461MB |  11.1GB |
|        4 |   16 |         24kHz |   192kB/s |    691MB |  16.6GB |
|        4 |   16 |         48kHz |   384kB/s |   1.38GB |  33.2GB |
|        4 |   16 |         96kHz |   768kB/s |   2.76GB |  66.4GB |
|        4 |   16 |        192kHz |  1.54MB/s |   5.53GB |   133GB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|        8 |   16 |          8kHz |   128kB/s |    461MB |  11.1GB |
|        8 |   16 |         16kHz |   256kB/s |    922MB |  22.1GB |
|        8 |   16 |         24kHz |   384kB/s |   1.38GB |  33.2GB |
|        8 |   16 |         48kHz |   768kB/s |   2.76GB |  66.4GB |
|        8 |   16 |         96kHz |  1.54MB/s |   5.53GB |   133GB |
|        8 |   16 |        192kHz |  3.07MB/s |   11.1GB |   265GB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|       16 |   16 |          8kHz |   256kB/s |    922MB |  22.1GB |
|       16 |   16 |         16kHz |   512kB/s |   1.84GB |  44.2GB |
|       16 |   16 |         24kHz |   768kB/s |   2.76GB |  66.4GB |
|       16 |   16 |         48kHz |  1.54MB/s |   5.53GB |   133GB |
|       16 |   16 |         96kHz |  3.07MB/s |   11.1GB |   265GB |
|       16 |   16 |        192kHz |  6.14MB/s |   22.1GB |   531GB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|       32 |   16 |          8kHz |   512kB/s |   1.84GB |  44.2GB |
|       32 |   16 |         16kHz |  1.02MB/s |   3.69GB |  88.5GB |
|       32 |   16 |         24kHz |  1.54MB/s |   5.53GB |   133GB |
|       32 |   16 |         48kHz |  3.07MB/s |   11.1GB |   265GB |
|       32 |   16 |         96kHz |  6.14MB/s |   22.1GB |   531GB |
|       32 |   16 |        192kHz |  12.3MB/s |   44.2GB |  1.06TB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|       64 |   16 |          8kHz |  1.02MB/s |   3.69GB |  88.5GB |
|       64 |   16 |         16kHz |  2.05MB/s |   7.37GB |   177GB |
|       64 |   16 |         24kHz |  3.07MB/s |   11.1GB |   265GB |
|       64 |   16 |         48kHz |  6.14MB/s |   22.1GB |   531GB |
|       64 |   16 |         96kHz |  12.3MB/s |   44.2GB |  1.06TB |
|       64 |   16 |        192kHz |  24.6MB/s |   88.5GB |  2.12TB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|      128 |   16 |          8kHz |  2.05MB/s |   7.37GB |   177GB |
|      128 |   16 |         16kHz |   4.1MB/s |   14.7GB |   354GB |
|      128 |   16 |         24kHz |  6.14MB/s |   22.1GB |   531GB |
|      128 |   16 |         48kHz |  12.3MB/s |   44.2GB |  1.06TB |
|      128 |   16 |         96kHz |  24.6MB/s |   88.5GB |  2.12TB |
|      128 |   16 |        192kHz |  49.2MB/s |    177GB |  4.25TB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|      256 |   16 |          8kHz |   4.1MB/s |   14.7GB |   354GB |
|      256 |   16 |         16kHz |  8.19MB/s |   29.5GB |   708GB |
|      256 |   16 |         24kHz |  12.3MB/s |   44.2GB |  1.06TB |
|      256 |   16 |         48kHz |  24.6MB/s |   88.5GB |  2.12TB |
|      256 |   16 |         96kHz |  49.2MB/s |    177GB |  4.25TB |
|      256 |   16 |        192kHz |  98.3MB/s |    354GB |  8.49TB |


## 24 bit resolution

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|        1 |   24 |          8kHz |    24kB/s |   86.4MB |  2.07GB |
|        1 |   24 |         16kHz |    48kB/s |    173MB |  4.15GB |
|        1 |   24 |         24kHz |    72kB/s |    259MB |  6.22GB |
|        1 |   24 |         48kHz |   144kB/s |    518MB |  12.4GB |
|        1 |   24 |         96kHz |   288kB/s |   1.04GB |  24.9GB |
|        1 |   24 |        192kHz |   576kB/s |   2.07GB |  49.8GB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|        2 |   24 |          8kHz |    48kB/s |    173MB |  4.15GB |
|        2 |   24 |         16kHz |    96kB/s |    346MB |  8.29GB |
|        2 |   24 |         24kHz |   144kB/s |    518MB |  12.4GB |
|        2 |   24 |         48kHz |   288kB/s |   1.04GB |  24.9GB |
|        2 |   24 |         96kHz |   576kB/s |   2.07GB |  49.8GB |
|        2 |   24 |        192kHz |  1.15MB/s |   4.15GB |  99.5GB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|        4 |   24 |          8kHz |    96kB/s |    346MB |  8.29GB |
|        4 |   24 |         16kHz |   192kB/s |    691MB |  16.6GB |
|        4 |   24 |         24kHz |   288kB/s |   1.04GB |  24.9GB |
|        4 |   24 |         48kHz |   576kB/s |   2.07GB |  49.8GB |
|        4 |   24 |         96kHz |  1.15MB/s |   4.15GB |  99.5GB |
|        4 |   24 |        192kHz |   2.3MB/s |   8.29GB |   199GB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|        8 |   24 |          8kHz |   192kB/s |    691MB |  16.6GB |
|        8 |   24 |         16kHz |   384kB/s |   1.38GB |  33.2GB |
|        8 |   24 |         24kHz |   576kB/s |   2.07GB |  49.8GB |
|        8 |   24 |         48kHz |  1.15MB/s |   4.15GB |  99.5GB |
|        8 |   24 |         96kHz |   2.3MB/s |   8.29GB |   199GB |
|        8 |   24 |        192kHz |  4.61MB/s |   16.6GB |   398GB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|       16 |   24 |          8kHz |   384kB/s |   1.38GB |  33.2GB |
|       16 |   24 |         16kHz |   768kB/s |   2.76GB |  66.4GB |
|       16 |   24 |         24kHz |  1.15MB/s |   4.15GB |  99.5GB |
|       16 |   24 |         48kHz |   2.3MB/s |   8.29GB |   199GB |
|       16 |   24 |         96kHz |  4.61MB/s |   16.6GB |   398GB |
|       16 |   24 |        192kHz |  9.22MB/s |   33.2GB |   796GB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|       32 |   24 |          8kHz |   768kB/s |   2.76GB |  66.4GB |
|       32 |   24 |         16kHz |  1.54MB/s |   5.53GB |   133GB |
|       32 |   24 |         24kHz |   2.3MB/s |   8.29GB |   199GB |
|       32 |   24 |         48kHz |  4.61MB/s |   16.6GB |   398GB |
|       32 |   24 |         96kHz |  9.22MB/s |   33.2GB |   796GB |
|       32 |   24 |        192kHz |  18.4MB/s |   66.4GB |  1.59TB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|       64 |   24 |          8kHz |  1.54MB/s |   5.53GB |   133GB |
|       64 |   24 |         16kHz |  3.07MB/s |   11.1GB |   265GB |
|       64 |   24 |         24kHz |  4.61MB/s |   16.6GB |   398GB |
|       64 |   24 |         48kHz |  9.22MB/s |   33.2GB |   796GB |
|       64 |   24 |         96kHz |  18.4MB/s |   66.4GB |  1.59TB |
|       64 |   24 |        192kHz |  36.9MB/s |    133GB |  3.19TB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|      128 |   24 |          8kHz |  3.07MB/s |   11.1GB |   265GB |
|      128 |   24 |         16kHz |  6.14MB/s |   22.1GB |   531GB |
|      128 |   24 |         24kHz |  9.22MB/s |   33.2GB |   796GB |
|      128 |   24 |         48kHz |  18.4MB/s |   66.4GB |  1.59TB |
|      128 |   24 |         96kHz |  36.9MB/s |    133GB |  3.19TB |
|      128 |   24 |        192kHz |  73.7MB/s |    265GB |  6.37TB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|      256 |   24 |          8kHz |  6.14MB/s |   22.1GB |   531GB |
|      256 |   24 |         16kHz |  12.3MB/s |   44.2GB |  1.06TB |
|      256 |   24 |         24kHz |  18.4MB/s |   66.4GB |  1.59TB |
|      256 |   24 |         48kHz |  36.9MB/s |    133GB |  3.19TB |
|      256 |   24 |         96kHz |  73.7MB/s |    265GB |  6.37TB |
|      256 |   24 |        192kHz |   147MB/s |    531GB |  12.7TB |


## 32 bit resolution

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|        1 |   32 |          8kHz |    32kB/s |    115MB |  2.76GB |
|        1 |   32 |         16kHz |    64kB/s |    230MB |  5.53GB |
|        1 |   32 |         24kHz |    96kB/s |    346MB |  8.29GB |
|        1 |   32 |         48kHz |   192kB/s |    691MB |  16.6GB |
|        1 |   32 |         96kHz |   384kB/s |   1.38GB |  33.2GB |
|        1 |   32 |        192kHz |   768kB/s |   2.76GB |  66.4GB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|        2 |   32 |          8kHz |    64kB/s |    230MB |  5.53GB |
|        2 |   32 |         16kHz |   128kB/s |    461MB |  11.1GB |
|        2 |   32 |         24kHz |   192kB/s |    691MB |  16.6GB |
|        2 |   32 |         48kHz |   384kB/s |   1.38GB |  33.2GB |
|        2 |   32 |         96kHz |   768kB/s |   2.76GB |  66.4GB |
|        2 |   32 |        192kHz |  1.54MB/s |   5.53GB |   133GB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|        4 |   32 |          8kHz |   128kB/s |    461MB |  11.1GB |
|        4 |   32 |         16kHz |   256kB/s |    922MB |  22.1GB |
|        4 |   32 |         24kHz |   384kB/s |   1.38GB |  33.2GB |
|        4 |   32 |         48kHz |   768kB/s |   2.76GB |  66.4GB |
|        4 |   32 |         96kHz |  1.54MB/s |   5.53GB |   133GB |
|        4 |   32 |        192kHz |  3.07MB/s |   11.1GB |   265GB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|        8 |   32 |          8kHz |   256kB/s |    922MB |  22.1GB |
|        8 |   32 |         16kHz |   512kB/s |   1.84GB |  44.2GB |
|        8 |   32 |         24kHz |   768kB/s |   2.76GB |  66.4GB |
|        8 |   32 |         48kHz |  1.54MB/s |   5.53GB |   133GB |
|        8 |   32 |         96kHz |  3.07MB/s |   11.1GB |   265GB |
|        8 |   32 |        192kHz |  6.14MB/s |   22.1GB |   531GB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|       16 |   32 |          8kHz |   512kB/s |   1.84GB |  44.2GB |
|       16 |   32 |         16kHz |  1.02MB/s |   3.69GB |  88.5GB |
|       16 |   32 |         24kHz |  1.54MB/s |   5.53GB |   133GB |
|       16 |   32 |         48kHz |  3.07MB/s |   11.1GB |   265GB |
|       16 |   32 |         96kHz |  6.14MB/s |   22.1GB |   531GB |
|       16 |   32 |        192kHz |  12.3MB/s |   44.2GB |  1.06TB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|       32 |   32 |          8kHz |  1.02MB/s |   3.69GB |  88.5GB |
|       32 |   32 |         16kHz |  2.05MB/s |   7.37GB |   177GB |
|       32 |   32 |         24kHz |  3.07MB/s |   11.1GB |   265GB |
|       32 |   32 |         48kHz |  6.14MB/s |   22.1GB |   531GB |
|       32 |   32 |         96kHz |  12.3MB/s |   44.2GB |  1.06TB |
|       32 |   32 |        192kHz |  24.6MB/s |   88.5GB |  2.12TB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|       64 |   32 |          8kHz |  2.05MB/s |   7.37GB |   177GB |
|       64 |   32 |         16kHz |   4.1MB/s |   14.7GB |   354GB |
|       64 |   32 |         24kHz |  6.14MB/s |   22.1GB |   531GB |
|       64 |   32 |         48kHz |  12.3MB/s |   44.2GB |  1.06TB |
|       64 |   32 |         96kHz |  24.6MB/s |   88.5GB |  2.12TB |
|       64 |   32 |        192kHz |  49.2MB/s |    177GB |  4.25TB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|      128 |   32 |          8kHz |   4.1MB/s |   14.7GB |   354GB |
|      128 |   32 |         16kHz |  8.19MB/s |   29.5GB |   708GB |
|      128 |   32 |         24kHz |  12.3MB/s |   44.2GB |  1.06TB |
|      128 |   32 |         48kHz |  24.6MB/s |   88.5GB |  2.12TB |
|      128 |   32 |         96kHz |  49.2MB/s |    177GB |  4.25TB |
|      128 |   32 |        192kHz |  98.3MB/s |    354GB |  8.49TB |

| channels | bits | sampling rate | data rate | per hour | per day |
| -------: | ---: | ------------: | --------: | -------: | ------: |
|      256 |   32 |          8kHz |  8.19MB/s |   29.5GB |   708GB |
|      256 |   32 |         16kHz |  16.4MB/s |     59GB |  1.42TB |
|      256 |   32 |         24kHz |  24.6MB/s |   88.5GB |  2.12TB |
|      256 |   32 |         48kHz |  49.2MB/s |    177GB |  4.25TB |
|      256 |   32 |         96kHz |  98.3MB/s |    354GB |  8.49TB |
|      256 |   32 |        192kHz |   197MB/s |    708GB |    17TB |



## SD card capacities


### 8kHz sampling rate

| capacity |   1 channels |   2 channels |   4 channels |   8 channels |  16 channels |  32 channels |  64 channels | 128 channels | 256 channels |
| -------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: |
|    128GB |     92.6days |     46.3days |     23.1days |     11.6days |      5.8days |      2.9days |      1.4days |        17.4h |         8.7h |
|    256GB |    185.2days |     92.6days |     46.3days |     23.1days |     11.6days |      5.8days |      2.9days |      1.4days |        17.4h |
|    512GB |    370.4days |    185.2days |     92.6days |     46.3days |     23.1days |     11.6days |      5.8days |      2.9days |      1.4days |
|      1TB |    740.7days |    370.4days |    185.2days |     92.6days |     46.3days |     23.1days |     11.6days |      5.8days |      2.9days |

### 16kHz sampling rate

| capacity |   1 channels |   2 channels |   4 channels |   8 channels |  16 channels |  32 channels |  64 channels | 128 channels | 256 channels |
| -------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: |
|    128GB |     46.3days |     23.1days |     11.6days |      5.8days |      2.9days |      1.4days |        17.4h |         8.7h |         4.3h |
|    256GB |     92.6days |     46.3days |     23.1days |     11.6days |      5.8days |      2.9days |      1.4days |        17.4h |         8.7h |
|    512GB |    185.2days |     92.6days |     46.3days |     23.1days |     11.6days |      5.8days |      2.9days |      1.4days |        17.4h |
|      1TB |    370.4days |    185.2days |     92.6days |     46.3days |     23.1days |     11.6days |      5.8days |      2.9days |      1.4days |

### 24kHz sampling rate

| capacity |   1 channels |   2 channels |   4 channels |   8 channels |  16 channels |  32 channels |  64 channels | 128 channels | 256 channels |
| -------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: |
|    128GB |     30.9days |     15.4days |      7.7days |      3.9days |      1.9days |        23.1h |        11.6h |         5.8h |         2.9h |
|    256GB |     61.7days |     30.9days |     15.4days |      7.7days |      3.9days |      1.9days |        23.1h |        11.6h |         5.8h |
|    512GB |    123.5days |     61.7days |     30.9days |     15.4days |      7.7days |      3.9days |      1.9days |        23.1h |        11.6h |
|      1TB |    246.9days |    123.5days |     61.7days |     30.9days |     15.4days |      7.7days |      3.9days |      1.9days |        23.1h |

### 48kHz sampling rate

| capacity |   1 channels |   2 channels |   4 channels |   8 channels |  16 channels |  32 channels |  64 channels | 128 channels | 256 channels |
| -------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: |
|    128GB |     15.4days |      7.7days |      3.9days |      1.9days |        23.1h |        11.6h |         5.8h |         2.9h |         1.4h |
|    256GB |     30.9days |     15.4days |      7.7days |      3.9days |      1.9days |        23.1h |        11.6h |         5.8h |         2.9h |
|    512GB |     61.7days |     30.9days |     15.4days |      7.7days |      3.9days |      1.9days |        23.1h |        11.6h |         5.8h |
|      1TB |    123.5days |     61.7days |     30.9days |     15.4days |      7.7days |      3.9days |      1.9days |        23.1h |        11.6h |

### 96kHz sampling rate

| capacity |   1 channels |   2 channels |   4 channels |   8 channels |  16 channels |  32 channels |  64 channels | 128 channels | 256 channels |
| -------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: |
|    128GB |      7.7days |      3.9days |      1.9days |        23.1h |        11.6h |         5.8h |         2.9h |         1.4h |      43.4min |
|    256GB |     15.4days |      7.7days |      3.9days |      1.9days |        23.1h |        11.6h |         5.8h |         2.9h |         1.4h |
|    512GB |     30.9days |     15.4days |      7.7days |      3.9days |      1.9days |        23.1h |        11.6h |         5.8h |         2.9h |
|      1TB |     61.7days |     30.9days |     15.4days |      7.7days |      3.9days |      1.9days |        23.1h |        11.6h |         5.8h |

### 192kHz sampling rate

| capacity |   1 channels |   2 channels |   4 channels |   8 channels |  16 channels |  32 channels |  64 channels | 128 channels | 256 channels |
| -------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: | -----------: |
|    128GB |      3.9days |      1.9days |        23.1h |        11.6h |         5.8h |         2.9h |         1.4h |      43.4min |      21.7min |
|    256GB |      7.7days |      3.9days |      1.9days |        23.1h |        11.6h |         5.8h |         2.9h |         1.4h |      43.4min |
|    512GB |     15.4days |      7.7days |      3.9days |      1.9days |        23.1h |        11.6h |         5.8h |         2.9h |         1.4h |
|      1TB |     30.9days |     15.4days |      7.7days |      3.9days |      1.9days |        23.1h |        11.6h |         5.8h |         2.9h |


## SD card prices

| capacity |  costs |
| -------: | -----: |
| 128GB    |  14 €  |
| 256GB    |  25 €  |
| 512GB    |  46 €  |
|   1TB    | 100 €  |

(prices from 2024 for U1 speed class)

In most configurations of sampling rates and channel counts, data
rates are not that high (<10MB/s). Class 10 cards (min 10MB/s) or
UHS-I cards with U1 speed class (10MB/s) should be sufficient. See
column "data rate" in the tables above.


