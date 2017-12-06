/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace prekladace imperativniho jazyka IFJ17
 * Varianta:Tym 031, varianta I
 * Soubor:   instrukce.h
  * Datum:   1.12.2017
 * Autori:  Kozouskova Aneta	<xkozou00@stud.fit.vutbr.cz>,
 *          Sencuch Filip	    <xsencu01@stud.fit.vutbr.cz>,
 *          Nguyen QuangTrang	<xnguye11@stud.fit.vutbr.cz>,
 *          Pribyl Tomas	    <xpriby17@stud.fit.vutbr.cz>
 */

#pragma once

#include <math.h>
#include "tagenerator.h"
#include "symtable.h"

void instrukce(TA_Item **ta_table, tBTSUzolPtr root);
void destroy_labels(int *jump_array);
void find_labels(TA_Item **ta_table, int **jump_array);
