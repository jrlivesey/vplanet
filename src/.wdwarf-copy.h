/**
    @file wdwarf.h
    @brief Subroutines that control the thermal evolution of a white dwarf
           primary.
    @author Joseph Livesey ([jrlivesey@wisc.edu](mailto:jrlivesey@wisc.edu>))
    @date 2023
*/

#define LSUN 3.846e26 // Solar luminosity (W)
#define TSUN 5778.    // Solar TEff (K)
#define MMWE 0.5      // Mean molecular weight per electron

#define WDWARF_MODEL_BASTI 1
#define WDWARF_MODEL_CONST 3

#define OPACITY_MODEL_BLOUIN 0
#define OPACITY_MODEL_CASSISI 1

#define WDWARF_T 1
#define WDWARF_L 2
#define WDWARF_ERR_LINEAR 1
#define WDWARF_ERR_NONE 0
#define WDWARF_ERR_OUTOFBOUNDS_LO -2
#define WDWARF_ERR_OUTOFBOUNDS_HI -3
#define WDWARF_ERR_ISNAN -4
#define WDWARF_ERR_FILE -6
#define WDWARF_ERR_BADORDER -7

/* Options */
#define OPTSTARTWDWARF 2400
#define OPTENDWDWARF 2500

#define OPT_WDMODEL 2412
#define OPT_OPACITYMODEL 2413
#define OPT_METALLICITY 2414
#define OPT_HEATMOSPHERE 2415
#define OPT_HALTENDBASTIGRID 2416

/* Outputs */
#define OUTSTARTWDWARF 2400
#define OUTENDWDWARF 2500

#define OUT_LUMINOSITYWDWARF 2410
#define OUT_TEMPERATUREWDWARF 2412

/* Read functions */
void BodyCopyWdwarf(BODY *, BODY *, int, int);
void ReadWDModel(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadOpacityModel(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadMetallicity(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void ReadHeAtm(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *, int);
void InitializeOptionsWdwarf(OPTIONS *, fnReadOption *);

/* Verify functions */
void VerifyLuminosityWdwarf(BODY *, CONTROL *, OPTIONS *, UPDATE *, double, int);
void VerifyTemperatureWdwarf(BODY *, CONTROL *, OPTIONS *, UPDATE *, double, int);
void VerifyRadiusWdwarf(BODY *, CONTROL *, OPTIONS *, UPDATE *, int);
void fnPropsAuxWdwarf();
void fnForceBehaviorWdwarf();
void AssignWdwarfDerivatives();
void NullWdwarfDerivatives();
void VerifyWdwarf(BODY *, CONTROL *, FILES *, OPTIONS *, OUTPUT *, SYSTEM *,
                  UPDATE *, int, int);
void ReadOptionsWdwarf(BODY *, CONTROL *, FILES *, OPTIONS *, SYSTEM *,
                       fnReadOption *, int);

/* Update functions */
void InitializeUpdateWdwarf(BODY *, UPDATE *, int);
void FinalizeUpdateLuminosityWdwarf(BODY *, UPDATE *, int *, int, int);
void FinalizeUpdateTemperatureWdwarf(BODY *, UPDATE *, int *, int, int);

void WriteLuminosity(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                     int, double *, char *);
void WriteTemperature(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UNITS *, UPDATE *,
                      int, double *, char *);
void InitializeOutputWdwarf(OUTPUT *, fnWriteOutput *);

void LogOptionsWdwarf(CONTROL *, FILE *);
void LogWdwarf(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
               fnWriteOutput *, FILE *);
void LogBodyWdwarf(BODY *, CONTROL *, OUTPUT *, SYSTEM *, UPDATE *,
                   fnWriteOutput *, FILE *, int);
void AddModuleWdwarf(CONTROL *, MODULE *, int, int);

/* WDWARF functions */
double fdRadiusWdwarf(BODY *, int);
double fdLuminosityWdwarf(BODY *, SYSTEM *, int *);
double fdTemperatureWdwarf(BODY *, SYSTEM *, int *);
double fdLuminosityFunctionBasti(int, int, int, double, double);
double fdTemperatureFunctionBasti(int, int, int, double, double);
double fdBasti(int, int, int, int, double, double, int, int *);

/* Data & data handling */
#define WDWARF_BASTI_ALEN 500

static double const WDWARF_BASTI_MARR_7[9] = {
    0.53, 0.54, 0.61, 0.68, 0.77, 0.87, 1.00, 1.10, 1.11
};
static double const WDWARF_BASTI_MARR_4[6] = {
    0.76, 0.77, 0.87, 1.00, 1.10, 1.11
};
static double const WDWARF_BASTI_MARR_3[5] = {
    0.86, 0.87, 1.00, 1.10, 1.11
};
static double const WDWARF_BASTI_MARR_2[4] = {
    0.99, 1.00, 1.10, 1.11
};
static double const WDWARF_BASTI_AARR[WDWARF_BASTI_ALEN] = {

};

/* Temperature grids */
static double const BASTI_BLOUIN_DA_Z000_LOGT[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DA_Z006_LOGT[6][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DA_Z010_LOGT[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DA_Z020_LOGT[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DA_Z030_LOGT[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DA_Z040_LOGT[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DB_Z000_LOGT[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DB_Z010_LOGT[4][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DB_Z020_LOGT[5][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DB_Z030_LOGT[6][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DB_Z040_LOGT[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DA_Z000_LOGT[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DA_Z006_LOGT[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DA_Z010_LOGT[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DA_Z020_LOGT[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DA_Z030_LOGT[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DA_Z040_LOGT[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DB_Z000_LOGT[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DB_Z010_LOGT[5][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DB_Z020_LOGT[6][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DB_Z030_LOGT[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DB_Z040_LOGT[9][WDWARF_BASTI_ALEN] = {

};

/* Luminosity grids */
static double const BASTI_BLOUIN_DA_Z000_LOGL[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DA_Z006_LOGL[6][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DA_Z010_LOGL[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DA_Z020_LOGL[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DA_Z030_LOGL[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DA_Z040_LOGL[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DB_Z000_LOGL[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DB_Z010_LOGL[4][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DB_Z020_LOGL[5][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DB_Z030_LOGL[6][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_BLOUIN_DB_Z040_LOGL[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DA_Z000_LOGL[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DA_Z006_LOGL[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DA_Z010_LOGL[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DA_Z020_LOGL[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DA_Z030_LOGL[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DA_Z040_LOGL[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DB_Z000_LOGL[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DB_Z010_LOGL[5][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DB_Z020_LOGL[6][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DB_Z030_LOGL[9][WDWARF_BASTI_ALEN] = {

};
static double const BASTI_CASSISI_DB_Z040_LOGL[9][WDWARF_BASTI_ALEN] = {

};