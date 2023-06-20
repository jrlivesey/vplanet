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

void ReadEmptyAlbedo() {
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
    double dEmptyArea, dDBlackAreaDt;
    dEmptyArea = body[iaBody[0]].dHabitArea - body[iaBody[0]].dBlackArea -
                  body[iaBody[0]].dWhiteArea;
    dDBlackAreaDt = body[iaBody[0]].dBlackArea * (dEmptyArea *
                    body[iaBody[0]].dBlackBirthParam -
                    body[iaBody[0]].dDeathParam);
    return dDBlackAreaDt;
}

double fndDWhiteAreaDt(BODY *body, SYSTEM *system, int *iaBody) {
    // Total derivative of the normalized area covered by "white" daisies.
    double dEmptyArea, dDWhiteAreaDt;
    dEmptyArea = body[iaBody[0]].dHabitArea - body[iaBody[0]].dBlackArea -
                  body[iaBody[0]].dWhiteArea;
    dDWhiteAreaDt = body[iaBody[0]].dWhiteArea * (dEmptyArea *
                    body[iaBody[0]].dWhiteBirthParam -
                    body[iaBody[0]].dDeathParam);
    return dDBlackAreaDt;
}

double fndEmptyArea(BODY *body, SYSTEM *system, int *iaBody) {
    double dEmptyArea;
    dEmptyArea = body[iaBody[0]].dHabitArea - body[iaBody[0]].dBlackArea -
                 body[iaBody[0]].dWhiteArea;
    return dEmptyArea;
}

double fndSurfAlbedo(BODY *body, SYSTEM *system, int *iaBody) {
    // Average albedo of the planet's surface.
    double dSurfAlbedo;
    dSurfAlbedo = body[iaBody[0]].dEmptyAlbedo * fndEmptyArea(body, system, iaBody) +
                  body[iaBody[0]].dBlackAlbedo * body[iaBody[0]].dBlackArea +
                  body[iaBody[0]].dWhiteAlbedo * body[iaBody[0]].dWhiteArea;
    return dSurfAlbedo;
}

double fndSurfTemp(BODY *body, SYSTEM *system, int *iaBody) {
    // Average temperature at the planet's surface.
    // Set dSurfEnFlux = the SurfEnFluxTotal output ?
    double dSurfEnFlux, dSurfTemp;
    dSurfTemp = pow(dSurfEnFlux * (1. - fndSurfAlbedo(body, system, iaBody)) /
                SIGMA, 0.25);
    return dSurfTemp;
}

double fndBlackTemp(BODY *body, SYSTEM *system, int *iaBody) {
    // Local temperature of the "black" daisies.
    double q, dBlackTemp;
    dBlackTemp = pow(q * (fndSurfAlbedo(body, system, iaBody) -
                      body[iaBody[0]].dBlackAlbedo) *
                     pow(fndSurfTemp(body, system, iaBody), 4), 0.25);
    return dBlackTemp;
}

double fndWhiteTemp(BODY *body, SYSTEM *system, int *iaBody) {
    // Local temperature of the "white" daisies.
    double q, dWhiteTemp;
    dWhiteTemp = pow(q * (fndSurfAlbedo(body, system, iaBody) -
                      body[iaBody[0]].dWhiteAlbedo) *
                     pow(fndSurfTemp(body, system, iaBody), 4), 0.25);
    return dWhiteTemp;
}

double fndBlackBirthParam(BODY *body, SYSTEM *system, int *iaBody) {
    // Birthrate parameter for the "black" daisies.
    double dBlackBirthParam;
    dBlackBirthParam = 1. - 0.003265 * (body[iaBody[0]].dMaxTemp -
                                        body[iaBody[0]].dBlackTemp);
    return dBlackBirthParam;
}

double fndWhiteBirthParam(BODY *body, SYSTEM *system, int *iaBody) {
    // Birthrate parameter for the "white" daisies.
    double dWhiteBirthParam;
    dWhiteBirthParam = 1. - 0.003265 * (body[iaBody[0]].dMaxTemp -
                                        body[iaBody[0]].dWhiteTemp);
    return dWhiteBirthParam;
}
