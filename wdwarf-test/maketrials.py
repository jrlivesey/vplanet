"""
This script creates and initializes 22 suites of vplanet simulations that check
the interpolations performed by wdwarf against the original BaSTI tracks.

"""


import os
import sys
import shutil


## BaSTI grid IDs
# (c07 -> Cassisi 2007 opacity model, b20 -> Blouin 2020 opacity model,
# DA -> H atmosphere, DB -> He atmosphere, mpoor -> [Fe/H] = 0,
# Z01 -> [Fe/H] = 0.01, etc.)
grid_list = [
    'c07DAmpoor',
    'c07DAZ006',
    'c07DAZ01',
    'c07DAZ02',
    'c07DAZ03',
    'c07DAZ04',
    'c07DBmpoor',
    'c07DBZ01',
    'c07DBZ02',
    'c07DBZ03',
    'c07DBZ04',
    'b20DAmpoor',
    'b20DAZ006',
    'b20DAZ01',
    'b20DAZ02',
    'b20DAZ03',
    'b20DAZ04',
    'b20DBmpoor',
    'b20DBZ01',
    'b20DBZ02',
    'b20DBZ03',
    'b20DBZ04',
]


## Masses to use for test cases (in solar masses)
mass_list = [
    0.55,
    0.59,
    0.63,
    0.67,
    0.71,
    0.75,
    0.79,
    0.83,
    0.87,
    0.91,
    0.95,
    0.99,
    1.03,
    1.07,
]


def write_bodyfile(name, mass, opacity_model, int_metallicity, he_atm):
    filename = os.path.join(os.getcwd(), name+'.in')
    text = [
        f'sName              {name}',
        f'saModules           wdwarf',
        f'dMass              {mass}',
        f'dRadius             -1.0',
        f'dAge                6.2e6',
        f'sWDModel            basti22',
        f'sOpacityModel      {opacity_model}',
        f'iMetallicityLevel  {int_metallicity}',
        f'bHeAtm             {he_atm}',
        f'saOutputOrder   Time -WDLuminosity WDTemperature -WDLXUV',
    ]
    with open(filename, 'w') as infile:
        infile.writelines([line+'\n' for line in text])
    return


# def write_vspacefile():
#     filename = os.path.join(os.getcwd(), 'vspace.in')
#     text = [
#         'srcfolder      ./'
#         'destfolder     ./'
#         'trialname      wdtest_'
#         'bodyfile       wd.in'
#         'dMass [0.54, 1.1, n10] mass'
#         'primaryfile    vpl.in'
#     ]
#     with open(filename, 'w') as infile:
#         infile.writelines([line+'\n' for line in text])
#     return


def write_vplfile(stop_time):
    cwd = os.getcwd()
    # bodyfiles = []
    # for f in os.listdir(cwd):
    #     if '.in' in f:
    #         bodyfiles.append(f)
    # str_bodyfiles = ' '.join(bodyfiles)
    str_bodyfiles = 'wd.in'
    filename = os.path.join(cwd, 'vpl.in')
    text = [
        f'sSystemName     wdtest',
        f'iVerbose        0',
        f'bOverwrite      1',
        f'saBodyFiles     {str_bodyfiles}',
        f'sUnitMass       solar',
        f'sUnitLength     aU',       
        f'sUnitTime       YEARS',
        f'sUnitAngle      d',
        f'sUnitTemp       K',
        f'bDoLog          1',
        f'iDigits         6',
        f'dMinValue       1e-10',
        f'bDoForward      1',
        f'dEta            0.01',
        f'dStopTime       {stop_time}',
        f'dOutputTime     1.0e6',
    ]
    with open(filename, 'w') as infile:
        infile.writelines([line+'\n' for line in text])
    return


def make_and_run_sim(grid_id, stop_time):
    ## Obtain WD properties
    opacity_model   = None
    int_metallicity = None
    he_atm          = None
    if grid_id[:3] == 'c07':
        opacity_model = 'cassisi07'
    elif grid_id[:3] == 'b20':
        opacity_model = 'blouin20'
    else:
        exit(1)
    if grid_id[3:5] == 'DA':
        he_atm = 0
    elif grid_id[3:5] == 'DB':
        he_atm = 1
    else:
        exit(1)
    if grid_id[6:] == 'poor':
        int_metallicity = 0
    elif grid_id[6:] == '006':
        int_metallicity = 1
    elif grid_id[6:] == '01':
        int_metallicity = 2
    elif grid_id[6:] == '02':
        int_metallicity = 3
    elif grid_id[6:] == '03':
        int_metallicity = 4
    elif grid_id[6:] == '04':
        int_metallicity = 5
    else:
        exit(1)
    
    ## Create input files
    cwd = os.getcwd()
    grid_path = os.path.join(cwd, grid_id)
    if os.path.exists(grid_path):
        shutil.rmtree(grid_path)
    os.mkdir(grid_path)
    os.chdir(grid_path)
    for ii, mass in enumerate(mass_list):
        name = '{:02d}'.format(ii)
        sim_path = os.path.join(os.getcwd(), name)
        if os.path.exists(sim_path):
            shutil.rmtree(sim_path)
        os.mkdir(sim_path)
        os.chdir(sim_path)
        write_bodyfile('wd', mass, opacity_model, int_metallicity, he_atm)
        write_vplfile(stop_time)
        os.system(os.path.join(os.getcwd(), '../../../bin/vplanet') + ' vpl.in')
        os.chdir('..')
        exit(0)

    ## Run simulations
    # os.system(os.path.join(os.getcwd(), '../../bin/vplanet') + ' vpl.in')
    # os.system('vspace vspace.in')
    # os.system('multiplanet -c 10 vspace.in')
    os.chdir('..')
    return


def main(stop_time='1.0e9'):
    for grid_id in grid_list:
        make_and_run_sim(grid_id, stop_time)
    return


if __name__ == '__main__':
    if len(sys.argv) > 1:
        main(sys.argv[1])
    else:
        main()
    