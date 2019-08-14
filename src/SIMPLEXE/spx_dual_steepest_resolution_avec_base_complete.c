// Copyright (c) 20xx-2019, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// This Source Code Form is subject to the terms of the Apache License, version 2.0.
// If a copy of the Apache License, version 2.0 was not distributed with this file, you can obtain one at http://www.apache.org/licenses/LICENSE-2.0.
// SPDX-License-Identifier: Apache-2.0
// This file is part of SIRIUS, a linear problem solver, used in the ANTARES Simulator : https://antares-simulator.org/.

/***********************************************************************

   FONCTION: 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"  

# include "lu_define.h"
# include "lu_fonctions.h"

/*----------------------------------------------------------------------------*/

void SPX_DualSteepestEdgeResolutionAvecBaseComplete( PROBLEME_SPX * Spx, double * BetaPReturn,
                                                     char ResolutionEnHyperCreux, char * ResetRefSpace,
																								     int * NbTermesNonNulsDeTau, char * StockageDeTau,
																								     char * LeSystemeAEteResolu )
{
int Var; int il; int ilMax; char Save; double BetaP; double X; char ResoudreLeSysteme; 
char SecondMembreCreux; int * Cdeb; int * CNbTerm; double * NBarreR; char * InDualFramework; 
int * NumerosDesVariablesHorsBase ; int * NumeroDeContrainte; int i; double * ACol; double * Tau;
char TypeDEntree; char TypeDeSortie; int j; int k; int * NumVarNBarreRNonNul; int * T;
int * IndexTermesNonNulsDeTau; int NbTermesNonNuls; char HyperCreuxInitial;

*ResetRefSpace = NON_SPX;
BetaP = *BetaPReturn;
 
T = Spx->T;
IndexTermesNonNulsDeTau = (int *) Spx->ErBMoinsUn;  	

InDualFramework              = Spx->InDualFramework;
NumerosDesVariablesHorsBase  = Spx->NumerosDesVariablesHorsBase;
NBarreR                      = Spx->NBarreR;
Tau                          = Spx->Tau;

Cdeb               = Spx->Cdeb;
CNbTerm            = Spx->CNbTerm;
NumeroDeContrainte = Spx->NumeroDeContrainte;
ACol               = Spx->ACol;

ResoudreLeSysteme = NON_SPX;
*LeSystemeAEteResolu = ResoudreLeSysteme; 
NbTermesNonNuls = 0 ;

if ( Spx->TypeDeStockageDeNBarreR == ADRESSAGE_INDIRECT_SPX ) {
	NumVarNBarreRNonNul = Spx->NumVarNBarreRNonNul;
  if ( ResolutionEnHyperCreux == OUI_SPX ) {
 	  *StockageDeTau = ADRESSAGE_INDIRECT_SPX;			
    /*NbTermesNonNuls = 0;*/
    for ( j = 0 ; j < Spx->NombreDeValeursNonNullesDeNBarreR ; j++ ) {
      Var = NumVarNBarreRNonNul[j];
      if ( InDualFramework[Var] == NON_SPX ) continue;			
      X = NBarreR[Var];
	    if ( X == 0.0 ) continue;
      /* Calcul sur les variables hors base qui sont dans le framework */
      ResoudreLeSysteme = OUI_SPX;
      BetaP += X * X; 
      il    = Cdeb[Var];
      ilMax = il + CNbTerm[Var];
      while ( il < ilMax ) {
	      k = NumeroDeContrainte[il];
		    if ( T[k] == -1 ) {
		      T[k] = 1;
		      IndexTermesNonNulsDeTau[NbTermesNonNuls] = k;
		      NbTermesNonNuls++;				
		    }
        Tau[k] += X * ACol[il];
	      il++;
      }				  
    }
	if ( NbTermesNonNuls != 0 ) ResoudreLeSysteme = OUI_SPX;
    for ( j = 0 ; j < NbTermesNonNuls ; j++ ) T[IndexTermesNonNulsDeTau[j]] = -1;
		
	TypeDEntree  = ADRESSAGE_INDIRECT_LU;
	TypeDeSortie = ADRESSAGE_INDIRECT_LU;			
	
	}		  
	else {
    /* Pas de resolution hyper creux */
 	  *StockageDeTau = VECTEUR_SPX;	
	  ResolutionEnHyperCreux = NON_SPX;		
    for ( j = 0 ; j < Spx->NombreDeValeursNonNullesDeNBarreR ; j++ ) {
      Var = NumVarNBarreRNonNul[j];
      if ( InDualFramework[Var] == NON_SPX ) continue;			
      X = NBarreR[Var];
		  if ( X == 0.0 ) continue;
      /* Calcul sur les variables hors base qui sont dans le framework */
      ResoudreLeSysteme = OUI_SPX;
      BetaP+= X * X; 
      il    = Cdeb[Var];
      ilMax = il + CNbTerm[Var];
      while ( il < ilMax ) {
        Tau[NumeroDeContrainte[il]]+= X * ACol[il];
	      il++;
      }				  
    }

    TypeDEntree  = VECTEUR_LU;
	TypeDeSortie = VECTEUR_LU;
	
	}
}
else {
  /* Pas de resolution hyper creux */
 	*StockageDeTau = VECTEUR_SPX;
	ResolutionEnHyperCreux = NON_SPX;	
  for ( i = 0 ; i < Spx->NombreDeVariablesHorsBase ; i++ ) {
    Var = NumerosDesVariablesHorsBase[i];
    if ( InDualFramework[Var] == NON_SPX ) continue;	
    if ( NBarreR[Var] == 0.0 ) continue;	
    X = NBarreR[Var];
    /* Calcul sur les variables hors base qui sont dans le framework */
    ResoudreLeSysteme = OUI_SPX;
    BetaP+= X * X; 
    /*        */
    il    = Cdeb[Var];
    ilMax = il + CNbTerm[Var];
    while ( il < ilMax ) {
      Tau[NumeroDeContrainte[il]]+= X * ACol[il];
      il++;
    }
  }	
  
  TypeDEntree  = VECTEUR_LU;
  TypeDeSortie = VECTEUR_LU;	
  
}     

*LeSystemeAEteResolu = ResoudreLeSysteme;
HyperCreuxInitial = ResolutionEnHyperCreux;

SPX_DualSteepestControleDuPoidsDeCntBase( Spx, BetaP, ResetRefSpace );
if ( *ResetRefSpace == OUI_SPX ) {
  return;
}

/* Dynamic steepest edge: si la variable sortante est une variable basique artificielle on l'enleve de 
   l'espace de reference */
SPX_DualSteepestVariablesDeBornesIdentiques( Spx, &BetaP ); 

*BetaPReturn = BetaP;

/* Resolution du systeme Tau = B^{-1} Tau */
if ( ResoudreLeSysteme == OUI_SPX ) {
  Save = NON_LU;
  SecondMembreCreux = OUI_LU;
	
  if ( ResolutionEnHyperCreux == OUI_SPX ) {
	  if ( NbTermesNonNuls >= Spx->spx_params->TAUX_DE_REMPLISSAGE_POUR_VECTEUR_HYPER_CREUX * Spx->NombreDeContraintes ) {
		  ResolutionEnHyperCreux = NON_SPX;
 	    *StockageDeTau = VECTEUR_SPX;
		  TypeDeSortie = VECTEUR_LU;	
		}
	}
	
  SPX_ResoudreBYegalA( Spx, TypeDEntree, Tau, IndexTermesNonNulsDeTau, &NbTermesNonNuls, &TypeDeSortie, 												
                       ResolutionEnHyperCreux, Save, SecondMembreCreux );
	
  SPX_DualSteepestGestionIndicateursHyperCreux( Spx, ResolutionEnHyperCreux, HyperCreuxInitial, TypeDeSortie,
	                                              StockageDeTau, ResetRefSpace );
	if ( *ResetRefSpace == OUI_SPX ) return;
}

*NbTermesNonNulsDeTau = NbTermesNonNuls;

if ( TypeDeSortie == ADRESSAGE_INDIRECT_LU ) *StockageDeTau = ADRESSAGE_INDIRECT_SPX;
else if ( TypeDeSortie == VECTEUR_LU ) *StockageDeTau = VECTEUR_SPX;
else {
  printf("BUG dans SPX_DualSteepestEdgeResolutionAvecBaseComplete TypeDeSortie = %d est incorrect\n",TypeDeSortie);
  exit(0);
}
return;
}

