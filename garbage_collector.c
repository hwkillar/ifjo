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

#include <stdlib.h>
#include <stdio.h>

#include "garbage_collector.h"
#include "funkce.h"

tGarbageRecordPtr garbageHead = NULL;
tGarbageRecordPtr garbageLast = NULL;

void * advMalloc(int velikost)
{
    tGarbageRecordPtr tmpNode = malloc(sizeof(struct GarbageRecord));
    if(tmpNode == NULL) {
        error = EAL;
        return NULL;
    }
    tmpNode->data = NULL;
    tmpNode->nextPtr = NULL;
    tmpNode->velikost = velikost;

    if(garbageHead == NULL)
        garbageHead = tmpNode;


    if(garbageLast != NULL)
    {
        garbageLast->nextPtr = tmpNode; //navazu predposledni
        garbageLast = tmpNode; //posunu posledni
    }
    else garbageLast = tmpNode;

    void * tmpAllocated = (void *)malloc(velikost);
    if(tmpAllocated == NULL) {
        error = EAL;
        return NULL;
    }
    tmpNode->data = tmpAllocated; //vlozim ukazatel do stromu

    return tmpAllocated;
}

void globalFree()
{
    tGarbageRecordPtr tmpVar = NULL;
    tmpVar = garbageHead;
    while(tmpVar != NULL) //uvolnim data programu
    {
        if(tmpVar->data != NULL)
            free(tmpVar->data);
        tmpVar = tmpVar->nextPtr;
    }
    tmpVar = garbageHead;
    while(tmpVar != NULL)//uvolnim samotny list
    {
        garbageHead = tmpVar;
        tmpVar = tmpVar->nextPtr;
        free(garbageHead);
    }
}

void * advRealloc(void * destinationAdr, int velikost)
{
    void * tmpVar = NULL;
    if(destinationAdr == NULL)
    {
        destinationAdr = advMalloc(velikost);
        return destinationAdr;
    }
    else
    {
        tmpVar = realloc(destinationAdr, velikost);
        if(tmpVar != destinationAdr)
            advAddReallocMem(tmpVar, velikost, destinationAdr);
        return tmpVar;
    }
}

void advAddReallocMem(void * tmpVar, int velikost, void * toNULL)
{
    advFindAndNULL(toNULL); //nepouzivana adresa se da na NULL

    tGarbageRecordPtr tmpNode = malloc(sizeof(struct GarbageRecord));
    if(tmpNode == NULL) {
        error = EAL;
        return ;
    }

    tmpNode->data = tmpVar;
    tmpNode->nextPtr = NULL;
    tmpNode->velikost = velikost;

    if(garbageHead == NULL)
        garbageHead = tmpNode;


    if(garbageLast != NULL)
    {
        garbageLast->nextPtr = tmpNode; //navazu predposledni
        garbageLast = tmpNode; //posunu posledni
    }
    else garbageLast = tmpNode;

    tmpNode->data = tmpVar; //vlozim ukazatel do stromu
}

void advFindAndNULL(void * toNULL)
{
    tGarbageRecordPtr tmp = garbageHead; //skocim na hlavicku
    if(tmp == NULL)
        return ;

    while(tmp != NULL)
    {
        if(tmp->data == toNULL)
            tmp->data = NULL;
        tmp = tmp->nextPtr;
    }
}
