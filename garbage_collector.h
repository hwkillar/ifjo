/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace prekladace imperativniho jazyka IFJ17
 * Varianta:Tym 031, varianta I
 * Subor:   garbage_collector.h
 * Datum:   1.12.2017
 * Autori:  Kozouskova Aneta	<xkozou00@stud.fit.vutbr.cz>,
 *          Sencuch Filip	    <xsencu01@stud.fit.vutbr.cz>,
 *          Nguyen QuangTrang	<xnguye11@stud.fit.vutbr.cz>,
 *          Pribyl Tomas	    <xpriby17@stud.fit.vutbr.cz>
 */

#pragma once

typedef struct GarbageRecord
{
    void * data;
    int velikost;
    void * nextPtr;
} tGarbageRecord, *tGarbageRecordPtr;




void * advMalloc(int velikost);
void * advRealloc(void * destinationAdr, int velikost);
void  globalFree();

void advAddReallocMem(void * tmpVar, int velikost, void * toNULL);
void advFindAndNULL(void * toNULL);
