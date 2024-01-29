/**
    @file wdwarf.c
    @brief Subroutines that control the evolution of a white dwarf primary.
    @author Joseph Livesey ([jrlivesey@wisc.edu](mailto:jrlivesey@wisc.edu>))
    @date 2023
*/

#include "vplanet.h"

void BodyCopyWdwarf(BODY *dest, BODY *src, int foo, int iNumBodies, int iBody) {
  dest[iBody].dLuminosity       = src[iBody].dLuminosity;
  dest[iBody].dTemperature      = src[iBody].dTemperature;
  dest[iBody].dLXUV             = src[iBody].dLXUV;
  dest[iBody].iWDModel          = src[iBody].iWDModel;
  dest[iBody].iOpacityModel     = src[iBody].iOpacityModel;
  dest[iBody].iMetallicityLevel = src[iBody].iMetallicityLevel;
  dest[iBody].bHeAtm            = src[iBody].bHeAtm;
}

/**************** WDWARF options ****************/

void ReadWDModel(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  /* This option cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "b", 1)) {
      body[iFile - 1].iWDModel = WDWARF_MODEL_BASTI;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are basti22.",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].iWDModel = WDWARF_MODEL_BASTI;
  }
}

void ReadOpacityModel(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This option cannot exist in primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (!memcmp(sLower(cTmp), "b", 1)) {
      body[iFile - 1].iOpacityModel = OPACITY_MODEL_BLOUIN;
    } else if (!memcmp(sLower(cTmp), "c", 1)) {
      body[iFile - 1].iOpacityModel = OPACITY_MODEL_CASSISI;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %s. Options are blouin20 or "
                "cassisi07.",
                options->cName, cTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].iOpacityModel = OPACITY_MODEL_BLOUIN;
  }
}

void ReadMetallicity(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int iTmp;

  AddOptionInt(files->Infile[iFile].cIn, options->cName, &iTmp, &lTmp,
               control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (iTmp >= 0 && iTmp <= 5) {
      body[iFile - 1].iMetallicityLevel = iTmp;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Unknown argument to %s: %i. Must be 1-5.",
                options->cName, iTmp);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].iMetallicityLevel = 0;
  }
}

void ReadHeAtm(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bHeAtm = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].bHeAtm = 0;
  }
}

void ReadHaltEndBastiGrid(BODY *body, CONTROL *control, FILES *files,
                            OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    control->Halt[iFile - 1].bHaltEndBastiGrid = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultInt(options, &control->Halt[iFile - 1].bHaltEndBastiGrid,
                       files->iNumInputs);
    }
  }
}

void InitializeOptionsWdwarf(OPTIONS *options, fnReadOption *fnRead) {
  int iOpt, iFile;

  sprintf(options[OPT_WDMODEL].cName, "sWDModel");
  sprintf(options[OPT_WDMODEL].cDescr, "WD luminosity evolution model");
  sprintf(options[OPT_WDMODEL].cDefault, "basti22");
  sprintf(options[OPT_WDMODEL].cValues, "basti22");
  options[OPT_WDMODEL].iType      = 3;
  options[OPT_WDMODEL].bMultiFile = 1;
  fnRead[OPT_WDMODEL]             = &ReadWDModel;
  sprintf(options[OPT_WDMODEL].cLongDescr,
          "If BASTI22 is selected luminosity and effective temperature will\n"
          "follow the model of Salaris, M. et al. (2022).\n");
  
  sprintf(options[OPT_OPACITYMODEL].cName, "sOpacityModel");
  sprintf(options[OPT_OPACITYMODEL].cDescr, "Electron opacity model");
  sprintf(options[OPT_OPACITYMODEL].cDefault, "blouin20");
  sprintf(options[OPT_OPACITYMODEL].cValues, "blouin20 cassisi07");
  options[OPT_OPACITYMODEL].iType      = 3;
  options[OPT_OPACITYMODEL].bMultiFile = 1;
  fnRead[OPT_OPACITYMODEL]             = &ReadOpacityModel;
  sprintf(options[OPT_OPACITYMODEL].cLongDescr,
          "Electron opacity model for the WD interior.\n");
  
  sprintf(options[OPT_METALLICITY].cName, "iMetallicityLevel");
  sprintf(options[OPT_METALLICITY].cDescr, "Integral measure of metallicity");
  sprintf(options[OPT_METALLICITY].cDefault, "0");
  options[OPT_METALLICITY].iType      = 1;
  options[OPT_METALLICITY].bMultiFile = 1;
  fnRead[OPT_METALLICITY]             = &ReadMetallicity;
  sprintf(options[OPT_METALLICITY].cLongDescr,
          "Integral measure of metallicity:\n"
          "0: Z = 0\n"
          "1: Z = 0.006\n"
          "2: Z = 0.01\n"
          "3: Z = 0.02\n"
          "4: Z = 0.03\n"
          "5: Z = 0.04\n");
  
  sprintf(options[OPT_HEATMOSPHERE].cName, "bHeAtm");
  sprintf(options[OPT_HEATMOSPHERE].cDescr, "Use helium atmosphere?");
  sprintf(options[OPT_HEATMOSPHERE].cDefault, "0");
  options[OPT_HEATMOSPHERE].iType      = 0;
  options[OPT_HEATMOSPHERE].bMultiFile = 1;
  fnRead[OPT_HEATMOSPHERE]             = &ReadHeAtm;
  sprintf(options[OPT_HEATMOSPHERE].cLongDescr,
          "If 0, uses hydrogen envelope BaSTI grid. If 1, uses helium \n"
          "envelope BaSTI grid.\n");
  
  sprintf(options[OPT_HALTENDBARAFFEFGRID].cName, "bHaltEndBastiGrid");
  sprintf(options[OPT_HALTENDBARAFFEFGRID].cDescr,
          "Halt when we reach the end of the BaSTI grid?");
  sprintf(options[OPT_HALTENDBARAFFEFGRID].cDefault, "1");
  options[OPT_HALTENDBARAFFEFGRID].iType = 0;
  fnRead[OPT_HALTENDBARAFFEFGRID]        = &ReadHaltEndBastiGrid;
  sprintf(options[OPT_HALTENDBARAFFEFGRID].cLongDescr,
          "The BaSTI WD model will only compute parameters until about 10 Gyr\n"
          "after the main sequence. Setting this flag to 1 will halt the code\n"
          "if the end of the model grid is reached.");
}

/**************** Verify WDWARF  ****************/

void VerifyLuminosityWdwarf(BODY *body, CONTROL *control, OPTIONS *options,
                            UPDATE *update, double dAge, int iBody) {

  if (body[iBody].iWDModel == WDWARF_MODEL_BASTI) {
    body[iBody].dLuminosity =
          fdLuminosityFunctionBasti(body[iBody].iOpacityModel,
                                    body[iBody].bHeAtm,
                                    body[iBody].iMetallicityLevel,
                                    body[iBody].dAge,
                                    body[iBody].dMass);
    if (options[OPT_LUMINOSITY].iLine[iBody+1] >= 0) {
      // Disregarding user-specified luminosity
      if (control->Io.iVerbose >= VERBINPUT) {
        printf("INFO: Luminosity set for body %d, but this value will be "
               "computed from the grid.\n",
               iBody);
      }
    }
  } else {
    // ?
  }

  update[iBody].iaType[update[iBody].iLuminosity][0]     = 0;
  update[iBody].iNumBodies[update[iBody].iLuminosity][0] = 1;
  update[iBody].iaBody[update[iBody].iLuminosity][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iLuminosity][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iLuminosity][0][0] = iBody;

  update[iBody].pdLuminosityWdwarf =
        &update[iBody].daDerivProc[update[iBody].iLuminosity][0];
}

void VerifyTemperatureWdwarf(BODY *body, CONTROL *control, OPTIONS *options,
                             UPDATE *update, double dAge, int iBody) {

  if (body[iBody].iWDModel == WDWARF_MODEL_BASTI) {
    body[iBody].dTemperature =
          fdTemperatureFunctionBasti(body[iBody].iOpacityModel,
                                     body[iBody].bHeAtm,
                                     body[iBody].iMetallicityLevel,
                                     body[iBody].dAge,
                                     body[iBody].dMass);
    if (options[OPT_TEMPERATURE].iLine[iBody+1] >= 0) {
      printf("INFO: Temperature set for body %d, but this value will be "
               "computed from the grid.\n",
               iBody);
    }
  } else {
    // ?
  }

  update[iBody].iaType[update[iBody].iTemperature][0]     = 0;
  update[iBody].iNumBodies[update[iBody].iTemperature][0] = 1;
  update[iBody].iaBody[update[iBody].iTemperature][0]     = malloc(
            update[iBody].iNumBodies[update[iBody].iTemperature][0] * sizeof(int));
  update[iBody].iaBody[update[iBody].iTemperature][0][0] = iBody;

  update[iBody].pdTemperatureWdwarf =
        &update[iBody].daDerivProc[update[iBody].iTemperature][0];
}

void VerifyRadiusWdwarf(BODY *body, CONTROL *control, OPTIONS *options,
                        UPDATE *update, int iBody) {
  
  if (body[iBody].iWDModel == WDWARF_MODEL_BASTI) {
    if (options[OPT_TEMPERATURE].iLine[iBody+1] < 0) {
      body[iBody].dRadius = REARTH;
      // if (body[iBody].dMass < MSUN) {
      //   // If the user did not set the radius, calculate from power law
      //   // body[iBody].dRadius = fdRadiusWdwarf(body, iBody);
      //   // If the user did not set the radius, set to Earth radius
      //   body[iBody].dRadius = REARTH;
      // } else {
      //   // In the relativistic regime, there exists no power law
      //   if (control->Io.iVerbose >= VERBERR) {
      //     printf("ERROR: Must set radius when body %d has mass greater than "
      //            "1 solar mass.\n",
      //            iBody);
      //   }
      //   exit(1);
      // }
    }
  }
}

void VerifyTidalQ(BODY *body, CONTROL *control, OPTIONS *options,
                  UPDATE *update, int iBody) {
  // Bit of a hack, just changing the default value of Q for WDs
  if (body[iBody].bEqtide) {
    if (options[OPT_TIDALQ].iLine[iBody+1] < 0) {
      body[iBody].dTidalQ = 1.e10;
    }
  }
}

void fnPropsAuxWdwarf(BODY *body, EVOLVE *evolve, IO *io, UPDATE *update,
                      int iBody) {
  // Empty
}

void fnForceBehaviorWdwarf(BODY *body, MODULE *module, EVOLVE *evolve, IO *io,
                           SYSTEM *system, UPDATE *update,
                           fnUpdateVariable ***fnUpdate, int iBody,
                           int iModule) {
  // Empty
}

void AssignWdwarfDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                             fnUpdateVariable ***fnUpdate, int iBody) {
  // The "derivatives" just point to the values for both of these quantities!
  fnUpdate[iBody][update[iBody].iLuminosity][0] = &fdLuminosityWdwarf;
  fnUpdate[iBody][update[iBody].iTemperature][0] = &fdTemperatureWdwarf;
}

void NullWdwarfDerivatives(BODY *body, EVOLVE *evolve, UPDATE *update,
                           fnUpdateVariable ***fnUpdate, int iBody) {
  fnUpdate[iBody][update[iBody].iLuminosity][0] = &fndUpdateFunctionTiny;
  fnUpdate[iBody][update[iBody].iTemperature][0] = &fndUpdateFunctionTiny;
}

void VerifyWdwarf(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  OUTPUT *output, SYSTEM *system, UPDATE *update, int iBody,
                  int iModule) {
  /* This routine is called when WDWARF is active for a body */

  if (update[iBody].iNumLuminosity > 1) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: Looks like there's more than one equation trying to set "
              "dLuminosity for body %d!",
              iBody);
    }
    exit(EXIT_INPUT);
  }

  if (update[iBody].iNumRadius > 1) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: Looks like there's more than one equation trying to set "
              "dRadius for body %d!",
              iBody);
    }
    exit(EXIT_INPUT);
  }

  if (update[iBody].iNumTemperature > 1) {
    if (control->Io.iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: Looks like there's more than one equation trying to set "
              "dTemperature for body %d!",
              iBody);
    }
    exit(EXIT_INPUT);
  }

  VerifyLuminosityWdwarf(body, control, options, update, body[iBody].dAge, iBody);
  VerifyTemperatureWdwarf(body, control, options, update, body[iBody].dAge, iBody);
  VerifyRadiusWdwarf(body, control, options, update, iBody);

  control->fnPropsAux[iBody][iModule] = &fnPropsAuxWdwarf;
  control->fnForceBehavior[iBody][iModule] = &fnForceBehaviorWdwarf;
  control->Evolve.fnBodyCopy[iBody][iModule] = &BodyCopyWdwarf;
}

void ReadOptionsWdwarf(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, fnReadOption *fnRead,
                        int iBody) {
  int iOpt;

  for (iOpt = OPTSTARTWDWARF; iOpt < OPTENDWDWARF; iOpt++) {
    if (options[iOpt].iType != -1) {
      fnRead[iOpt](body, control, files, &options[iOpt], system, iBody + 1);
    }
  }
}

/**************** WDWARF Update  ****************/

void InitializeUpdateWdwarf(BODY *body, UPDATE *update, int iBody) {

  if (update[iBody].iNumLuminosity == 0) {
    update[iBody].iNumVars++;
  }
  update[iBody].iNumLuminosity++;

  // if (body[iBody].dRadius > 0.) {
  //   if (update[iBody].iNumRadius == 0) {
  //     update[iBody].iNumVars++;
  //   }
  //   update[iBody].iNumRadius++;
  // }

  // if (update[iBody].iNumRot == 0) {
  //   update[iBody].iNumVars++;
  // }
  // update[iBody].iNumRot++;

  if (body[iBody].dTemperature > 0.) {
    if (update[iBody].iNumTemperature == 0) {
      update[iBody].iNumVars++;
    }
    update[iBody].iNumTemperature++;
  }
}

void FinalizeUpdateLuminosityWdwarf(BODY *body, UPDATE *update, int *iEqn,
                                    int iVar, int iBody, int foo) {
  update[iBody].iaModule[iVar][*iEqn] = WDWARF;
  update[iBody].iNumLuminosity        = (*iEqn)++;
}

void FinalizeUpdateTemperatureWdwarf(BODY *body, UPDATE *update, int *iEqn,
                                     int iVar, int iBody, int foo) {
  update[iBody].iaModule[iVar][*iEqn] = WDWARF;
  update[iBody].iNumTemperature       = (*iEqn)++;
}

/**************** WDWARF Outputs ****************/

void WriteLuminosityWdwarf(BODY *body, CONTROL *control, OUTPUT *output,
                     SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                     double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dLuminosity;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}

void WriteTemperatureWdwarf(BODY *body, CONTROL *control, OUTPUT *output,
                      SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
                      double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dTemperature;
  // Kelvin only
  fsUnitsTemp(0, cUnit);
}

void WriteLXUVWdwarf(BODY *body, CONTROL *control, OUTPUT *output,
               SYSTEM *system, UNITS *units, UPDATE *update, int iBody,
               double *dTmp, char cUnit[]) {
  *dTmp = body[iBody].dLXUV;

  if (output->bDoNeg[iBody]) {
    *dTmp *= output->dNeg;
    strcpy(cUnit, output->cNeg);
  } else {
    *dTmp /= fdUnitsPower(units->iTime, units->iMass, units->iLength);
    fsUnitsPower(units, cUnit);
  }
}

void InitializeOutputWdwarf(OUTPUT *output, fnWriteOutput fnWrite[]) {

  sprintf(output[OUT_LUMINOSITYWDWARF].cName, "WDLuminosity");
  sprintf(output[OUT_LUMINOSITYWDWARF].cDescr, "Luminosity");
  sprintf(output[OUT_LUMINOSITYWDWARF].cNeg, "LSUN");
  output[OUT_LUMINOSITYWDWARF].bNeg       = 1;
  output[OUT_LUMINOSITYWDWARF].dNeg       = 1. / LSUN;
  output[OUT_LUMINOSITYWDWARF].iNum       = 1;
  output[OUT_LUMINOSITYWDWARF].iModuleBit = WDWARF;
  fnWrite[OUT_LUMINOSITYWDWARF]           = &WriteLuminosityWdwarf;

  sprintf(output[OUT_TEMPERATUREWDWARF].cName, "WDTemperature");
  sprintf(output[OUT_TEMPERATUREWDWARF].cDescr, "Effective Temperature");
  output[OUT_TEMPERATUREWDWARF].bNeg       = 0;
  output[OUT_TEMPERATUREWDWARF].iNum       = 1;
  output[OUT_TEMPERATUREWDWARF].iModuleBit = WDWARF;
  fnWrite[OUT_TEMPERATUREWDWARF]           = &WriteTemperatureWdwarf;

  sprintf(output[OUT_LXUVWDWARF].cName, "WDLXUV");
  sprintf(output[OUT_LXUVWDWARF].cDescr, "Base X-ray/XUV Luminosity");
  sprintf(output[OUT_LXUVWDWARF].cNeg, "LSUN");
  output[OUT_LXUVWDWARF].bNeg       = 1;
  output[OUT_LXUVWDWARF].dNeg       = 1. / LSUN;
  output[OUT_LXUVWDWARF].iNum       = 1;
  output[OUT_LXUVWDWARF].iModuleBit = WDWARF;
  fnWrite[OUT_LXUVWDWARF]           = &WriteLXUVWdwarf;
}

/***************** WDWARF Halts *****************/

int fbHaltEndBastiGrid(BODY *body, EVOLVE *evolve, HALT *halt, IO *io,
                        UPDATE *update, fnUpdateVariable ***fnUpdate,
                        int iBody) {
  // Halt evolution when age > end of BaSTI grid
  if (body[iBody].iWDModel == WDWARF_MODEL_CONST) {
    if (io->iVerbose > VERBPROG) {
      printf("HALT: %s reached the edge of the luminosity grid at ",
             body[iBody].cName);
      fprintd(stdout, body[iBody].dAge / YEARSEC, io->iSciNot, io->iDigits);
      printf(" years.\n");
    }
    return 1;
  }
  return 0;
}

void CountHaltsWdwarf(HALT *halt, int *iHalt) {
  if (halt->bHaltEndBastiGrid) {
    (*iHalt)++;
  }
}

void VerifyHaltWdwarf(BODY *body, CONTROL *control, OPTIONS *options,
                      int iBody, int *iHalt) {
  if (control->Halt[iBody].bHaltEndBastiGrid) {
    control->fnHalt[iBody][(*iHalt)++] = &fbHaltEndBastiGrid;
  }
}

/*********** WDWARF Logging Functions ***********/

void LogOptionsWdwarf(CONTROL *control, FILE *fp) {
  // Empty
}

void LogWdwarf(BODY *body, CONTROL *control, OUTPUT *output, SYSTEM *system,
               UPDATE *update, fnWriteOutput fnWrite[], FILE *fp) {
  // Empty
}

void LogBodyWdwarf(BODY *body, CONTROL *control, OUTPUT *output,
                    SYSTEM *system, UPDATE *update, fnWriteOutput fnWrite[],
                    FILE *fp, int iBody) {
  int iOut;
  fprintf(fp, "----- WDWARF PARAMETERS (%s)------\n", body[iBody].cName);

  for (iOut = OUTSTARTWDWARF; iOut < OUTENDWDWARF; iOut++) {
    if (output[iOut].iNum > 0) {
      WriteLogEntry(body, control, &output[iOut], system, update, fnWrite[iOut],
                    fp, iBody);
    }
  }
}

void AddModuleWdwarf(CONTROL *control, MODULE *module, int iBody, int iModule) {

  module->iaModule[iBody][iModule] = WDWARF;

  module->fnReadOptions[iBody][iModule]       = &ReadOptionsWdwarf;
  module->fnLogBody[iBody][iModule]           = &LogBodyWdwarf;
  module->fnVerify[iBody][iModule]            = &VerifyWdwarf;
  module->fnCountHalts[iBody][iModule]        = &CountHaltsWdwarf;
  module->fnVerifyHalt[iBody][iModule]        = &VerifyHaltWdwarf;
  module->fnAssignDerivatives[iBody][iModule] = &AssignWdwarfDerivatives;
  module->fnNullDerivatives[iBody][iModule]   = &NullWdwarfDerivatives;
  module->fnInitializeUpdate[iBody][iModule]  = &InitializeUpdateWdwarf;
  module->fnFinalizeUpdateLuminosity[iBody][iModule] =
        &FinalizeUpdateLuminosityWdwarf;
  module->fnFinalizeUpdateTemperature[iBody][iModule] = 
        &FinalizeUpdateTemperatureWdwarf;
}

/*************** WDWARF Functions ***************/

double fdTrapezoid(BODY *body, int iBody,
                   double (*func)(BODY *, int, double), double lo,
                   double hi, int n) {
    // Numerical integration of an arbitrary function via the trapezoid rule
    // on the interval [lo, hi].
    double h, res;
    int k;
    h = (hi - lo) / n;
    res = 0.5 * h * (func(body, iBody, lo) + func(body, iBody, hi));
    for (k=1; k<n; k++) {
        res += h * func(body, iBody, lo + k * h);
    }
    return res;
}

double fdRomberg(BODY *body, int iBody, double (*func)(BODY *, int, double),
                 double lo, double hi) {
    // Romberg integration of an arbitrary function on the interval [lo, hi].
    double R[JMAX+1][JMAX+1];
    double h, res;
    int j, k;
    int n = 1e2; // Number of steps

    for (j=0; j<=JMAX; j++) {
        h = (hi - lo) / n; // Step size
        R[j][0] = fdTrapezoid(body, iBody, func, lo, hi, n);
        for (k=1; k<=j; k++) {
            R[j][k] = R[j][k-1] + 1.0/(pow(4.0, k) - 1.0) *
                      (R[j][k-1] - R[j-1][k-1]);
        }
        n *= 2; // Increase number of steps
    }
    return R[JMAX][JMAX];
}

double fdXUVFracWdwarf(BODY *body, int iBody) {
  // Returns the fraction of the bolometric luminosity within the XUV regime,
  // assuming a perfect blackbody.
  double intxuv, norm, res;
  double lo = XUV_LO;
  double hi = XUV_HI;

  intxuv = fdIntegratePlanckSpectrum(body, iBody, lo, hi);
  norm = fdIntegrateTotalPlanckSpectrum(body, iBody);
  res = intxuv/norm;
  return res;
}

double fdIntegratePlanckSpectrum(BODY *body, int iBody, double lo, double hi) {
  // Integral of the Planck spectrum over frequency from lo to hi.
  double dTemp = body[iBody].dTemperature;
  /*
    Integration here
  */
}

double fdIntegrateTotalPlanckSpectrum(BODY *body, int iBody) {
  // Integral of the total Planck spectrum from 0 to +inf.
  double dTemp = body[iBody].dTemperature;
  /*
    Integration here
  */
}

double fdPlanckSpectrum(BODY *body, int iBody, double dFreq) {
  // Spectral energy density of a blackbody (IN SI UNITS)
  double res;
  double dTemp = body[iBody].dTemperature;
  res = 8.0 * PI * HPLANCK * pow(dFreq, 3) / pow(LIGHTSPEED, 3) 
        / (exp(KBOLTZ / HPLANCK * dTemp / dFreq) - 1.0);
  return res;
}

double fdLXUVWdwarf(BODY *body, SYSTEM *system, int *iaBody) {
  double xuvfrac, res;
  xuvfrac = fdXUVFracWdwarf(body, iaBody[0]);
  res = xuvfrac * body[iaBody[0]].dLuminosity;
  return res;
}

// double fdRadiusWdwarf(BODY *body, int iBody) {
//   // No interpolation here; this is simply the mass-radius power law.
//   // From Chandrasekhar (1939)
//   double R = 0.716 * pow(MMWE, -5./3.) * pow(body[iBody].dMass, -1./3.);
//   return R;
// }

double fdLuminosityWdwarf(BODY *body, SYSTEM *system, int *iaBody) {
  double res;
  if (body[iaBody[0]].iWDModel == WDWARF_MODEL_BASTI) {
    res = fdLuminosityFunctionBasti(body[iaBody[0]].iOpacityModel,
                                    body[iaBody[0]].bHeAtm,
                                    body[iaBody[0]].iMetallicityLevel,
                                    body[iaBody[0]].dAge,
                                    body[iaBody[0]].dMass);
    if (!isnan(res)) {
      return res;
    } else {
      body[iaBody[0]].iWDModel = WDWARF_MODEL_CONST;
    }
  }
  if (body[iaBody[0]].iWDModel == WDWARF_MODEL_CONST) {
    return body[iaBody[0]].dLuminosity;
  } else {
    return 0;
  }
}

double fdTemperatureWdwarf(BODY *body, SYSTEM *system, int *iaBody) {
  double res;
  if (body[iaBody[0]].iWDModel == WDWARF_MODEL_BASTI) {
    res = fdTemperatureFunctionBasti(body[iaBody[0]].iOpacityModel,
                                     body[iaBody[0]].bHeAtm,
                                     body[iaBody[0]].iMetallicityLevel,
                                     body[iaBody[0]].dAge,
                                     body[iaBody[0]].dMass);
    if (!isnan(res)) {
      return res;
    } else {
      body[iaBody[0]].iWDModel = WDWARF_MODEL_CONST;
    }
  }
  if (body[iaBody[0]].iWDModel == WDWARF_MODEL_CONST) {
    return body[iaBody[0]].dTemperature;
  } else {
    return 0;
  }
}

double fdLuminosityFunctionBasti(int iOpacityModel, int bHeAtm,
                                 int iMetallicityLevel, double dAge,
                                 double dMass) {
  int iError;
  double L = fdBasti(WDWARF_L, iOpacityModel, bHeAtm, iMetallicityLevel,
                     dAge, dMass, 3, &iError);
  if ((iError == WDWARF_ERR_NONE) || (iError == WDWARF_ERR_LINEAR)) {
    return L;
  } else if (iError == WDWARF_ERR_OUTOFBOUNDS_HI ||
             iError == WDWARF_ERR_ISNAN) {
    return NAN;
  } else {
    if (iError == WDWARF_ERR_OUTOFBOUNDS_LO) {
      fprintf(stderr,
              "ERROR: Luminosity out of bounds (low) in fdBasti().\n");
    } else if (iError == WDWARF_ERR_FILE) {
      fprintf(stderr,
              "ERROR: File access error in Luminosity routine fdBasti().\n");
    } else if (iError == WDWARF_ERR_BADORDER) {
      fprintf(stderr,
              "ERROR: Bad Luminosity interpolation order in routine "
              "fdBasti().\n");
    } else {
      fprintf(stderr, "ERROR: Undefined Luminosity error in fdBasti().\n");
    }
    exit(EXIT_INT);
  }
}

double fdTemperatureFunctionBasti(int iOpacityModel, int bHeAtm,
                                  int iMetallicityLevel, double dAge,
                                  double dMass) {
  int iError;
  double T = fdBasti(WDWARF_T, iOpacityModel, bHeAtm, iMetallicityLevel,
                     dAge, dMass, 3, &iError);
  if ((iError == WDWARF_ERR_NONE) || (iError == WDWARF_ERR_LINEAR)) {
    return T;
  } else if (iError == WDWARF_ERR_OUTOFBOUNDS_HI ||
             iError == WDWARF_ERR_ISNAN) {
    return NAN;
  } else {
    if (iError == WDWARF_ERR_OUTOFBOUNDS_LO) {
      fprintf(stderr,
              "ERROR: Temperature out of bounds (low) in fdBasti().\n");
    } else if (iError == WDWARF_ERR_FILE) {
      fprintf(stderr,
              "ERROR: File access error in Temperature routine fdBasti().\n");
    } else if (iError == WDWARF_ERR_BADORDER) {
      fprintf(stderr,
              "ERROR: Bad Temperature interpolation order in routine "
              "fdBasti().\n");
    } else {
      fprintf(stderr, "ERROR: Undefined Temperature error in fdBasti().\n");
    }
    exit(EXIT_INT);
  }
}

double fdBastiBiLinear7(int iMLEN, int iALEN,
                       double const data[9][WDWARF_BASTI_ALEN],
                       int xi, int yi, double dx, double dy) {
  // Linearly interpolate over data, given indices of lower bounds on grid xi,
  // yi and normalized distances to the interpolation point dx, dy.
  // Basically identical to `fdBaraffeBiLinear` in body.c.
  // This function handles BaSTI grids with 7 mass values.
  double C0, C1, C;
  if (dx == 0) {
    C0 = data[xi][yi];
    C1 = data[xi][yi + 1];
  } else {
    C0 = data[xi][yi] * (1 - dx) + data[xi + 1][yi] * dx;
    C1 = data[xi][yi + 1] * (1 - dx) + data[xi + 1][yi + 1] * dx;
  }
  if (dy == 0) {
    C = C0;
  } else {
    C = C0 * (1 - dy) + C1 * dy;
  }
  return C;
}

double fdBastiBiLinear4(int iMLEN, int iALEN,
                       double const data[6][WDWARF_BASTI_ALEN],
                       int xi, int yi, double dx, double dy) {
  // This function handles BaSTI grids with 4 mass values.
  double C0, C1, C;
  if (dx == 0) {
    C0 = data[xi][yi];
    C1 = data[xi][yi + 1];
  } else {
    C0 = data[xi][yi] * (1 - dx) + data[xi + 1][yi] * dx;
    C1 = data[xi][yi + 1] * (1 - dx) + data[xi + 1][yi + 1] * dx;
  }
  if (dy == 0) {
    C = C0;
  } else {
    C = C0 * (1 - dy) + C1 * dy;
  }
  return C;
}

double fdBastiBiLinear3(int iMLEN, int iALEN,
                       double const data[5][WDWARF_BASTI_ALEN],
                       int xi, int yi, double dx, double dy) {
  // This function handles BaSTI grids with 3 mass values.
  double C0, C1, C;
  if (dx == 0) {
    C0 = data[xi][yi];
    C1 = data[xi][yi + 1];
  } else {
    C0 = data[xi][yi] * (1 - dx) + data[xi + 1][yi] * dx;
    C1 = data[xi][yi + 1] * (1 - dx) + data[xi + 1][yi + 1] * dx;
  }
  if (dy == 0) {
    C = C0;
  } else {
    C = C0 * (1 - dy) + C1 * dy;
  }
  return C;
}

double fdBastiBiLinear2(int iMLEN, int iALEN,
                       double const data[4][WDWARF_BASTI_ALEN],
                       int xi, int yi, double dx, double dy) {
  // This function handles BaSTI grids with 2 mass values.
  double C0, C1, C;
  if (dx == 0) {
    C0 = data[xi][yi];
    C1 = data[xi][yi + 1];
  } else {
    C0 = data[xi][yi] * (1 - dx) + data[xi + 1][yi] * dx;
    C1 = data[xi][yi + 1] * (1 - dx) + data[xi + 1][yi + 1] * dx;
  }
  if (dy == 0) {
    C = C0;
  } else {
    C = C0 * (1 - dy) + C1 * dy;
  }
  return C;
}

double fdBastiBiCubic7(int iMLEN, int iALEN,
                       double const data[9][WDWARF_BASTI_ALEN],
                       int xi, int yi, double dx, double dy) {
  // Perform bicubic interpolation over data, given indices of lower bounds on
  // grid xi, yi and normalized distances to the interpolation point dx, dy.
  // Basically identical to `fdBaraffeBiCubic` in body.c.
  // This function handles BaSTI grids with 7 mass values.
  double dvCoeff[16];
  int j, k;
  int ijkn      = 0;
  double dypow  = 1;
  double result = 0;

  double dvDeriv[16] = {// values of the function at each corner
                        data[xi][yi], data[xi + 1][yi], data[xi][yi + 1],
                        data[xi + 1][yi + 1],

                        // values of df/dx at each corner.
                        0.5 * (data[xi + 1][yi] - data[xi - 1][yi]),
                        0.5 * (data[xi + 2][yi] - data[xi][yi]),
                        0.5 * (data[xi + 1][yi + 1] - data[xi - 1][yi + 1]),
                        0.5 * (data[xi + 2][yi + 1] - data[xi][yi + 1]),

                        // values of df/dy at each corner.
                        0.5 * (data[xi][yi + 1] - data[xi][yi - 1]),
                        0.5 * (data[xi + 1][yi + 1] - data[xi + 1][yi - 1]),
                        0.5 * (data[xi][yi + 2] - data[xi][yi]),
                        0.5 * (data[xi + 1][yi + 2] - data[xi + 1][yi]),

                        // values of d2f/dxdy at each corner.
                        0.25 * (data[xi + 1][yi + 1] - data[xi - 1][yi + 1] -
                                data[xi + 1][yi - 1] + data[xi - 1][yi - 1]),
                        0.25 * (data[xi + 2][yi + 1] - data[xi][yi + 1] -
                                data[xi + 2][yi - 1] + data[xi][yi - 1]),
                        0.25 * (data[xi + 1][yi + 2] - data[xi - 1][yi + 2] -
                                data[xi + 1][yi] + data[xi - 1][yi]),
                        0.25 * (data[xi + 2][yi + 2] - data[xi][yi + 2] -
                                data[xi + 2][yi] + data[xi][yi])};

  fvMatrixVectorMult(STELLAR_BICUBIC_MATRIX, dvDeriv, dvCoeff);
  dypow = 1;
  for (j = 0; j < 4; ++j) {
    result +=
          dypow * (dvCoeff[ijkn] +
                   dx * (dvCoeff[ijkn + 1] +
                         dx * (dvCoeff[ijkn + 2] + dx * dvCoeff[ijkn + 3])));
    ijkn += 4;
    dypow *= dy;
  }
  return result;
}

double fdBastiBiCubic4(int iMLEN, int iALEN,
                       double const data[6][WDWARF_BASTI_ALEN],
                       int xi, int yi, double dx, double dy) {
  // This function handles BaSTI grids with 4 mass values.
  double dvCoeff[16];
  int j, k;
  int ijkn      = 0;
  double dypow  = 1;
  double result = 0;

  double dvDeriv[16] = {// values of the function at each corner
                        data[xi][yi], data[xi + 1][yi], data[xi][yi + 1],
                        data[xi + 1][yi + 1],

                        // values of df/dx at each corner.
                        0.5 * (data[xi + 1][yi] - data[xi - 1][yi]),
                        0.5 * (data[xi + 2][yi] - data[xi][yi]),
                        0.5 * (data[xi + 1][yi + 1] - data[xi - 1][yi + 1]),
                        0.5 * (data[xi + 2][yi + 1] - data[xi][yi + 1]),

                        // values of df/dy at each corner.
                        0.5 * (data[xi][yi + 1] - data[xi][yi - 1]),
                        0.5 * (data[xi + 1][yi + 1] - data[xi + 1][yi - 1]),
                        0.5 * (data[xi][yi + 2] - data[xi][yi]),
                        0.5 * (data[xi + 1][yi + 2] - data[xi + 1][yi]),

                        // values of d2f/dxdy at each corner.
                        0.25 * (data[xi + 1][yi + 1] - data[xi - 1][yi + 1] -
                                data[xi + 1][yi - 1] + data[xi - 1][yi - 1]),
                        0.25 * (data[xi + 2][yi + 1] - data[xi][yi + 1] -
                                data[xi + 2][yi - 1] + data[xi][yi - 1]),
                        0.25 * (data[xi + 1][yi + 2] - data[xi - 1][yi + 2] -
                                data[xi + 1][yi] + data[xi - 1][yi]),
                        0.25 * (data[xi + 2][yi + 2] - data[xi][yi + 2] -
                                data[xi + 2][yi] + data[xi][yi])};

  fvMatrixVectorMult(STELLAR_BICUBIC_MATRIX, dvDeriv, dvCoeff);
  dypow = 1;
  for (j = 0; j < 4; ++j) {
    result +=
          dypow * (dvCoeff[ijkn] +
                   dx * (dvCoeff[ijkn + 1] +
                         dx * (dvCoeff[ijkn + 2] + dx * dvCoeff[ijkn + 3])));
    ijkn += 4;
    dypow *= dy;
  }
  return result;
}

double fdBastiBiCubic3(int iMLEN, int iALEN,
                       double const data[5][WDWARF_BASTI_ALEN],
                       int xi, int yi, double dx, double dy) {
  // This function handles BaSTI grids with 3 mass values.
  double dvCoeff[16];
  int j, k;
  int ijkn      = 0;
  double dypow  = 1;
  double result = 0;

  double dvDeriv[16] = {// values of the function at each corner
                        data[xi][yi], data[xi + 1][yi], data[xi][yi + 1],
                        data[xi + 1][yi + 1],

                        // values of df/dx at each corner.
                        0.5 * (data[xi + 1][yi] - data[xi - 1][yi]),
                        0.5 * (data[xi + 2][yi] - data[xi][yi]),
                        0.5 * (data[xi + 1][yi + 1] - data[xi - 1][yi + 1]),
                        0.5 * (data[xi + 2][yi + 1] - data[xi][yi + 1]),

                        // values of df/dy at each corner.
                        0.5 * (data[xi][yi + 1] - data[xi][yi - 1]),
                        0.5 * (data[xi + 1][yi + 1] - data[xi + 1][yi - 1]),
                        0.5 * (data[xi][yi + 2] - data[xi][yi]),
                        0.5 * (data[xi + 1][yi + 2] - data[xi + 1][yi]),

                        // values of d2f/dxdy at each corner.
                        0.25 * (data[xi + 1][yi + 1] - data[xi - 1][yi + 1] -
                                data[xi + 1][yi - 1] + data[xi - 1][yi - 1]),
                        0.25 * (data[xi + 2][yi + 1] - data[xi][yi + 1] -
                                data[xi + 2][yi - 1] + data[xi][yi - 1]),
                        0.25 * (data[xi + 1][yi + 2] - data[xi - 1][yi + 2] -
                                data[xi + 1][yi] + data[xi - 1][yi]),
                        0.25 * (data[xi + 2][yi + 2] - data[xi][yi + 2] -
                                data[xi + 2][yi] + data[xi][yi])};

  fvMatrixVectorMult(STELLAR_BICUBIC_MATRIX, dvDeriv, dvCoeff);
  dypow = 1;
  for (j = 0; j < 4; ++j) {
    result +=
          dypow * (dvCoeff[ijkn] +
                   dx * (dvCoeff[ijkn + 1] +
                         dx * (dvCoeff[ijkn + 2] + dx * dvCoeff[ijkn + 3])));
    ijkn += 4;
    dypow *= dy;
  }
  return result;
}

double fdBastiBiCubic2(int iMLEN, int iALEN,
                       double const data[4][WDWARF_BASTI_ALEN],
                       int xi, int yi, double dx, double dy) {
  // This function handles BaSTI grids with 2 mass values.
  double dvCoeff[16];
  int j, k;
  int ijkn      = 0;
  double dypow  = 1;
  double result = 0;

  double dvDeriv[16] = {// values of the function at each corner
                        data[xi][yi], data[xi + 1][yi], data[xi][yi + 1],
                        data[xi + 1][yi + 1],

                        // values of df/dx at each corner.
                        0.5 * (data[xi + 1][yi] - data[xi - 1][yi]),
                        0.5 * (data[xi + 2][yi] - data[xi][yi]),
                        0.5 * (data[xi + 1][yi + 1] - data[xi - 1][yi + 1]),
                        0.5 * (data[xi + 2][yi + 1] - data[xi][yi + 1]),

                        // values of df/dy at each corner.
                        0.5 * (data[xi][yi + 1] - data[xi][yi - 1]),
                        0.5 * (data[xi + 1][yi + 1] - data[xi + 1][yi - 1]),
                        0.5 * (data[xi][yi + 2] - data[xi][yi]),
                        0.5 * (data[xi + 1][yi + 2] - data[xi + 1][yi]),

                        // values of d2f/dxdy at each corner.
                        0.25 * (data[xi + 1][yi + 1] - data[xi - 1][yi + 1] -
                                data[xi + 1][yi - 1] + data[xi - 1][yi - 1]),
                        0.25 * (data[xi + 2][yi + 1] - data[xi][yi + 1] -
                                data[xi + 2][yi - 1] + data[xi][yi - 1]),
                        0.25 * (data[xi + 1][yi + 2] - data[xi - 1][yi + 2] -
                                data[xi + 1][yi] + data[xi - 1][yi]),
                        0.25 * (data[xi + 2][yi + 2] - data[xi][yi + 2] -
                                data[xi + 2][yi] + data[xi][yi])};

  fvMatrixVectorMult(STELLAR_BICUBIC_MATRIX, dvDeriv, dvCoeff);
  dypow = 1;
  for (j = 0; j < 4; ++j) {
    result +=
          dypow * (dvCoeff[ijkn] +
                   dx * (dvCoeff[ijkn + 1] +
                         dx * (dvCoeff[ijkn + 2] + dx * dvCoeff[ijkn + 3])));
    ijkn += 4;
    dypow *= dy;
  }
  return result;
}

double fdBastiInterpolate7(int iMLEN, int iALEN,
                     double const xarr[9], double const yarr[WDWARF_BASTI_ALEN],
                     double const data[9][WDWARF_BASTI_ALEN],
                     double M, double A, int iOrder, int *iError) {
  // Helper function for the interpolation. Very similar to
  // `fdBaraffeInterpolate` in body.c.
  // This function handles BaSTI grids with 7 mass values.

  double dx, dy;
  int xi, yi;
  int dxi, dyi;
  double result = 0;

  // Get bounds on grid
  *iError = 0;
  xi      = fiGetLowerBound(M, xarr, iMLEN);
  yi      = fiGetLowerBound(A, yarr, iALEN);

  if (xi < 0) {
    *iError = xi;
    return 0;
  } else if (yi < 0) {
    *iError = yi;
    return 0;
  }

  // Normalized distance to grid points
  dx = (M - xarr[xi]) / (xarr[xi + 1] - xarr[xi]);
  dy = (A - yarr[yi]) / (yarr[yi + 1] - yarr[yi]);

  if (iOrder == 1) {
    result = fdBastiBiLinear7(iMLEN, iALEN, data, xi, yi, dx, dy);
    if (isnan(result)) {
      *iError = WDWARF_ERR_ISNAN;
      return 0;
    }
    return result;
  } else if (iOrder == 3) {
    result = fdBastiBiCubic7(iMLEN, iALEN, data, xi, yi, dx, dy);
    if (isnan(result)) {
      // Maybe we can still linearly interpolate. Let's check:
      if (dx == 0) {
        for (dyi = 0; dyi < 2; dyi++) {
          if (isnan(data[xi][yi + dyi])) {
            // Hopeless; you're bounded by
            // a NaN on at least one side
            *iError = WDWARF_ERR_ISNAN;
            return 0;
          }
        }
      } else if (dy == 0) {
        for (dxi = 0; dxi < 2; dxi++) {
          if (isnan(data[xi + dxi][yi])) {
            // Hopeless; you're bounded by
            // a NaN on at least one side
            *iError = WDWARF_ERR_ISNAN;
            return 0;
          }
        }
      } else {
        for (dxi = 0; dxi < 2; dxi++) {
          for (dyi = 0; dyi < 2; dyi++) {
            if (isnan(data[xi + dxi][yi + dyi])) {
              // Hopeless; you're bounded by
              // a NaN on at least one side
              *iError = WDWARF_ERR_ISNAN;
              return 0;
            }
          }
        }
      }
      // We're good! A linear interpolation will save the day.
      *iError = WDWARF_ERR_LINEAR;
      return fdBastiBiLinear7(iMLEN, iALEN, data, xi, yi, dx, dy);
    }
    return result;
  } else {
    *iError = WDWARF_ERR_BADORDER;
    return 0;
  }
}

double fdBastiInterpolate4(int iMLEN, int iALEN,
                     double const xarr[6], double const yarr[WDWARF_BASTI_ALEN],
                     double const data[6][WDWARF_BASTI_ALEN],
                     double M, double A, int iOrder, int *iError) {
  // This function handles BaSTI grids with 4 mass values.

  double dx, dy;
  int xi, yi;
  int dxi, dyi;
  double result = 0;

  // Get bounds on grid
  *iError = 0;
  xi      = fiGetLowerBound(M, xarr, iMLEN);
  yi      = fiGetLowerBound(A, yarr, iALEN);

  if (xi < 0) {
    *iError = xi;
    return 0;
  } else if (yi < 0) {
    *iError = yi;
    return 0;
  }

  // Normalized distance to grid points
  dx = (M - xarr[xi]) / (xarr[xi + 1] - xarr[xi]);
  dy = (A - yarr[yi]) / (yarr[yi + 1] - yarr[yi]);

  if (iOrder == 1) {
    result = fdBastiBiLinear4(iMLEN, iALEN, data, xi, yi, dx, dy);
    if (isnan(result)) {
      *iError = WDWARF_ERR_ISNAN;
      return 0;
    }
    return result;
  } else if (iOrder == 3) {
    result = fdBastiBiCubic4(iMLEN, iALEN, data, xi, yi, dx, dy);
    if (isnan(result)) {
      // Maybe we can still linearly interpolate. Let's check:
      if (dx == 0) {
        for (dyi = 0; dyi < 2; dyi++) {
          if (isnan(data[xi][yi + dyi])) {
            // Hopeless; you're bounded by
            // a NaN on at least one side
            *iError = WDWARF_ERR_ISNAN;
            return 0;
          }
        }
      } else if (dy == 0) {
        for (dxi = 0; dxi < 2; dxi++) {
          if (isnan(data[xi + dxi][yi])) {
            // Hopeless; you're bounded by
            // a NaN on at least one side
            *iError = WDWARF_ERR_ISNAN;
            return 0;
          }
        }
      } else {
        for (dxi = 0; dxi < 2; dxi++) {
          for (dyi = 0; dyi < 2; dyi++) {
            if (isnan(data[xi + dxi][yi + dyi])) {
              // Hopeless; you're bounded by
              // a NaN on at least one side
              *iError = WDWARF_ERR_ISNAN;
              return 0;
            }
          }
        }
      }
      // We're good! A linear interpolation will save the day.
      *iError = WDWARF_ERR_LINEAR;
      return fdBastiBiLinear4(iMLEN, iALEN, data, xi, yi, dx, dy);
    }
    return result;
  } else {
    *iError = WDWARF_ERR_BADORDER;
    return 0;
  }
}

double fdBastiInterpolate3(int iMLEN, int iALEN,
                     double const xarr[5], double const yarr[WDWARF_BASTI_ALEN],
                     double const data[5][WDWARF_BASTI_ALEN],
                     double M, double A, int iOrder, int *iError) {
  // This function handles BaSTI grids with 3 mass values.

  double dx, dy;
  int xi, yi;
  int dxi, dyi;
  double result = 0;

  // Get bounds on grid
  *iError = 0;
  xi      = fiGetLowerBound(M, xarr, iMLEN);
  yi      = fiGetLowerBound(A, yarr, iALEN);

  if (xi < 0) {
    *iError = xi;
    return 0;
  } else if (yi < 0) {
    *iError = yi;
    return 0;
  }

  // Normalized distance to grid points
  dx = (M - xarr[xi]) / (xarr[xi + 1] - xarr[xi]);
  dy = (A - yarr[yi]) / (yarr[yi + 1] - yarr[yi]);

  if (iOrder == 1) {
    result = fdBastiBiLinear3(iMLEN, iALEN, data, xi, yi, dx, dy);
    if (isnan(result)) {
      *iError = WDWARF_ERR_ISNAN;
      return 0;
    }
    return result;
  } else if (iOrder == 3) {
    result = fdBastiBiCubic3(iMLEN, iALEN, data, xi, yi, dx, dy);
    if (isnan(result)) {
      // Maybe we can still linearly interpolate. Let's check:
      if (dx == 0) {
        for (dyi = 0; dyi < 2; dyi++) {
          if (isnan(data[xi][yi + dyi])) {
            // Hopeless; you're bounded by
            // a NaN on at least one side
            *iError = WDWARF_ERR_ISNAN;
            return 0;
          }
        }
      } else if (dy == 0) {
        for (dxi = 0; dxi < 2; dxi++) {
          if (isnan(data[xi + dxi][yi])) {
            // Hopeless; you're bounded by
            // a NaN on at least one side
            *iError = WDWARF_ERR_ISNAN;
            return 0;
          }
        }
      } else {
        for (dxi = 0; dxi < 2; dxi++) {
          for (dyi = 0; dyi < 2; dyi++) {
            if (isnan(data[xi + dxi][yi + dyi])) {
              // Hopeless; you're bounded by
              // a NaN on at least one side
              *iError = WDWARF_ERR_ISNAN;
              return 0;
            }
          }
        }
      }
      // We're good! A linear interpolation will save the day.
      *iError = WDWARF_ERR_LINEAR;
      return fdBastiBiLinear3(iMLEN, iALEN, data, xi, yi, dx, dy);
    }
    return result;
  } else {
    *iError = WDWARF_ERR_BADORDER;
    return 0;
  }
}

double fdBastiInterpolate2(int iMLEN, int iALEN,
                     double const xarr[4], double const yarr[WDWARF_BASTI_ALEN],
                     double const data[4][WDWARF_BASTI_ALEN],
                     double M, double A, int iOrder, int *iError) {
  // This function handles BaSTI grids with 2 mass values.

  double dx, dy;
  int xi, yi;
  int dxi, dyi;
  double result = 0;

  // Get bounds on grid
  *iError = 0;
  xi      = fiGetLowerBound(M, xarr, iMLEN);
  yi      = fiGetLowerBound(A, yarr, iALEN);

  if (xi < 0) {
    *iError = xi;
    return 0;
  } else if (yi < 0) {
    *iError = yi;
    return 0;
  }

  // Normalized distance to grid points
  dx = (M - xarr[xi]) / (xarr[xi + 1] - xarr[xi]);
  dy = (A - yarr[yi]) / (yarr[yi + 1] - yarr[yi]);

  if (iOrder == 1) {
    result = fdBastiBiLinear2(iMLEN, iALEN, data, xi, yi, dx, dy);
    if (isnan(result)) {
      *iError = WDWARF_ERR_ISNAN;
      return 0;
    }
    return result;
  } else if (iOrder == 3) {
    result = fdBastiBiCubic2(iMLEN, iALEN, data, xi, yi, dx, dy);
    if (isnan(result)) {
      // Maybe we can still linearly interpolate. Let's check:
      if (dx == 0) {
        for (dyi = 0; dyi < 2; dyi++) {
          if (isnan(data[xi][yi + dyi])) {
            // Hopeless; you're bounded by
            // a NaN on at least one side
            *iError = WDWARF_ERR_ISNAN;
            return 0;
          }
        }
      } else if (dy == 0) {
        for (dxi = 0; dxi < 2; dxi++) {
          if (isnan(data[xi + dxi][yi])) {
            // Hopeless; you're bounded by
            // a NaN on at least one side
            *iError = WDWARF_ERR_ISNAN;
            return 0;
          }
        }
      } else {
        for (dxi = 0; dxi < 2; dxi++) {
          for (dyi = 0; dyi < 2; dyi++) {
            if (isnan(data[xi + dxi][yi + dyi])) {
              // Hopeless; you're bounded by
              // a NaN on at least one side
              *iError = WDWARF_ERR_ISNAN;
              return 0;
            }
          }
        }
      }
      // We're good! A linear interpolation will save the day.
      *iError = WDWARF_ERR_LINEAR;
      return fdBastiBiLinear2(iMLEN, iALEN, data, xi, yi, dx, dy);
    }
    return result;
  } else {
    *iError = WDWARF_ERR_BADORDER;
    return 0;
  }
}

double fdBasti(int iParam, int iOpacityModel, int bHeAtm, int iMetallicityLevel,
               double A, double M, int iOrder, int *iError) {
  // Returns the stellar effective temperature or luminosity by interpolating
  // over the pertinent BaSTI grid using either a bilinear (iOrder = 1) or
  // bicubic (iOrder = 3) spline.
  // Basically the same as `fdBaraffe` in body.c.
  double res = 0.;

  if (iParam == WDWARF_T) {

    if (iOpacityModel == OPACITY_MODEL_BLOUIN) {
      if (!bHeAtm) {
        if (iMetallicityLevel == 0) {
          // b20 opacity, type DA, Z = 0 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DA_Z000_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 1) {
          // b20 opacity, type DA, Z = 0.006 (4 mass values)
          res = fdBastiInterpolate4(6, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_4, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DA_Z006_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 2) {
          // b20 opacity, type DA, Z = 0.01 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DA_Z010_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 3) {
          // b20 opacity, type DA, Z = 0.02 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DA_Z020_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 4) {
          // b20 opacity, type DA, Z = 0.03 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DA_Z030_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 5) {
          // b20 opacity, type DA, Z = 0.04 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DA_Z040_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else {
          // ?
        }
      } else {
        if (iMetallicityLevel == 0) {
          // b20 opacity, type DB, Z = 0 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DB_Z000_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 2) {
          // b20 opacity, type DB, Z = 0.01 (2 mass values)
          res = fdBastiInterpolate2(4, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_2, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DB_Z010_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 3) {
          // b20 opacity, type DB, Z = 0.02 (3 mass values)
          res = fdBastiInterpolate3(5, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_3, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DB_Z020_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 4) {
          // b20 opacity, type DB, Z = 0.03 (4 mass values)
          res = fdBastiInterpolate4(6, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_4, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DB_Z030_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 5) {
          // b20 opacity, type DB, Z = 0.04 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DB_Z040_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else {
          // ?
        }
      }
    } else if (iOpacityModel == OPACITY_MODEL_CASSISI) {
      if (!bHeAtm) {
        if (iMetallicityLevel == 0) {
          // c07 opacity, type DA, Z = 0 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DA_Z000_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 1) {
          // c07 opacity, type DA, Z = 0.006 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DA_Z006_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 2) {
          // c07 opacity, type DA, Z = 0.010 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DA_Z010_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 3) {
          // c07 opacity, type DA, Z = 0.020 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DA_Z020_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 4) {
          // c07 opacity, type DA, Z = 0.030 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DA_Z030_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 5) {
          // c07 opacity, type DA, Z = 0.040 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DA_Z040_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else {
          // ?
        }
      } else {
        if (iMetallicityLevel == 0) {
          // c07 opacity, type DB, Z = 0 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DB_Z000_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 2) {
          // c07 opacity, type DB, Z = 0.010 (3 mass values)
          res = fdBastiInterpolate3(5, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_3, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DB_Z010_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 3) {
          // c07 opacity, type DB, Z = 0.020 (4 mass values)
          res = fdBastiInterpolate4(6, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_4, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DB_Z020_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 4) {
          // c07 opacity, type DB, Z = 0.030 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DB_Z030_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 5) {
          // c07 opacity, type DB, Z = 0.040 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DB_Z040_LOGT, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        }
      }
    }

    return pow(10., res);

  } else if (iParam == WDWARF_L) {

    if (iOpacityModel == OPACITY_MODEL_BLOUIN) {
      if (!bHeAtm) {
        if (iMetallicityLevel == 0) {
          // b20 opacity, type DA, Z = 0 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DA_Z000_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 1) {
          // b20 opacity, type DA, Z = 0.006 (4 mass values)
          res = fdBastiInterpolate4(6, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_4, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DA_Z006_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 2) {
          // b20 opacity, type DA, Z = 0.01 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DA_Z010_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 3) {
          // b20 opacity, type DA, Z = 0.02 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DA_Z020_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 4) {
          // b20 opacity, type DA, Z = 0.03 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DA_Z030_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 5) {
          // b20 opacity, type DA, Z = 0.04 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DA_Z040_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else {
          // ?
        }
      } else {
        if (iMetallicityLevel == 0) {
          // b20 opacity, type DB, Z = 0 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DB_Z000_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 2) {
          // b20 opacity, type DB, Z = 0.01 (2 mass values)
          res = fdBastiInterpolate2(4, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_2, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DB_Z010_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 3) {
          // b20 opacity, type DB, Z = 0.02 (3 mass values)
          res = fdBastiInterpolate3(5, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_3, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DB_Z020_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 4) {
          // b20 opacity, type DB, Z = 0.03 (4 mass values)
          res = fdBastiInterpolate4(6, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_4, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DB_Z030_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 5) {
          // b20 opacity, type DB, Z = 0.04 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_BLOUIN_DB_Z040_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else {
          // ?
        }
      }
    } else if (iOpacityModel == OPACITY_MODEL_CASSISI) {
      if (!bHeAtm) {
        if (iMetallicityLevel == 0) {
          // c07 opacity, type DA, Z = 0 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DA_Z000_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 1) {
          // c07 opacity, type DA, Z = 0.006 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DA_Z006_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 2) {
          // c07 opacity, type DA, Z = 0.010 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DA_Z010_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 3) {
          // c07 opacity, type DA, Z = 0.020 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DA_Z020_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 4) {
          // c07 opacity, type DA, Z = 0.030 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DA_Z030_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 5) {
          // c07 opacity, type DA, Z = 0.040 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DA_Z040_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else {
          // ?
        }
      } else {
        if (iMetallicityLevel == 0) {
          // c07 opacity, type DB, Z = 0 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DB_Z000_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 2) {
          // c07 opacity, type DB, Z = 0.010 (3 mass values)
          res = fdBastiInterpolate3(5, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_3, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DB_Z010_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 3) {
          // c07 opacity, type DB, Z = 0.020 (4 mass values)
          res = fdBastiInterpolate4(6, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_4, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DB_Z020_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 4) {
          // c07 opacity, type DB, Z = 0.030 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DB_Z030_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        } else if (iMetallicityLevel == 5) {
          // c07 opacity, type DB, Z = 0.040 (7 mass values)
          res = fdBastiInterpolate7(9, WDWARF_BASTI_ALEN,
                                    WDWARF_BASTI_MARR_7, WDWARF_BASTI_AARR,
                                    BASTI_CASSISI_DB_Z040_LOGL, M / MSUN,
                                    A / (1.e9 * YEARSEC), iOrder, iError);
        }
      }
    }

    return LSUN * pow(10., res);

  } else {
    *iError = WDWARF_ERR_FILE;
    return 0;
  }
}
