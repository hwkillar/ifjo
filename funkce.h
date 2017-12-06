/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace prekladace imperativniho jazyka IFJ17
 * Varianta:Tym 031, varianta I
 * Soubor:   funkce.h
 * Autori:  Kozouskova Aneta	<xkozou00@stud.fit.vutbr.cz>,
 *          Sencuch Filip	    <xsencu01@stud.fit.vutbr.cz>,
 *          Nguyen QuangTrang	<xnguye11@stud.fit.vutbr.cz>,
 *          Pribyl Tomas	    <xpriby17@stud.fit.vutbr.cz>
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/**
 * Typ pre ukazatel na soubor
 */
typedef FILE *tFile;

/**
 * Chybove kody.
 */
typedef enum Error
{
    EOK,        // 0 - vsechno je ok
    ELEX,       // 1 - chyby v ramci lexikalnej analyzy
    ESYN,       // 2 - chyby v ramci syntaktickej analyzy
    ESEM,       // 3 - chyby v ramci semantickej analyzy
    ETYP,       // 4 - sémantická chyba typové kompatibility
    EOST,       // 6 - ostatní sémantické chyby.
    EAL,        // 99
} tError;

/**
 * Globalne premenne
 */
tFile soubor;
tError error;
int radek;
int sloupec;


void perr(void);
void perrdet(void);
