// Copyright (c) 20xx-2019, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// This Source Code Form is subject to the terms of the Apache License, version 2.0.
// If a copy of the Apache License, version 2.0 was not distributed with this file, you can obtain one at http://www.apache.org/licenses/LICENSE-2.0.
// SPDX-License-Identifier: Apache-2.0
// This file is part of SIRIUS, a linear problem solver, used in the ANTARES Simulator : https://antares-simulator.org/.

/***********************************************************************

   FONCTION: Heuristique, on fixe des variables peu fractionnaires et
             on resout le probleme dans l'espoir de trouver une solution
             entiere.
						 Attention, lorsque ce module est appele les variables
						 UminTrav UmaxTrav et TypeDeBorneTrav on ete remises a leur
						 valeur du noeud racine. Donc leurs valeurs n'inclut pas
						 les instanciations faites en amont du noeud a partir duquel
						 on demarre.
						 Par contre les structures du simplexe se trouvent dans l'etat
						 du dernier noeud resolu (bornes et types de bornes).						 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"
   
# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"  

# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define TRACES 1

/*----------------------------------------------------------------------------*/

void PNE_HeuristiquePilotage( PROBLEME_PNE * Pne, char * PasDeSolutionEntiereDansSousArbre ) 
{
BB * Bb; CONTROLS * ControlsDuProblemeEnCours; char ProblemeTropGros; 
int ExistenceDUneSolution;

*PasDeSolutionEntiereDansSousArbre = NON_PNE;
  
ExistenceDUneSolution = NON_PNE;

return;
  
if ( Pne->PremFrac < 0 ) return;

ControlsDuProblemeEnCours = Pne->Controls;
if ( ControlsDuProblemeEnCours != NULL ) {
  /* On est deja dans un sous probleme: pas d'heuristique */
  return;
}

/* Test */
if ( Pne->YaUneSolutionEntiere == OUI_PNE ) return;
Bb = (BB *) Pne->ProblemeBbDuSolveur;
if ( Bb->NoeudEnExamen->ProfondeurDuNoeud > 0 ) return;
if ( Pne->NombreDeVariablesAValeurFractionnaire > 50 ) {  
  if ( Pne->NombreDeVariablesAValeurFractionnaire > 0.1 * Pne->NombreDeVariablesEntieresNonFixes ) {
	  return;
	}
}
/* Fin test */

/* Si le cout du noeud relaxe est trop grand on a toutes les chances de trouver une
   solution trop chere */
if ( Pne->Critere > 0.9999 * Pne->CoutOpt && Pne->Critere > Pne->CoutOpt - 10 ) {
  printf(" critere pas assez en dessous de cout opt pour lancer une heuristique\n");
  return;
}

Bb = (BB *) Pne->ProblemeBbDuSolveur;

if ( Pne->pne_params->RELANCE_PERIODIQUE_DU_SIMPLEXE_AU_NOEUD_RACINE == OUI_PNE ) {
	if (Bb->NombreDeProblemesResolus % 30 == 0) {
		PNE_RelanceDuSimplexeAuNoeudRacine(Pne, &ExistenceDUneSolution);
		if (ExistenceDUneSolution == OUI_PNE) {
			printf("Relance ReducedCostFixingAuNoeudRacine \n");
			PNE_ReducedCostFixingAuNoeudRacine(Pne);
		}
	}
}

if ( Bb->NoeudEnExamen->ProfondeurDuNoeud > 2 && 0 ) return;

/* Au dela d'une certaine profondeur on ne fait les heuristiques qu'a une frequence pas trop
   grande. La profondeur du noeud racine est 0 */
if ( Bb->NoeudEnExamen->ProfondeurDuNoeud > 1 ) {
  /* NombreDeProblemesResolus est egal au nombre de noeuds resolus */
  if ( Bb->NombreDeProblemesResolus % Pne->pne_params->CYCLE_HEURISTIQUES != 0 ) return;
}

ProblemeTropGros = NON_PNE;  
if ( Pne->YaUneSolutionEntiere == OUI_PNE ) {	
  if ( Pne->StopHeuristiqueRINS == NON_PNE ) {
    if ( Pne->FaireHeuristiqueRINS == NON_PNE ) {
      Pne->NombreDeRefusSuccessifsHeuristiqueRINS++;
	    if ( Pne->NombreDeRefusSuccessifsHeuristiqueRINS > Pne->pne_params->NB_MAX_REFUS_SUCCESSIFS ) {
        Pne->FaireHeuristiqueRINS = OUI_PNE;  
		    Pne->NombreDeSolutionsHeuristiqueRINS = 0;
		    Pne->NombreDEchecsSuccessifsHeuristiqueRINS = 0;
        Pne->NombreDeRefusSuccessifsHeuristiqueRINS = 0;
	    }	
    }
    if ( Pne->FaireHeuristiqueRINS == OUI_PNE && 0 ) {
		  printf("PNE_HeuristiqueRINS:\n");
	    ProblemeTropGros = PNE_HeuristiqueRINS( Pne );
      if ( ProblemeTropGros == NON_PNE && 0 ) return;
		}
	}
}

if ( Pne->StopHeuristiqueFixation == NON_PNE && Pne->YaUneSolutionEntiere == NON_PNE ) {
  if ( Pne->FaireHeuristiqueFixation == NON_PNE ) {
    Pne->NombreDeRefusSuccessifsHeuristiqueFixation++;
	  if ( Pne->NombreDeRefusSuccessifsHeuristiqueFixation > Pne->pne_params->NB_MAX_REFUS_SUCCESSIFS ) {
      Pne->FaireHeuristiqueFixation = OUI_PNE;
	  	Pne->NombreDeSolutionsHeuristiqueFixation = 0;    
      Pne->NombreDeRefusSuccessifsHeuristiqueFixation = 0;
		  Pne->NombreDEchecsSuccessifsHeuristiqueFixation = 0;
	  }	
  }
  if ( Pne->FaireHeuristiqueFixation == OUI_PNE ) {	
		/*printf("PNE_HeuristiqueVariablesEntieresFixees:\n");*/
    ProblemeTropGros = PNE_HeuristiqueVariablesEntieresFixees( Pne );
    /*if ( ProblemeTropGros == NON_PNE ) return;*/     		
		if ( Pne->YaUneSolutionEntiere == OUI_PNE ) return;
  }
}

if ( Pne->StopHeuristiqueFractionalDive == NON_PNE ) {
  if ( Pne->FaireHeuristiqueFractionalDive == NON_PNE ) {
    Pne->NombreDeRefusSuccessifsHeuristiqueFractionalDive++;
	  if ( Pne->NombreDeRefusSuccessifsHeuristiqueFractionalDive > Pne->pne_params->NB_MAX_REFUS_SUCCESSIFS ) {
      Pne->FaireHeuristiqueFractionalDive = OUI_PNE;
	  	Pne->NombreDeSolutionsHeuristiqueFractionalDive = 0;    
      Pne->NombreDeRefusSuccessifsHeuristiqueFractionalDive = 0;
		  Pne->NombreDEchecsSuccessifsHeuristiqueFractionalDive = 0;
	  }
	}
  if ( Pne->FaireHeuristiqueFractionalDive == OUI_PNE && 0 ) {
		printf("PNE_HeuristiqueRechercheEnProfondeur:\n");	
    PNE_HeuristiqueRechercheEnProfondeur( Pne, PasDeSolutionEntiereDansSousArbre );
	}
}

return;
}

