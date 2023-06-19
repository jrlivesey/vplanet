/**
   @file daisy.c

   @brief Subroutines that control the integration of the Daisyworld surface coverage model.

   @author Joseph R. Livesey ([jrlivesey](https://github.com/jrlivesey/))

   @date 2023

*/

/*
PRIMARY VARIABLES
-----------------
dBlackArea: normalized coverage of "black" daisies
dWhiteArea: normalized coverage of "white" daisies
*/

#include "vplanet.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void BodyCopyDaisy(BODY *dest, BODY *src, int iNumBodies, int iBody) {
    // copy body params
}

void InitializeBodyDaisy(BODY *body, CONTROL *control, UPDATE *update,
                         int iBody) {
    if (body[iBody].iDaisy) {
        // allocate memory in BODY
    }
}

/* READ OPTIONS */
void ReadBlackAlbedo() {
}

void ReadWhiteAlbedo() {
}

void ReadBarrenAlbedo() {
}

void ReadHabitArea() {
}

void ReadInitBlackArea() {
}

void ReadInitWhiteArea() {
}

void ReadDeathParam() {
}

void InitializeOptionsDaisy() {
}

/* WRITE OUTPUTS */
void WriteBlackArea() {
}

void WriteWhiteArea() {
}

void InitializeOutputDaisy() {
}

/* FUNCTIONS FOR CALCULATION */
double fndDBlackAreaDt(BODY *body, SYSTEM *system, int *iaBody) {
    // Total derivative of the normalized area covered by "black" daisies.
    double dBarrenArea, dDBlackAreaDt;
    dBarrenArea = body[iaBody[0]].dHabitArea - body[iaBody[0]].dBlackArea -
                  body[iaBody[0]].dWhiteArea;
    dDBlackAreaDt = body[iaBody[0]].dBlackArea * (dBarrenArea *
                    body[iaBody[0]].dBlackBirthParam -
                    body[iaBody[0]].dDeathParam);
    return dDBlackAreaDt;
}

double fndDWhiteAreaDt(BODY *body, SYSTEM *system, int *iaBody) {
    // Total derivative of the normalized area covered by "white" daisies.
    double dBarrenArea, dDWhiteAreaDt;
    dBarrenArea = body[iaBody[0]].dHabitArea - body[iaBody[0]].dBlackArea -
                  body[iaBody[0]].dWhiteArea;
    dDWhiteAreaDt = body[iaBody[0]].dWhiteArea * (dBarrenArea *
                    body[iaBody[0]].dWhiteBirthParam -
                    body[iaBody[0]].dDeathParam);
    return dDBlackAreaDt;
}

double fndSurfAlbedo(BODY *body, SYSTEM *system, int *iaBody) {
    // Average albedo of the planet's surface.

}

double fndSurfTemp(BODY *body, SYSTEM *system, int *iaBody) {
    // Average temperature at the planet's surface.
    // Set dSurfEnFlux = the SurfEnFluxTotal output ?
    double dSurfEnFlux, dBarrenArea, dSurfAlbedo, dSurfTemp;
    dBarrenArea = body[iaBody[0]].dHabitArea - body[iaBody[0]].dBlackArea -
                  body[iaBody[0]].dWhiteArea;
    dSurfAlbedo = body[iaBody[0]].dBarrenAlbedo * dBarrenArea +
                  body[iaBody[0]].dBlackAlbedo * body[iaBody[0]].dBlackArea +
                  body[iaBody[0]].dWhiteAlbedo * body[iaBody[0]].dWhiteArea;
    dSurfTemp = pow(dSurfEnFlux * (1. - dSurfAlbedo) / SIGMA, 0.25);
    return dSurfTemp;
}

double fndBlackBirthParam(BODY *body, SYSTEM *system, int *iaBody) {
    
}