/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace prekladace imperativniho jazyka IFJ17
 * Varianta:Tym 031, varianta I
 * Soubor:  main.c
 * Datum:   6.12.2017
 * Autori:  Kozouskova Aneta	<xkozou00@stud.fit.vutbr.cz>,
 *          Sencuch Filip	    <xsencu01@stud.fit.vutbr.cz>,
 *          Nguyen QuangTrang	<xnguye11@stud.fit.vutbr.cz>,
 *          Pribyl Tomas	    <xpriby17@stud.fit.vutbr.cz>
 */

/**
 * Hlavickove soubory
 */

#include "funkce.h"
#include "garbage_collector.h"
#include "symtable.h"
#include "syntakticky_analyzator.h"
#include "clean.h"
#include <signal.h>

tFile soubor;




/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace prekladace imperativniho jazyka IFJ17
 * Varianta:Tym 031, varianta I
 * Soubor:  main.c
 * Datum:   6.12.2017
 * Autori:  Kozouskova Aneta	<xkozou00@stud.fit.vutbr.cz>,
 *          Sencuch Filip	    <xsencu01@stud.fit.vutbr.cz>,
 *          Nguyen QuangTrang	<xnguye11@stud.fit.vutbr.cz>,
 *          Pribyl Tomas	    <xpriby17@stud.fit.vutbr.cz>
 */

/**
 * Hlavickove soubory
 */

#include "funkce.h"
#include "garbage_collector.h"
#include "symtable.h"
#include "syntakticky_analyzator.h"
#include "clean.h"
#include <signal.h>

tFile soubor;




/**
 * Inicializujeme prekladac
 */
static void initMain(int argc, char **argv)
{
    // overime argumenty
    if (argc != 2)
    {
        error = ETYP;
        fprintf(stderr, "Byl zadan spatny pocet parametru!\n");
        return;
    }

    // pokusime se otevrit soubor
    if (!(soubor = fopen(argv[1], "r")))
    {
        error = ETYP;
        fprintf(stderr, "Nepodarilo se otevrit soubor!\n");
        return;
    }

    // inicializujeme tabulku symbolov
    TSinit();


}

/**
 * Zacatek programu
 */
int main(int argc, char **argv)
{
    (void) signal(SIGINT,clean_all);
    initMain(argc, argv);
    if (error)
    {
        perr();
        return error;
    }

    ////////////////////////////////////////
    ////      SYNTAKTICKA ANALYZA       ////
    ////////////////////////////////////////
    error = parser();
    if (error == ELEX)
    {
        perrdet();
        return error;
    }
    else if (error)
    {
        perr();
        return error;
    }

    // zrusime tabulku symbolov
    TSdispose();

    //zrusime tabulku 3add kodu


    // zavreme soubor
    fclose(soubor);
    globalFree();

    return EOK;
}
