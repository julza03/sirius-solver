// Copyright (c) 20xx-2019, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// This Source Code Form is subject to the terms of the Apache License, version 2.0.
// If a copy of the Apache License, version 2.0 was not distributed with this file, you can obtain one at http://www.apache.org/licenses/LICENSE-2.0.
// SPDX-License-Identifier: Apache-2.0
// This file is part of SIRIUS, a linear problem solver, used in the ANTARES Simulator : https://antares-simulator.org/.

/***********************************************************************

   FONCTION: Pour les variables duales dont une seule ou aucune borne n'est
	           connue on cherche des variables qui permettent d'etablir la
						 borne manquante.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# define TRACES 0
# define MAX_ITER 10

/*----------------------------------------------------------------------------*/

void PRS_BornerLesVariablesDualesNonBornees( PRESOLVE * Presolve, int * NombreDeBornesCalculees )
{
int NbIter; int Nb; int il; int ilMax; int Cnt; int NombreDeContraintes; int Var;
char TypeBrn; char BorneInfCalculee; char BorneSupCalculee; double BorneInf; double BorneSup;
double * BorneInfPourPresolve; double * BorneInfNative; double * BorneSupPourPresolve;
double * BorneSupNative; int * Mdeb; int * NbTerm; int * Nuvar; int * TypeDeBorneNative;
int * TypeDeBornePourPresolve;  char * ContrainteInactive; char * ConnaissanceDeLambda;
char * ConserverLaBorneInfDuPresolve; char * ConserverLaBorneSupDuPresolve; double * A;
char UneBorneAEteCalculee; char * TypeDeValeurDeBorneInf; char * TypeDeValeurDeBorneSup;
PROBLEME_PNE * Pne;

*NombreDeBornesCalculees = 0;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

NombreDeContraintes = Pne->NombreDeContraintesTrav;   
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;

TypeDeBorneNative = Pne->TypeDeBorneTrav;
BorneInfNative = Pne->UminTrav;
BorneSupNative = Pne->UmaxTrav;

ContrainteInactive = Presolve->ContrainteInactive;
ConnaissanceDeLambda = Presolve->ConnaissanceDeLambda;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
TypeDeValeurDeBorneInf = Presolve->TypeDeValeurDeBorneInf;
TypeDeValeurDeBorneSup = Presolve->TypeDeValeurDeBorneSup;
BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;
ConserverLaBorneInfDuPresolve = Presolve->ConserverLaBorneInfDuPresolve;
ConserverLaBorneSupDuPresolve = Presolve->ConserverLaBorneSupDuPresolve;

NbIter = 0;
while ( NbIter < MAX_ITER ) {
  NbIter++;
  Nb = *NombreDeBornesCalculees;
  for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
    if ( ContrainteInactive[Cnt] == OUI_PNE ) continue;
		if ( ConnaissanceDeLambda[Cnt] == LAMBDA_CONNU ) continue;
		if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_ET_MAX_CONNU ) continue;

    il    = Mdeb[Cnt];
    ilMax = il + NbTerm[Cnt];
    while ( il < ilMax ) {
		  if ( A[il] == 0 ) goto NextIl;			 
      Var = Nuvar[il];
      if ( TypeDeBornePourPresolve[Var] == VARIABLE_FIXE ) goto NextIl;			
      TypeBrn = PRS_ChoisirLaBorneLaPlusContraignantePourLesVariablesDuales( 
                      TypeDeBorneNative[Var], TypeDeBornePourPresolve[Var],
										  BorneInfPourPresolve[Var], BorneInfNative[Var],
										  BorneSupPourPresolve[Var], BorneSupNative[Var],
										  ConserverLaBorneInfDuPresolve[Var], ConserverLaBorneSupDuPresolve[Var],
										  TypeDeValeurDeBorneInf[Var], TypeDeValeurDeBorneSup[Var] );     											
      if ( TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES ) goto NextIl;			
      /* On peut etablir une contrainte sur le cout reduit */			
      PRS_CalculeBorneSurVariableDualeEnFonctionDeLaVariable( Presolve, Cnt, Var, TypeBrn,
																														  &BorneInfCalculee, &BorneInf,
																					                    &BorneSupCalculee, &BorneSup );	
      PRS_MettreAJourLesBornesDUneVariableDuale( Presolve, Cnt,
																		             BorneInfCalculee, BorneInf, BorneSupCalculee, BorneSup,
                                                 &UneBorneAEteCalculee );																			
      if ( UneBorneAEteCalculee == OUI_PNE ) *NombreDeBornesCalculees = *NombreDeBornesCalculees + 1;
      NextIl:
	    il++;
    }		
  }
	if ( *NombreDeBornesCalculees == Nb ) break;	
}

#if VERBOSE_PRS
  printf("-> Nombre de bornes duales infinies qui ont pu etre ramenees a une valeur finie: %d\n",*NombreDeBornesCalculees);	
#endif

return;
}
