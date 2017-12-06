/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace prekladace imperativniho jazyka IFJ17
 * Varianta:Tym 031, varianta I
 * Soubor:  vestfunk.h
 * Datum:   14.11.2017
 * Autori:  Kozouskova Aneta	<xkozou00@stud.fit.vutbr.cz>,
 *          Sencuch Filip	    <xsencu01@stud.fit.vutbr.cz>,
 *          Nguyen QuangTrang	<xnguye11@stud.fit.vutbr.cz>,
 *          Pribyl Tomas	    <xpriby17@stud.fit.vutbr.cz>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include "symtable.h"
#pragma once

#define MAX 256

//define DEBUG 1


tBTSUzolPtr Substr(tBTSUzolPtr input, tBTSUzolPtr node_start, tBTSUzolPtr node_end);
tBTSUzolPtr Length(tBTSUzolPtr input);
tBTSUzolPtr Asc(tBTSUzolPtr input, tBTSUzolPtr index);
tBTSUzolPtr Chr(tBTSUzolPtr input);
//int Length(char *str);
//int Asc(char *str, int i);
//int Chr(int i);
//int substr(char* str, int i, int n);
