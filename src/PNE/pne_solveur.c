// Copyright (c) 20xx-2019, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// This Source Code Form is subject to the terms of the Apache License, version 2.0.
// If a copy of the Apache License, version 2.0 was not distributed with this file, you can obtain one at http://www.apache.org/licenses/LICENSE-2.0.
// SPDX-License-Identifier: Apache-2.0
// This file is part of SIRIUS, a linear problem solver, used in the ANTARES Simulator : https://antares-simulator.org/.

/***********************************************************************

   FONCTION:  Solveur de PLNE 

   IMPORTANT: 
            * Inclure le header "pne_constantes_externes.h" dans le module 
              appelant. Il contient les valeurs des constantes symboliques 
              telles que OUI_PNE NON_PNE ENTIER REEL etc .. 
            * Ne jamais utiliser directement les valeurs numeriques des 
              constantes symboliques mais seulement leurs noms.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

/*----------------------------------------------------------------------------*/
/*
  Point d'entree du solveur de PNE:  
  -> Inclure le header "pne_constantes_externes.h" dans le module appelant. Il contient les valeurs 
     des constantes symboliques telles que OUI_PNE NON_PNE ENTIER REEL etc .. 
  -> Les variables sont numerot�e � partir de 0. Exemple: s'il y a 3 variables, 
     leurs numeros sont 0 1 et 2.
  -> En consequence s'il y a N inconnues, les colonnes de la matrice des contraintes 
     vont de 0 a N-1 inclus.
  -> Les contraintes sont numerot�e � partir de 0. Exemple: s'il y a 2 contraintes, 
     leurs numeros sont 0 et 1.
  -> En consequence s'il y a M contraintes, les lignes de la matrice des contraintes 
     vont de 0 a M-1 inclus.
*/
/*----------------------------------------------------------------------------*/

void PNE_Solveur( PROBLEME_A_RESOUDRE * Probleme , PNE_PARAMS * pneParams )
{
PROBLEME_PNE * Pne; void * Tas;

/* Controle anti piratage tres simplifie: on controle l'adresse MAC de la machine */
/*PNE_ControleMacAdresse( );*/

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  Probleme->ExistenceDUneSolution = ARRET_CAR_ERREUR_INTERNE;
  return; /* On n'utilise pas le malloc proprietaire */
  Tas = MEM_Init(); 
  Pne = (PROBLEME_PNE *) MEM_Malloc( Tas, sizeof( PROBLEME_PNE ) );
  if ( Pne == NULL ) {
    printf("Saturation memoire, impossible d'allouer l'objet PROBLEME_PNE\n");
    Probleme->ExistenceDUneSolution = ARRET_CAR_ERREUR_INTERNE;
    return;
  }				
	Pne->Tas = Tas;
# else 		
  Pne = (PROBLEME_PNE *) malloc( sizeof( PROBLEME_PNE ) );
  if ( Pne == NULL ) {
    printf("Saturation memoire, impossible d'allouer l'objet PROBLEME_PNE\n");
    Probleme->ExistenceDUneSolution = ARRET_CAR_ERREUR_INTERNE;
    return;
  }
	Tas = NULL;
	Pne->Tas = Tas;
	Pne->Controls = NULL;
# endif

	if (pneParams == NULL) {
		pneParams = newDefaultPneParams();
	}
	Pne->pne_params = pneParams;

Pne->AnomalieDetectee = NON_PNE ;
setjmp( Pne->Env );  
if ( Pne->AnomalieDetectee == OUI_PNE ) {
  /* Liberation du probleme */ 
  PNE_LibereProbleme( Pne );
  Probleme->ExistenceDUneSolution = ARRET_CAR_ERREUR_INTERNE;
  fflush(stdout);
  return;
} 
else {
 
  if ( Probleme->SortirLesDonneesDuProbleme == OUI_PNE ) {
    PNE_EcrireJeuDeDonneesMPS( Pne, Probleme );   
  }

  /* Optimisation */
  
  Pne->ProblemeSpxDuSolveur = NULL; 
  Pne->ProblemeBbDuSolveur  = NULL; 
  Pne->ProblemePrsDuSolveur = NULL;

  /* Initialisation du temps */
  time( &(Pne->HeureDeCalendrierDebut) );
  
  PNE_SolveurCalculs( Probleme , Pne ); 
}

/* Pour vider le buffer des traces */
fflush(stdout);

return;
}
