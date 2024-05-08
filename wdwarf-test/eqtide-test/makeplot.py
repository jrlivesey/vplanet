import numpy as np
import pandas as pd
import matplotlib as mpl
import matplotlib.pyplot as plt
from seaborn import color_palette


mpl.rcParams['text.usetex'] = True
mpl.rcParams['font.family'] = 'cmu-serif'
palette = color_palette('colorblind', 10)


wd = pd.read_table(
    'wdtides.wd.forward',
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

planet = pd.read_table(
    'wdtides.planet.forward',
    sep='\s+',
    header=None,
    names = [
        'time',
        'semimajoraxis',
        'eccentricity',
        'longp',
        'rotper',
        'surfenfluxeqtide',
        'surfenfluxtot'
    ],
    encoding='ascii'
)


time                  = wd['time'].to_numpy()
luminosity            = wd['luminosity'].to_numpy()
temperature           = wd['temperature'].to_numpy()
hz_lim_rec_venus      = wd['hzlimrecvenus'].to_numpy()
hz_lim_runaway        = wd['hzlimrunaway'].to_numpy()
hz_lim_max_greenhouse = wd['hzlimmaxgreenhouse'].to_numpy()
hz_lim_early_mars     = wd['hzlimearlymars'].to_numpy()

sma                 = planet['semimajoraxis'].to_numpy()
eccentricity        = planet['eccentricity'].to_numpy()
long_pericenter     = planet['longp'].to_numpy()
rotation_period     = planet['rotper'].to_numpy()
surf_en_flux_eqtide = planet['surfenfluxeqtide'].to_numpy()
surf_en_flux_total  = planet['surfenfluxtot'].to_numpy()

time0 = 1.e5
# surf_en_flux_eqtide[surf_en_flux_eqtide < 1.e-5] = np.nan
surf_en_flux_eqtide[np.where(time < time0)] = np.nan


fig, axes = plt.subplots(1, 3, figsize=(10, 3))
axes[0].plot(time, eccentricity, c=palette[2])
axes[0].set_ylabel('Eccentricity')
axes[0].set_xscale('log')

# axes[1].fill_between(time, hz_lim_rec_venus, hz_lim_early_mars,
#                      color=palette[9], label='optimistic HZ')
# axes[1].fill_between(time, hz_lim_runaway, hz_lim_max_greenhouse,
#                      color=palette[0], label='conservative HZ')
axes[1].plot(time, sma, c='k', label='semi-major axis')
axes[1].set_ylabel('Orbital distance (au)')
axes[1].set_xscale('log')
# axes[1].set_yscale('log')
axes[1].legend(loc='center left')

axes[2].plot(time, surf_en_flux_eqtide, c=palette[1])
axes[2].set_ylabel(r'Surface tidal heat flux (W m$^{-2}$)')
# axes[2].set_ylim(top=2.e5)
axes[2].set_xscale('log')
# axes[2].set_yscale('log')

for ax in axes:
    ax.set_xlim(time0, time.max())
    ax.set_xlabel('Simulation time (yr)')

fig.tight_layout()
fig.savefig('eqtide+wdwarf.pdf')
