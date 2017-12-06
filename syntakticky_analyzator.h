/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace prekladace imperativniho jazyka IFJ17
 * Varianta:Tym 031, varianta I
 * Soubor:   syntakticky_analyzator.h
 * Datum:   14.11.2017
 * Autori:  Kozouskova Aneta	<xkozou00@stud.fit.vutbr.cz>,
 *          Sencuch Filip	    <xsencu01@stud.fit.vutbr.cz>,
 *          Nguyen QuangTrang	<xnguye11@stud.fit.vutbr.cz>,
 *          Pribyl Tomas	    <xpriby17@stud.fit.vutbr.cz>
 */

#pragma once

#include "funkce.h"
#include "lexikalni_analyzator.h"
#include "symtable.h"
#include "vyrazy.h"
#include "tagenerator.h"

/**
 * Globalni promenna
 */
extern tSymbol name;
extern bool isPrint;
extern tBTSUzolPtr typPrirazeni;
extern tBTSUzolPtr volana_fce;
extern char * functionName;
extern TA_Item *ta;
extern int nazev_fce;
extern int *pole_back;
extern int index_pb;
extern bool isLocal;

extern tBTSUzolPtr cilovaAdresaFce;

/* 1=read;
 * 2=sort;
 *
 *
 * */
tError parser();
tError program();
tError funkce();
tError body();
tError params();
tError paramsNext();
tError varDeclar();
tError prirazeni();
tError list();
tError vest();
tError input();
tError print();
tError litExpr();
tError literal();
tError args();
tError argsNext();

/**
 * Funkce pro vyhledavani promennych ve stome
 */

void najdiKolize(tBTSUzolPtr uzel);
