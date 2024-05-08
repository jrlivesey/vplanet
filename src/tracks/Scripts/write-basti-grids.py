"""
Joseph R. Livesey, 2023

This script reads in the BaSTI stellar evolution tracks and linearly
interpolates them along the mass and age axes to obtain the rectilinear grid
used in wdwarf.h. It also extrapolates to make tracks just beyond the grids, so that
the bicubic interpolations made in wdwarf.c are numerically stable.

This script is based havily on `Baraffe2015RGInterp.py`, the equivalent tool made
for the STELLAR module by David Fleming.
"""



import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import pathlib
from astropy import (units as u, constants as const)
from scipy.integrate import quad
from scipy.interpolate import interp1d



class EvolGrid:

    def __init__(self, opacity_model, atmosphere, metallicity, tracks):
        self.opacity_model = opacity_model
        self.atmosphere = atmosphere
        self.metallicity = metallicity
        self.tracks = tracks
        self.masses = list(tracks.drop_duplicates(subset='Mwd')['Mwd'])
        self.masses.sort()
        self.mlen = len(self.masses)



class NameFormatError(Exception):

    def __init__(self, dirname, quality):
        self.message = 'ERROR: Unable to retrieve %s from grid name \"%s\"' \
                        % (quality, dirname)

    def __str__(self):
        return repr(self.message)



# def blackbody_spectrum(freq, temp):
#     temp *= u.K
#     freq *= u.Hz
#     spec = 2. * const.h * freq ** 3 / (const.c ** 2) \
#             * 1./(np.exp(const.h * freq / (const.k_B * temp)) - 1.)
#     return spec / spec.unit



# def xuv_frac(temp):
#     lbol, _ = quad(lambda v: blackbody_spectrum(v, temp), 0.0, 1.0e+50,
#                    points=[1.0e+13, 2.0e+15])
#     lxuv, _ = quad(lambda v: blackbody_spectrum(v, temp), 2.48e+15, 3.0e+16)
#     fxuv = lxuv / lbol
#     return fxuv



def define_new_grid(old_grid):
    global mlen, alen
    global marr, aarr
    
    # Lengths of arrays
    mlen, alen = (old_grid.mlen, 500)

    # Grid masses in solar units
    if mlen == 7:
        marr = [
            0.53, # <- This is a dummy mass for extrapolation
            0.54,
            0.61,
            0.68,
            0.77,
            0.87,
            1.00,
            1.10,
            1.11, # <- This is a dummy mass for extrapolation
        ]

    elif mlen == 4:
        marr = [
            0.76, # <- This is a dummy mass for extrapolation
            0.77,
            0.87,
            1.00,
            1.10,
            1.11, # <- This is a dummy mass for extrapolation
        ]

    elif mlen == 3:
        marr = [
            0.86, # <- This is a dummy mass for extrapolation
            0.87,
            1.00,
            1.10,
            1.11, # <- This is a dummy mass for extrapolation
        ]

    elif mlen == 2:
        marr = [
            0.99, # <- This is a dummy mass for extrapolation
            1.00,
            1.10,
            1.11, # <- This is a dummy mass for extrapolation
        ]

    else:
        raise FileNotFoundError()
    
    mlen += 2

    # Grid ages in Gyr
    aarr = np.logspace(6.1, 9.8, 500) / 1.e9



def get_raw_data(subdir):
    """
    Reads in raw data from the BaSTI WD evolution tracks (for a given electron
    opacity model, metallicity, and atmosphere type). Returns an EvolGrid
    object with the tracks for this type of WD.
    """
    opacity_model = None
    atmosphere    = None
    metallicity   = None
    data          = pd.DataFrame()

    # Parse file name
    gridname = subdir.name

    try:
        om = gridname[:3]
        if om == 'b20':
            opacity_model = 'blouin20'
        elif om == 'c07':
            opacity_model = 'cassisi07'
        else:
            raise NameFormatError(subdir, 'opacity model')

        atm = gridname[3:5]
        if atm == 'DA':
            atmosphere = 'H'
        elif atm == 'DB':
            atmosphere = 'He'
        else:
            raise NameFormatError(subdir, 'atmosphere type')

        z = gridname[6:8]
        if z == 'po':
            metallicity = 0.
        elif z == '00':
            metallicity =  0.006
        elif z == '01':
            metallicity = 0.01
        elif z == '02':
            metallicity = 0.02
        elif z == '03':
            metallicity = 0.03
        elif z == '04':
            metallicity = 0.04
        else:
            raise NameFormatError(subdir, 'metallicity')
        
    except NameFormatError as err:
        print(err)
        exit(1)

    for file in subdir.iterdir():
        if str(file.name)[0] != '.':
            # Retrieve data
            df = pd.read_table(
                file,
                comment="#",
                sep='\s+',
                header=None,
                names=[
                    'log(t)',
                    'Mwd',
                    'log(Teff)',
                    'log(L/Lo)',
                    'F435W',
                    'F475',
                    'F502N',
                    'F550M',
                    'F555W',
                    'F606W',
                    'F625W',
                    'F658N',
                    'F660N',
                    'F775W',
                    'F814W',
                    'F850LP',
                    'F892N',
                ],
                encoding='ascii',
            )

            # Add to DataFrame
            if not data.empty:
                data = pd.concat([data, df])
            else:
                data = df
        
        else:
            pass
    
    evol_grid = EvolGrid(opacity_model, atmosphere, metallicity, data)

    return evol_grid



def plot_tracks(quantity, data, age, orig, interp):
    groups = data.groupby('Mwd').groups

    _, ax = plt.subplots()
    for i, group in enumerate(groups):
        if group >= np.min(marr):
            ax.plot(
                aarr*1.e9,
                10. ** interp[str(group)],
                label=str(group),
                color='C%d' % (i % 9),
                lw=2,
            )
            ax.scatter(
                10. ** age[str(group)],
                10. ** np.array([float(o) for o in orig[str(group)]]),
                label=str(group),
                color='C%d' % (i % 9),
                s=20,
            )
    ax.set_xlabel('Age [yr]')
    ax.set_xscale('log')
    ax.set_yscale('log')
    ax.legend(loc=0)
    if quantity == 'L':
        ax.set_ylabel(r'Luminosity [$L_{\odot}$]')
        ax.set_title('Luminosity Interpolation')
        plt.show()
    elif quantity == 'T':
        ax.set_ylabel('Temperature [K]')
        ax.set_title('Temperature Interpolation')
        plt.show()



def write_track(interp):
    for val in interp:
        if np.isnan(val):
            print('NAN, ', end='')
        else:
            print('%.6lf, ' % val, end='')
    print()



def write_grid(quantity, data, interp):
    groups = data.groupby('Mwd').groups
    
    if quantity == 'L':
        print()
        print('\n Luminosity \n')

    elif quantity == 'T':
        print()
        print('\n Effective Temperature \n')
    
    elif quantity == 'f':
        print()
        print('\n XUV frac \n')

    else:
        print('Invalid quantity')
        exit(1)
    
    print('{', end='')
    write_track(interp[str(marr[0])])
    for group in groups:
        if group >= np.min(marr):
            write_track(interp[str(group)])
    write_track(interp[str(marr[-1])])
    print('}', end='')



def linear_interpolate_and_extrapolate(subdir):
    grid = get_raw_data(subdir)
    data = grid.tracks

    # Create new grid with standardized points in mass-age space
    define_new_grid(grid)

    # Group by masses (extract individual evolutionary tracks)
    groups = data.groupby('Mwd').groups

    # Dicts to hold new quantities
    basti_age  = {}
    basti_lum  = {}
    basti_teff = {}

    basti_lum_interp_funcs  = {}
    basti_teff_interp_funcs = {}

    basti_lum_interp  = {}
    basti_teff_interp = {}

    for group in groups:
        # Indices of rows in DataFrame corresponding to a single track
        inds = np.array(groups[group])
        data_m = data.loc[data['log(t)'] != 0.0]
        data_m = data_m.loc[data_m['Mwd'] == group]

        # Extract data
        basti_age[str(group)]  = data_m['log(t)'].to_numpy()
        basti_lum[str(group)]  = data_m['log(L/Lo)'].to_numpy()
        basti_teff[str(group)] = data_m['log(Teff)'].to_numpy()
        # print(basti_age[str(group)].shape)
        # basti_age[str(group)]  = data.values[inds, 0]
        # basti_lum[str(group)]  = data.values[inds, 3]
        # basti_teff[str(group)] = data.values[inds, 2]

        # Build linear interpolation functions
        try:
            basti_lum_interp_funcs[str(group)] = interp1d(
                basti_age[str(group)],
                basti_lum[str(group)],
                kind='linear',
                fill_value=np.nan,
                assume_sorted=True,
                bounds_error=False,
            )

            basti_teff_interp_funcs[str(group)] = interp1d(
                basti_age[str(group)],
                basti_teff[str(group)],
                kind='linear',
                fill_value=np.nan,
                assume_sorted=True,
                bounds_error=False,
            )

        except ValueError as verr:
            print(verr)
            print(subdir)
            print(group)
            exit(0)

        basti_lum_interp[str(group)] = basti_lum_interp_funcs[str(group)](
            np.log10(aarr * 1.e9)
        )

        basti_teff_interp[str(group)] = basti_teff_interp_funcs[str(group)](
            np.log10(aarr * 1.e9)
        )

    # Sanity-check plots
    plot_tracks('L', data, basti_age, basti_lum, basti_lum_interp)
    plot_tracks('T', data, basti_age, basti_teff, basti_teff_interp)

    # Lower mass limit depends on the grid
    int_idx = str(marr[0])
    snd_idx = str(marr[1])
    fst_idx = str(marr[2])

    int_mass = marr[0]
    snd_mass = marr[1]
    fst_mass = marr[2]

    # Extrapolating luminosity just beyond BaSTI grid
    fst = basti_lum_interp['1.0']
    snd = basti_lum_interp['1.1']
    basti_lum_interp['1.11'] = snd + (snd - fst) / (1.1 - 1.0) * (1.11 - 1.1)

    fst = basti_lum_interp[fst_idx]
    snd = basti_lum_interp[snd_idx]
    basti_lum_interp[int_idx] = snd + (snd - fst) / (snd_mass - fst_mass) \
                                * (int_mass - snd_mass)

    # Extrapolating temperature just beyond BaSTI grid
    fst = basti_teff_interp['1.0']
    snd = basti_teff_interp['1.1']
    basti_teff_interp['1.11'] = snd + (snd - fst) / (1.1 - 1.0) * (1.11 - 1.1)

    fst = basti_teff_interp[fst_idx]
    snd = basti_teff_interp[snd_idx]
    basti_teff_interp[int_idx] = snd + (snd - fst) / (snd_mass - fst_mass) \
                                * (int_mass - snd_mass)
    

    # # debuggin
    # fig, axes = plt.subplots()
    # ax = axes
    # for mval in groups:
    #     age_data = basti_age[str(mval)]

    #     # print(basti_lum[str(mval)])
    #     # for _ in range(50): print()
    #     # print(basti_lum_interp[str(mval)])
    #     # exit(0)

    #     ax.scatter(age_data, basti_lum[str(mval)], c='b', s=5)
    #     ax.scatter(np.log10(aarr * 1.e9), basti_lum_interp[str(mval)], c='r', s=2)
    #     # axes[0].scatter(age_data, mval*np.ones_like(age_data), c=basti_lum[str(mval)], s=5)
    #     # axes[1].scatter(aarr, mval*np.ones_like(aarr), c=basti_lum_interp[str(mval)], s=2)
    #     # cb = plt.colorbar()
    #     break
    # plt.show(); exit(0)


    
    # # Calculate XUV fraction from temperature
    # xuv_fracs = xuv_frac(basti_teff_interp)

    # Write each grid in vplanet-readable format
    for _ in range(10): print()
    print(f'Opacity model: {grid.opacity_model}; atmosphere: {grid.atmosphere}; \
          Z = {grid.metallicity}')
    write_grid('L', data, basti_lum_interp)
    write_grid('T', data, basti_teff_interp)
    # write_grid('f', data, xuv_fracs)



def main():
    grids = []
    path = pathlib.Path()
    grids_path = path / 'tracks/Data/basti-grids'
    for subdir in grids_path.iterdir():
        # print(subdir)
        if subdir.is_dir():
            linear_interpolate_and_extrapolate(subdir)



if __name__ == '__main__':
    main()
