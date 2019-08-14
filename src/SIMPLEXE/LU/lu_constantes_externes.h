// Copyright (c) 20xx-2019, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// This Source Code Form is subject to the terms of the Apache License, version 2.0.
// If a copy of the Apache License, version 2.0 was not distributed with this file, you can obtain one at http://www.apache.org/licenses/LICENSE-2.0.
// SPDX-License-Identifier: Apache-2.0
// This file is part of SIRIUS, a linear problem solver, used in the ANTARES Simulator : https://antares-simulator.org/.

# ifndef CONSTANTES_EXTERNES_LU_DEJA_DEFINIES  
/*******************************************************************************************/
/* 
  Definition des constantes symboliques a utiliser par le module appelant la 
  factorisation lu 
*/ 
			 									        
# define OUI_LU      1  	             				     
# define NON_LU      0  	 	         
# define SATURATION_MEMOIRE     2        	      
# define MATRICE_SINGULIERE     3
# define PRECISION_DE_RESOLUTION_NON_ATTEINTE  4  /* Quand on fait du raffinement iteratif, si on atteint
                                                     pas la precision demandee */
  
/* Les contextes d'utilisation de la factorisation */
# define LU_SIMPLEXE         1
# define LU_POINT_INTERIEUR  2
# define LU_GENERAL          3

/*******************************************************************************************/
# define CONSTANTES_EXTERNES_LU_DEJA_DEFINIES	
# endif 

 


