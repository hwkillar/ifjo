/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace prekladace imperativniho jazyka IFJ17
 * Varianta:Tym 031, varianta I
 * Subor:   ial.h
 * Datum:   14.11.2017
 * Autori:  Kozouskova Aneta	<xkozou00@stud.fit.vutbr.cz>,
 *          Sencuch Filip	    <xsencu01@stud.fit.vutbr.cz>,
 *          Nguyen QuangTrang	<xnguye11@stud.fit.vutbr.cz>,
 *          Pribyl Tomas	    <xpriby17@stud.fit.vutbr.cz>
 */

#pragma once
#include <stdbool.h>

/**
 * Datove struktury - Binarni vyhladavaci strom
 */
typedef char *tNazov;   // klic pro binarni vyhladavani

typedef enum            // enumerace pro interni typy symbolu
{
    tNil,               // null
    tBool,
    tDouble,
    tString,
    tInt,
} tTypSymbolu;

typedef union               // unie pro data symbolu
{
    bool b;
    double d;
    char *s;
    int i;
} tObsah;

typedef struct   // symbol
{
    tNazov nazev;       // nazev symbolu
    tTypSymbolu typ;    // datovy typ symbolu
    bool varFc;         // true je funkce a false je var
    int argCounter;
    void * nextNode; //prechod na dalsi argument funkce
    tObsah value;
    int jump;
} tSymbol, *tSymbolPtr;

typedef struct tBTSUzol
{
    tNazov klic;
    tSymbol data;
    struct tBTSUzol *lptr;
    struct tBTSUzol *rptr;
} *tBTSUzolPtr;	        // uzel binarniho stromu

/**
 * Globalne promene
 */
extern tBTSUzolPtr ts;  // tabulka symbolu

/**
 * Prototypy funkci
 */
void TSinit(void);                              // inicializuje tabulku symbolu
void TSdispose(void);                           // zrusi tabulku symbolu
void TSinitSymbol(tSymbol *symbol);             // inicializuje symbol pri deklaraci
tSymbol *TSvlozSymbol(tSymbol data);              // vlozi symbol do tabulky
tSymbol *TSvlozBool(tNazov nazev, bool data);
tSymbol *TSvlozDouble(tNazov nazev, double data);
tSymbol *TSvlozInt(tNazov nazev, int data);
tSymbol *TSvlozString(tNazov nazev, char *string);
tBTSUzolPtr TSreadSymbol(tNazov nazev);              // precte symbol z tabulky
void ialSort(char *string);                     // funkce potrebna poe nas
int ialSearch(char *string, char *pattern);     // nase funkce

tBTSUzolPtr BTSsearch(tBTSUzolPtr koren, tNazov klic);


void quickSort(char* stringToSort, int left, int right);
tBTSUzolPtr prep_quickSort(tBTSUzolPtr input);
void calcCharJumps(int charJump[], char* stringToFind, int len_stringToFind);
void calcMatchJumps(int matchJump[], char *patCont, int m);
tBTSUzolPtr find(tBTSUzolPtr input, tBTSUzolPtr inputToFind);
