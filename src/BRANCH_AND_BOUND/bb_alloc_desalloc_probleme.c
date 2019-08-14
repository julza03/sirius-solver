// Copyright (c) 20xx-2019, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// This Source Code Form is subject to the terms of the Apache License, version 2.0.
// If a copy of the Apache License, version 2.0 was not distributed with this file, you can obtain one at http://www.apache.org/licenses/LICENSE-2.0.
// SPDX-License-Identifier: Apache-2.0
// This file is part of SIRIUS, a linear problem solver, used in the ANTARES Simulator : https://antares-simulator.org/.

/***********************************************************************

   FONCTION: Desallocations finales
       
   AUTEUR: R. GONZALEZ

************************************************************************/

#include "bb_sys.h"
#include "bb_define.h"
#include "bb_fonctions.h"

#include "pne_define.h"

# ifdef BB_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "bb_memoire.h"
# endif

/*---------------------------------------------------------------------------------------------------------*/

void BB_BranchAndBoundAllouerProbleme( BB * Bb )
{

Bb->ValeursCalculeesDesVariablesPourLeProblemeRelaxeCourant         = (double *) malloc( Bb->NombreDeVariablesDuProbleme         * sizeof( double ) );
Bb->ValeursCalculeesDesVariablesEntieresPourLeProblemeRelaxeCourant = (double *) malloc( Bb->NombreDeVariablesEntieresDuProbleme * sizeof( double ) );

Bb->NumerosDesVariablesEntieresDuProbleme                           = (int *)   malloc( Bb->NombreDeVariablesEntieresDuProbleme * sizeof( int   ) );
Bb->ValeursOptimalesDesVariables                                    = (double *) malloc( Bb->NombreDeVariablesDuProbleme         * sizeof( double ) );
Bb->ValeursOptimalesDesVariablesEntieres                            = (double *) malloc( Bb->NombreDeVariablesEntieresDuProbleme * sizeof( double ) );

/* Il faut revoir ces dimensionnements. On dimensionne plus large le complement de base pour pouvoir mettre des coupes mais ce n'est pas la 
   bonne methode, il vaut mieux faire des realloc */
Bb->PositionDeLaVariableAEntierInf    = (int *) malloc( ( Bb->NombreDeVariablesDuProbleme   + Bb->NombreDeContraintesDuProbleme ) * sizeof( int ) );
Bb->ComplementDeLaBaseAEntierInf      = (int *) malloc( ( Bb->NombreDeContraintesDuProbleme + Bb->NombreDeVariablesDuProbleme   ) * sizeof( int ) );
Bb->PositionDeLaVariableAEntierSup    = (int *) malloc( ( Bb->NombreDeVariablesDuProbleme   + Bb->NombreDeContraintesDuProbleme ) * sizeof( int ) );
Bb->ComplementDeLaBaseAEntierSup      = (int *) malloc( ( Bb->NombreDeContraintesDuProbleme + Bb->NombreDeVariablesDuProbleme   ) * sizeof( int ) );

if ( Bb->ValeursCalculeesDesVariablesPourLeProblemeRelaxeCourant         == NULL || 
     Bb->ValeursCalculeesDesVariablesEntieresPourLeProblemeRelaxeCourant == NULL || 

     Bb->NumerosDesVariablesEntieresDuProbleme == NULL || 
     Bb->ValeursOptimalesDesVariables          == NULL ||
     Bb->ValeursOptimalesDesVariablesEntieres  == NULL || 

     Bb->PositionDeLaVariableAEntierInf == NULL ||
     Bb->ComplementDeLaBaseAEntierInf   == NULL ||
     Bb->PositionDeLaVariableAEntierSup == NULL ||
     Bb->ComplementDeLaBaseAEntierSup   == NULL 
) {

  printf("\n Saturation memoire dans la partie branch & bound, fonction: BB_BranchAndBoundAllouerProbleme \n");
  Bb->AnomalieDetectee = OUI;
  longjmp( Bb->EnvBB , Bb->AnomalieDetectee ); 
}

Bb->NombreDeNoeudsEvaluesSansCalculdeCoupes = 0;

/* Les zones ci-dessous sont allouees au moment de la resolution du probleme relaxe */
Bb->CoupeSaturee           = NULL;
Bb->CoupeSatureeAEntierInf = NULL;
Bb->CoupeSatureeAEntierSup = NULL;

/* Initialise a une grande valeur */
Bb->AverageG = NON_INITIALISE;
Bb->AverageI = NON_INITIALISE;
Bb->AverageK = NON_INITIALISE;

return; 

}

/*---------------------------------------------------------------------------------------------------------*/

void BB_BranchAndBoundDesallouerProbleme( BB * Bb )
{
PROBLEME_PNE * Pne; 

if ( Bb == NULL ) return;

# ifdef BB_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE
  Pne = (PROBLEME_PNE *) Bb->ProblemePneDuBb;
  Pne->ProblemeBbDuSolveur = NULL;	
  MEM_Quit( Bb->Tas );
	return;
# endif

/* On libere toute l'arborescence */
Bb->NoeudEnExamen = NULL;

if ( Bb->NoeudRacine != NULL ) {
  BB_SupprimerTousLesDescendantsDUnNoeud( Bb, Bb->NoeudRacine );				
  /* Le sp ci-dessus ne supprime pas le noeud de depart */
  BB_DesallouerUnNoeud( Bb, Bb->NoeudRacine );
}

free( Bb->ValeursCalculeesDesVariablesPourLeProblemeRelaxeCourant );
free( Bb->ValeursCalculeesDesVariablesEntieresPourLeProblemeRelaxeCourant );
free( Bb->NumerosDesVariablesEntieresDuProbleme );
free( Bb->ValeursOptimalesDesVariables );
free( Bb->ValeursOptimalesDesVariablesEntieres );
free( Bb->PositionDeLaVariableAEntierInf );
free( Bb->ComplementDeLaBaseAEntierInf );
free( Bb->PositionDeLaVariableAEntierSup );
free( Bb->ComplementDeLaBaseAEntierSup );

free( Bb->CoupeSaturee );
free( Bb->CoupeSatureeAEntierInf );
free( Bb->CoupeSatureeAEntierSup );

Pne = (PROBLEME_PNE *) Bb->ProblemePneDuBb;
Pne->ProblemeBbDuSolveur = NULL;

free( Bb );

return; 

}

