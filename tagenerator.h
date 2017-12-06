/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace prekladace imperativniho jazyka IFJ17
 * Varianta:Tym 031, varianta I
 * Soubor:   tagenerator.h
 * Datum:   14.11.2017
 * Autori:  Kozouskova Aneta	<xkozou00@stud.fit.vutbr.cz>,
 *          Sencuch Filip	    <xsencu01@stud.fit.vutbr.cz>,
 *          Nguyen QuangTrang	<xnguye11@stud.fit.vutbr.cz>,
 *          Pribyl Tomas	    <xpriby17@stud.fit.vutbr.cz>
 */

#pragma once
#include "symtable.h"
#include "funkce.h"
#include "lexikalni_analyzator.h"



/*****
 * Struktura do ktere se ukladaji instrukce s informacemi o operatorech, vysledku, poradi operace a pripadnem skoku
 *****/
typedef struct
{
    size_t operation;
    void *op1;
    void *op2;
    void *result;
    int jump;
    int end;
} TA_Item;

/*****
 * konstatna po kolika se budou prialokovavat polozky do pole 3adresnych instrukci nebo pole jumpu
 *****/
#define ALL_MORE 10

/*****
 * vycet instrukci
 *****/

enum op
{
    /** numericke operace **/
    I_ADD,	//+
    I_SUB,		//-
    I_DIVC,		// /
    I_DIVD,     // \/
    I_MUL,      // *
    //I_POW,		// ^
    I_ASSIGN,	// =
    /** logicke operace **/
    I_EQUAL,		// ==
    I_NEQUAL,	// !=
    I_ELESS,		// <=
    I_LESS,		// <
    I_MORE,		// >
    I_EMORE,		// >=
    /** vestavene fce **/
    I_INPUT,
    I_PRINT,
    I_Asc,
    I_Chr,
    I_SUBS,
    I_Length,

    /** jumpy **/
    I_LABEL,		// jenom labeldsfds
    I_TJUMP,		//true jump
    I_FJUMP,		//false jump
    I_JUMP,		//nepodmineny jump
    I_GOTO,
    I_FCEJUMP,

};

/*****
 * Hlavicky funkci
 *****/

void ta_Init(TA_Item **ta_table);
void ta_Insert(TA_Item **ta_table, size_t operation, tBTSUzolPtr op1, tBTSUzolPtr op2, tBTSUzolPtr result);
void ta_InsertJump(TA_Item **ta_table, size_t operation, tBTSUzolPtr op1, int jump);
void ta_Get(TA_Item **ta_table, size_t *operation, tBTSUzolPtr *op1, tBTSUzolPtr *op2, tBTSUzolPtr *result, int position);
void ta_Destroy(TA_Item **ta_table);
