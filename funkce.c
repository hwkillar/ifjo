/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace prekladace imperativniho jazyka IFJ17
 * Varianta:Tym 031, varianta I
 * Soubor:   funkce.c
 * Autori:  Kozouskova Aneta	<xkozou00@stud.fit.vutbr.cz>,
 *          Sencuch Filip	    <xsencu01@stud.fit.vutbr.cz>,
 *          Nguyen QuangTrang	<xnguye11@stud.fit.vutbr.cz>,
 *          Pribyl Tomas	    <xpriby17@stud.fit.vutbr.cz>
 */

#include "funkce.h"
#include "lexikalni_analyzator.h"
#include "garbage_collector.h"


/* globalni promenna */
tStav stav = EOK;
int radek = 1;
int sloupec = 1;

/**
 * Nazvy jednotlivych chyb
 */
const char *tError_mena[] =
{
    "",
    "Chyba v ramci lexikalni analyzy\0",
    "Chyba v ramci syntakticke analyzy\0",
    "Chyba v ramci semanticke analyzy\0",
    "Chyba ostatní sémantické chyby\0",
    "sémantická chyba typové\0",
    "Interni chyba prekladace\0"
};

/**
 * Vypise chybu
 */
void perr(void)
{
    fprintf(stderr, "%s radek:%d sloupec %d\n", tError_mena[error], token.radek,token.sloupec);
}

void perrdet()
{
    fprintf(stderr, "%s na radku %3d sloupci %3d\n", tError_mena[error], radek, sloupec);
}
