import numpy as np
import pandas as pd
import matplotlib as mpl
import matplotlib.pyplot as plt
from seaborn import color_palette
import astropy.units as u
import astropy.constants as const


lw = 3
mpl.rcParams['text.usetex'] = True
mpl.rcParams['font.family'] = 'cmu-serif'
palette = color_palette('colorblind', 10)


def right_blackbody_integral(freq, temp):
    ## Series solution of the integral from v to +inf of the blackbody spectrum
    ## at temperature T. From Michels (1968).
    N = 100

    freq *= u.Hz
    wnum = freq / const.c # Convert to wavenumber
    c2 = const.h / const.k_B * const.c / temp
    
    def series_term(n):
        var = n * c2 * wnum
        poly = np.power(var, 3) + 3 * np.power(var, 2) + 6 * var + 6.0
        return poly * np.exp(-var) / np.power(n, 4) * 15.0 / np.power(np.pi, 4)

    result = sum([series_term(n+1) for n in range(N)])
    return result

def finite_blackbody_integral(lo, hi, temp):
    ## Integral of the blackbody spectrum between frequencies "lo" and "hi", in Hz.
    int1 = right_blackbody_integral(lo, temp)
    int2 = right_blackbody_integral(hi, temp)
    result = int1 - int2
    return result

def xuv_frac(temp):
    temp *= u.K
    return finite_blackbody_integral(2.498e+15, 2.998e+18, temp)


df = pd.read_table(
    'wdhz.wd.forward',
    sep='\s+',
    header=None,
    names = [
        'time',
        'luminosity',
        'temperature',
        'hzlimrecvenus',
        'hzlimrunaway',
        'hzlimmaxgreenhouse',
        'hzlimearlymars'
    ],
    encoding='ascii'
)


time                  = df['time'].to_numpy() / 1.e9 + 1.0 # starting age 1 Gyr
luminosity            = df['luminosity'].to_numpy() * 1.e4 # in 1e-4 * solar lum
temperature           = df['temperature'].to_numpy()
hz_lim_rec_venus      = df['hzlimrecvenus'].to_numpy()
hz_lim_runaway        = df['hzlimrunaway'].to_numpy()
hz_lim_max_greenhouse = df['hzlimmaxgreenhouse'].to_numpy()
hz_lim_early_mars     = df['hzlimearlymars'].to_numpy()
xuv_luminosity = xuv_frac(temperature) * luminosity * 1.e3 # in 1e-7 * solar lum


fig, axes = plt.subplots(3, 1, figsize=(4, 7))
axes[0].plot(time, luminosity, c=palette[1], lw=lw, label='total')
axes[0].plot(time, xuv_luminosity, c=palette[0], lw=lw, label=r'X-ray/extreme UV ($\times 10^3$)')
axes[0].set_ylabel(r'Luminosity ($10^{-4} L_\odot$)')
axes[0].legend()

axes[1].plot(time, temperature, c=palette[2], lw=lw)
axes[1].set_ylabel('Temperature (K)')

axes[2].fill_between(time, hz_lim_rec_venus, hz_lim_early_mars,
                     color=palette[9], label='optimistic HZ')
axes[2].fill_between(time, hz_lim_runaway, hz_lim_max_greenhouse,
                     color=palette[0], label='conservative HZ')
axes[2].set_ylabel('Orbital distance (au)')
axes[2].legend()

for ax in axes:
    ax.set_xlim(time.min(), time.max())
    ax.set_xlabel('Time since AGB (Gyr)')

fig.tight_layout()
fig.savefig('wdhz.pdf')
