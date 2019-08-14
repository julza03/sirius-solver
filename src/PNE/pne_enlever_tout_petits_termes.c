// Copyright (c) 20xx-2019, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// This Source Code Form is subject to the terms of the Apache License, version 2.0.
// If a copy of the Apache License, version 2.0 was not distributed with this file, you can obtain one at http://www.apache.org/licenses/LICENSE-2.0.
// SPDX-License-Identifier: Apache-2.0
// This file is part of SIRIUS, a linear problem solver, used in the ANTARES Simulator : https://antares-simulator.org/.

/***********************************************************************
   FONCTION: Construction du probleme
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"

# define RAPPORT_ECRETEMENT_PETITS_TERMES 1.e+15
# define RAPPORT_ECRETEMENT_PETITS_TERMES_PAR_CONTRAINTE 1.e+12
# define SEUIL_ALERTE_ECRETEMENT 1.e-6  
# define SEUIL_ECRETEMENT_1 1.e-10
# define SEUIL_ECRETEMENT_2 5.e-8

/*----------------------------------------------------------------------------*/
/*   Si le rapport Max/Min est tres grand on enleve les tout petits termes    */

void PNE_EnleverLesToutPetitsTermes( int * Mdeb, int * NbTerm, int * Indcol,
                                     double * A, double * Xmax, double * Xmin,
																		 int NombreDeContraintes, char AffichageDesTraces )
{
int Cnt; int il; int ilMax; double PlusGrandTerme; double PlusPetitTerme;
double X; char OnSupprime; int ilDeb; char ToutPetitsTermes;

ToutPetitsTermes = NON_PNE;
PlusGrandTerme = -1;
PlusPetitTerme = LINFINI_SPX;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  il    = Mdeb[Cnt]; 
  ilMax = il + NbTerm[Cnt]; 
  while ( il < ilMax ) {
    X = fabs( A[il] );
    if( X != 0.0 ) {
      if ( X > PlusGrandTerme ) PlusGrandTerme = X;
      if ( X < PlusPetitTerme ) PlusPetitTerme = X;
    }
    il++;   
  }    
}

if ( PlusGrandTerme / PlusPetitTerme < RAPPORT_ECRETEMENT_PETITS_TERMES ) return;

/*
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  il = Mdeb[Cnt]; 
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) { 
    X = fabs( A[il] );
    if( X < SEUIL_ALERTE_ECRETEMENT ) {
		  OnSupprime = NON_SPX;
      if ( X < SEUIL_ECRETEMENT_1 ) {			  
				printf("Suppression d'un terme trop petit A fois delta X = %e SEUIL_ECRETEMENT_1 = %e\n",X * ( Xmax[Indcol[il]] - Xmin[Indcol[il]]),SEUIL_ECRETEMENT_1);			  
				OnSupprime = OUI_SPX;
			}
			else if ( X * ( Xmax[Indcol[il]] - Xmin[Indcol[il]] ) < SEUIL_ECRETEMENT_2 ) {			  
				printf("Suppression d'un terme trop petit A fois delta X = %e SEUIL_ECRETEMENT_2 = %e\n",X * ( Xmax[Indcol[il]] - Xmin[Indcol[il]]),SEUIL_ECRETEMENT_2);			  
				OnSupprime = OUI_SPX;
			}
			if ( OnSupprime == OUI_SPX ) {
			  if ( NbTerm[Cnt] > 1 ) {					
				  ilMax--;
				  A[il] = A[ilMax];
				  Indcol[il] = Indcol[ilMax];
				  NbTerm[Cnt]--;
				  continue;
				}
			}
    }
    il++;
  }    
}
*/

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  il = Mdeb[Cnt];
	ilDeb = il;
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) { 
    X = fabs( A[il] );
    if( X < SEUIL_ALERTE_ECRETEMENT ) {
		  /* On regarde si on peut supprimer quelque chose a condition qu'il n'y ait pas que des petits termes dans la contrainte */
      PlusGrandTerme = -1;
      PlusPetitTerme = LINFINI_SPX;
	    il = ilDeb;
      while ( il < ilMax ) {
        X = fabs( A[il] );
        if( X != 0.0 ) {
          if ( X > PlusGrandTerme ) PlusGrandTerme = X;
          if ( X < PlusPetitTerme ) PlusPetitTerme = X;
        }
        il++;
			}		 
      if ( PlusGrandTerme / PlusPetitTerme < RAPPORT_ECRETEMENT_PETITS_TERMES_PAR_CONTRAINTE ) {
			  /* On prefere tout garder */
				break;
			}
      /* On essai d'enlever les petits termes */			          
      il = ilDeb; 
      while ( il < ilMax ) { 
        X = fabs( A[il] );
        if( X < SEUIL_ALERTE_ECRETEMENT ) {			
		      OnSupprime = NON_SPX;
          if ( X < SEUIL_ECRETEMENT_1 ) {
			      /*
						printf("Suppression d'un terme trop petit A fois delta X = %e SEUIL_ECRETEMENT_1 = %e\n",X * ( Xmax[Indcol[il]] - Xmin[Indcol[il]]),SEUIL_ECRETEMENT_1);
			      */
						OnSupprime = OUI_SPX;
					  ToutPetitsTermes = OUI_PNE;
			    }
			    else if ( X * ( Xmax[Indcol[il]] - Xmin[Indcol[il]] ) < SEUIL_ECRETEMENT_2 ) {
			      /*
						printf("Suppression d'un terme trop petit A fois delta X = %e SEUIL_ECRETEMENT_2 = %e\n",X * ( Xmax[Indcol[il]] - Xmin[Indcol[il]]),SEUIL_ECRETEMENT_2);
					  */
						ToutPetitsTermes = OUI_PNE;
			      OnSupprime = OUI_SPX;
			    }
			    if ( OnSupprime == OUI_SPX ) {
					  /* Il est preferable de mettre 0 */
						A[il] = 0.0;
						/*
			      if ( NbTerm[Cnt] > 1 ) {					
				      ilMax--;
				      A[il] = A[ilMax];
				      Indcol[il] = Indcol[ilMax];
				      NbTerm[Cnt]--;
				      continue;
				    }
						*/
					}
				}
				il++;
			}
			break;
    }
    il++;
  }    
}

if ( AffichageDesTraces == OUI_PNE ) {
  if ( ToutPetitsTermes == OUI_PNE ) {
    printf("Constraints containing terms very close to zero where detected, making the problem harder to solve:\n");
 	  printf(".. whenever possible, please keep an eye on this issue when computing your constraints coefficients\n"); 
  }
}

return;
}

