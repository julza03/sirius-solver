// Copyright (c) 20xx-2019, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// This Source Code Form is subject to the terms of the Apache License, version 2.0.
// If a copy of the Apache License, version 2.0 was not distributed with this file, you can obtain one at http://www.apache.org/licenses/LICENSE-2.0.
// SPDX-License-Identifier: Apache-2.0
// This file is part of SIRIUS, a linear problem solver, used in the ANTARES Simulator : https://antares-simulator.org/.

/***********************************************************************

   FONCTION: Verification du calcul de l'inverse de la ligne de la base 
	           correspondant a la variable sortante.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "pne_fonctions.h"

# define CYCLE_DE_VERIFICATION_DE_ERBMOINS1_HYPER_CREUX   20

# define SEUIL_DE_VERIFICATION_DE_ErBMoinsUn_1   1.e-6 /*1.e-6*/ 
# define SEUIL_DE_VERIFICATION_DE_ErBMoinsUn_1a  1.e-6 /*1.e-6*/ 
# define SEUIL_DE_VERIFICATION_DE_ErBMoinsUn_2   1.e-6 /*1.e-6*/ 
# define SEUIL_DE_VERIFICATION_DE_ErBMoinsUn_2a  1.e-6 /*1.e-6*/  
# define NOMBRE_DE_VERIFICATIONS                100   /*10*/ 

# define NBITER_RAFFINEMENT    0
# define NBITER_RAFFINEMENT_a  0

//# define Spx->spx_params->VERBOSE_SPX 0

/*----------------------------------------------------------------------------*/
/*           Verification du calcule de la ligne de B^{-1} qui 
             correspond a la variable sortante: on verifie sur k contraintes 
             tirees au hasard  		 			              */

void SPX_DualVerifierErBMoinsUn( PROBLEME_SPX * Spx )

{
int Cnt; int Var; int il; int ilMax; double S; int NbFois; int Nombre; double X;
int * VariableEnBaseDeLaContrainte; int * Cdeb; int * CNbTerm; int * NumeroDeContrainte;
double * ACol; double * ErBMoinsUn; char Imprecision; int VariableSortante;
int NombreDeContraintes;  int NbTermesNonNulsDeErBMoinsUn; int * IndexTermesNonNulsDeErBMoinsUn; int j;
int * T; double * Bs; char * PositionDeLaVariable; int * Mdeb ; int * NbTerm; int * Indcol; double * A;
int * ContrainteDeLaVariableEnBase; int NombreDeValeursNonNulles; int * NumCntNonNul;
int CntCol; int CntBase; char CntBaseControle;

/* 
printf("Attention verif SPX_DualVerifierErBMoinsUn Iteration %d\n",Spx->Iteration); 
*/

/* Si on est en stockage hyper creux on verifie tout le vecteur */
if ( Spx->TypeDeStockageDeErBMoinsUn != COMPACT_SPX ) goto CCC;

if ( Spx->Iteration % CYCLE_DE_VERIFICATION_DE_ERBMOINS1_HYPER_CREUX != 0 ) return;

VariableSortante = Spx->VariableSortante;

ErBMoinsUn = Spx->ErBMoinsUn;
IndexTermesNonNulsDeErBMoinsUn = Spx->IndexTermesNonNulsDeErBMoinsUn;
NbTermesNonNulsDeErBMoinsUn = Spx->NbTermesNonNulsDeErBMoinsUn;
T = Spx->T;
Bs = Spx->Bs;

Mdeb = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol  = Spx->Indcol; 
A       = Spx->A;

PositionDeLaVariable = Spx->PositionDeLaVariable;

ContrainteDeLaVariableEnBase = Spx->ContrainteDeLaVariableEnBase;

NumCntNonNul = (int *) Spx->V;
CntBase = ContrainteDeLaVariableEnBase[VariableSortante];

NombreDeValeursNonNulles = 0;
for ( j = 0 ; j < NbTermesNonNulsDeErBMoinsUn ; j++ ) {
  X = ErBMoinsUn[j];
  Cnt = IndexTermesNonNulsDeErBMoinsUn[j];						
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
	  if ( PositionDeLaVariable[Indcol[il]] == EN_BASE_LIBRE ) {
	    CntCol = ContrainteDeLaVariableEnBase[Indcol[il]];
      if ( T[CntCol] == 1 ) {
			  Bs[CntCol] += X * A[il];			
      }
		  else {
			  T[CntCol] = 1;
		 	  Bs[CntCol] = X * A[il];
			  NumCntNonNul[NombreDeValeursNonNulles] = CntCol;
        NombreDeValeursNonNulles++;			
		  }
		}
    il++;
	}
}

/*for ( j = 0 ; j < NombreDeValeursNonNulles ; j++ ) T[NumCntNonNul[j]] = -1;*/

CntBaseControle = NON_SPX;

for ( j = 0 ; j < NombreDeValeursNonNulles ; j++ ) {	
  CntCol = NumCntNonNul[j];
  X = Bs[CntCol];
	if ( CntCol == CntBase ) {
	  CntBaseControle = OUI_SPX;
	  S = 1.;
	}
	else S = 0.0;
  if ( fabs( X - S ) > SEUIL_DE_VERIFICATION_DE_ErBMoinsUn_1 ) {			
	  Imprecision = OUI_SPX;
    if ( Spx->NombreDeChangementsDeBase < 10 ) {
		  /* Si ca se produit dans les premieres iterations apres une factorisation */
		  Spx->FlagStabiliteDeLaFactorisation = 1;   			
		}		
	if (Spx->spx_params->VERBOSE_SPX) {
		printf("Controle Hyper Creux SPX_DualVerifierErBMoinsUn Iteration %d Phase %d erreur de resolution sur ErBMoinsUn: %e",
			Spx->Iteration, (int)Spx->PhaseEnCours, fabs(X - S));
		printf(" ecart trop grand on refactorise la base    Spx->StrongBranchingEnCours %d\n", (int)Spx->StrongBranchingEnCours);
	}
		Spx->FactoriserLaBase = OUI_SPX;
		CntBaseControle = OUI_SPX;
	  /* On augmente le seuil dual de pivotage */			  
		Spx->SeuilDePivotDual = Spx->spx_params->COEFF_AUGMENTATION_VALEUR_DE_PIVOT_ACCEPTABLE * Spx->spx_params->VALEUR_DE_PIVOT_ACCEPTABLE;    		
		if (Spx->spx_params->VERBOSE_SPX) {
			printf("SeuilDePivotDual -> %e\n", Spx->SeuilDePivotDual);
		}
    Spx->FaireDuRaffinementIteratif = NBITER_RAFFINEMENT;
    if ( fabs( X - S ) > SEUIL_DE_VERIFICATION_DE_ErBMoinsUn_1a ) {			
      Spx->FaireDuRaffinementIteratif = NBITER_RAFFINEMENT_a;
		  Spx->FaireChangementDeBase = NON_SPX;
		  if (Spx->spx_params->VERBOSE_SPX) {
			  printf("Le changement de base est refuse\n");
		  }
		}		
		break;
  } 
}
if ( CntBaseControle == NON_SPX ) {
	if (Spx->spx_params->VERBOSE_SPX) {
		printf("Controle Hyper Creux SPX_DualVerifierErBMoinsUn Iteration %d erreur de resolution sur ErBMoinsUn pour la contrainte basique\n",
			Spx->Iteration);
		printf("NombreDeValeursNonNulles de Bs %d\n", NombreDeValeursNonNulles);
	}
	Spx->FactoriserLaBase = OUI_SPX;
	Spx->FaireDuRaffinementIteratif = NBITER_RAFFINEMENT;
	/* On augmente le seuil dual de pivotage */
 	Spx->SeuilDePivotDual = Spx->spx_params->COEFF_AUGMENTATION_VALEUR_DE_PIVOT_ACCEPTABLE * Spx->spx_params->VALEUR_DE_PIVOT_ACCEPTABLE;
	if (Spx->spx_params->VERBOSE_SPX) {
		printf("SeuilDePivotDual -> %e\n", Spx->SeuilDePivotDual);
	}
	Spx->FaireChangementDeBase = NON_SPX;		
}

for ( j = 0 ; j < NombreDeValeursNonNulles ; j++ ) {
  Cnt = NumCntNonNul[j];
  T[Cnt] = -1;
	Bs[Cnt] = 0;
}

return;

CCC:

VariableEnBaseDeLaContrainte = Spx->VariableEnBaseDeLaContrainte;

Cdeb    = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;
ACol    = Spx->ACol;
NumeroDeContrainte = Spx->NumeroDeContrainte;

NombreDeContraintes = Spx->NombreDeContraintes;
ErBMoinsUn          = Spx->ErBMoinsUn;
VariableSortante    = Spx->VariableSortante;
Imprecision         = NON_SPX;

NbFois = 0;

while ( NbFois < NOMBRE_DE_VERIFICATIONS ) { 
  /* On tire un nombre au hasard compris entre 0 et NombreDeContraintes - 1 */

	if (Spx->spx_params->UTILISER_PNE_RAND == OUI_SPX) {
		Spx->A1 = PNE_Rand(Spx->A1);
		X = Spx->A1 * (NombreDeContraintes - 1);
	}
	else {
		X = rand() * Spx->UnSurRAND_MAX * (NombreDeContraintes - 1);
	}
	
  Nombre = (int) X;
  if ( Nombre >= NombreDeContraintes ) Nombre = NombreDeContraintes - 1; 
  Cnt = Nombre;
  Var = VariableEnBaseDeLaContrainte[Cnt];
  /* Verification */
  S = 0.;
  if ( Var == VariableSortante ) S = -1.;
  il    = Cdeb[Var];
  ilMax = il + CNbTerm[Var];
  while ( il < ilMax ) {
    S+= ACol[il] * ErBMoinsUn[NumeroDeContrainte[il]];
    il++;
  }
  if ( fabs( S ) > SEUIL_DE_VERIFICATION_DE_ErBMoinsUn_1 ) {			
	  Imprecision = OUI_SPX;
    if ( Spx->NombreDeChangementsDeBase < 10 ) {
		  /* Si ca se produit dans les premieres iterations apres une factorisation */
		  Spx->FlagStabiliteDeLaFactorisation = 1;   			
		}
		break;
  } 
  NbFois++;
}

if ( Imprecision == OUI_SPX ) {
  /* Suspiscion: on recalcule l'ecart moyen pour voir s'il faut refactoriser */
	S = 0.0;
  for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
    Var = VariableEnBaseDeLaContrainte[Cnt];
    X = 0.;
    if ( Var == VariableSortante ) X = -1.;
    il    = Cdeb[Var];
    ilMax = il + CNbTerm[Var];
    while ( il < ilMax ) {
      X+= ACol[il] * ErBMoinsUn[NumeroDeContrainte[il]];
      il++;
    }
	  S+= fabs( X );
	}  
  S/= NombreDeContraintes;
  if ( S > SEUIL_DE_VERIFICATION_DE_ErBMoinsUn_2 && Spx->NombreDeChangementsDeBase > 0 ) {
	  if (Spx->spx_params->VERBOSE_SPX) {
		  printf("SPX_DualVerifierErBMoinsUn Iteration %d Phase %d erreur de resolution sur ErBMoinsUn: %e",
			  Spx->Iteration, (int)Spx->PhaseEnCours, fabs(S));
		  printf(" ecart trop grand on refactorise la base \n");
	  }
		Spx->FactoriserLaBase = OUI_SPX;
		/* On augmente le seuil dual de pivotage */ 	  
		Spx->SeuilDePivotDual = Spx->spx_params->COEFF_AUGMENTATION_VALEUR_DE_PIVOT_ACCEPTABLE * Spx->spx_params->VALEUR_DE_PIVOT_ACCEPTABLE;    
		if (Spx->spx_params->VERBOSE_SPX) {
			printf("SeuilDePivotDual -> %e\n", Spx->SeuilDePivotDual);
		}
    Spx->FaireDuRaffinementIteratif = NBITER_RAFFINEMENT;
    if ( S > SEUIL_DE_VERIFICATION_DE_ErBMoinsUn_2a ) {
      Spx->FaireDuRaffinementIteratif = NBITER_RAFFINEMENT_a;
	    Spx->FaireChangementDeBase = NON_SPX;
		if (Spx->spx_params->VERBOSE_SPX) {
			printf("Le changement de base est refuse\n");
		}
		}
  }
	else Spx->FaireDuRaffinementIteratif = NBITER_RAFFINEMENT;
}
 
return;
}



