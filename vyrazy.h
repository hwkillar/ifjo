/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace prekladace imperativniho jazyka IFJ17
 * Varianta:Tym 031, varianta I
 * Soubor: vyrazy.h
 * Datum:   14.11.2017
 * Autori:  Kozouskova Aneta	<xkozou00@stud.fit.vutbr.cz>,
 *          Sencuch Filip	    <xsencu01@stud.fit.vutbr.cz>,
 *          Nguyen QuangTrang	<xnguye11@stud.fit.vutbr.cz>,
 *          Pribyl Tomas	    <xpriby17@stud.fit.vutbr.cz>
 */

#pragma once

/**
 * Hlavickove soubory
 */
#include "symtable.h"
#include "lexikalni_analyzator.h"
#include "funkce.h"
#include "syntakticky_analyzator.h"
#include "tagenerator.h"
#include <stdlib.h>


//globalni promenna pro generovani unikatnich nazvu konstant
extern unsigned int nameID;
extern bool isExprWr;

/**
 * enumy pro vyplneni staticke precedencni tabulky
 */

typedef enum
{
    M,	//< mensi
    V,	//> vetsi
    R,	//= rovno
    E	//prazdne misto v tabulce -> synt. chyba
} tPrvkyindexu;

typedef enum
{
    ADD,		//0 +
    SUB,	//1 -
    TIMES,		//2 *
    DELENO_D,	//3 /
    DELENO_C,   //4 \ A
    MOCNINA,	//5 ^
    ROVNITKO,	//6 ==
    NEROVNITKO,	//7 <>
    MENSIROVNO,	//8 <=
    VETSIROVNO,	//9 >=
    VICE,		//10 >
    MENE,		//11 <
    LZAVORKA,	//12 (
    PZAVORKA,	//13 )
    ID,		//14 promenna a konstanta
    FCE,		//15 volani funkce
    CAR,	//15 ,
    KDOLAR,	//17 $ konec vyrazu
    MENSITKO,	//18 oznaceni < na zasobniku
    NETERM,	//19 neterminalni znak napr. E->
    CHYB,		//20 jiny token do vyrazu nepripoustime
    KONKAT,	//21 ..
} tIndexy;

/**
 * zasobnik pro vyrazy
 */
//datova slozka na zasobniku se sklada z vicero prvku
typedef struct
{
    tSymbol data;		//polozka tabulky symbolu
    tIndexy p;	//pseudopolozky s kterymi budu operovat na zasobniku
} tData;

extern tData neterm; //globalni pro ukladani vysledku exp

typedef struct tZasPrvek
{
    struct tZasPrvek *ptr;
    tData item;
} *tZPrvekptr;

typedef struct
{
    tZPrvekptr vrchol;
} tVZasobnik;

/**
 * Prototypy funkci
 */
//pparser
tError pparser();	//main precedencni analyzy
// zasobnik
void VSinit(tVZasobnik *zasobnik);	//inicializace zasobniku
void VSpush(tVZasobnik *zasobnik, tData item);	//push
void VSpop(tVZasobnik *zasobnik);				//pop
void VStop(tVZasobnik *zasobnik, tData *item);
void VStoppop(tVZasobnik *zasobnik, tData *item);
bool VSempty(tVZasobnik *zasobnik);
