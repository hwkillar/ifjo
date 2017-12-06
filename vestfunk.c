/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace prekladace imperativniho jazyka IFJ17
 * Varianta:Tym 031, varianta I
 * Soubor:   vestfunk.c
 * Datum:   14.11.2017
 * Autori:  Kozouskova Aneta	<xkozou00@stud.fit.vutbr.cz>,
 *          Sencuch Filip	    <xsencu01@stud.fit.vutbr.cz>,
 *          Nguyen QuangTrang	<xnguye11@stud.fit.vutbr.cz>,
 *          Pribyl Tomas	    <xpriby17@stud.fit.vutbr.cz>
 */

#include "symtable.h"
#include "vestfunk.h"
#include "vyrazy.h"
#include "garbage_collector.h"

/*int Length (char *str)// vlastni funkce bez pomoci c knihovna
{
    int len = 0;
    while (str[len] != '\0')
    {
        len++;
    }
    return len;
}
int Length(char *str)
{
    return strlen(str);
}

/*int Asc(char *str, int i)
{
    int number = 0;
    number = str[i] - 'a' + 97;
    return number;
}

char Chr(int i)
{
    char z;
    z = i + 'a' - 97;
    return z;
}*/
/*char *substr (char *str, int i, int n)
{
    if (i < 0 || n < 0)
    {
        fprintf(stderr, "substr: index or length is negative value. \n");
        return 4;// other error
    }
    char *newstr = (char*) malloc(sizeof(char));
    if (newstr == NULL)
        return 99;// chyba alokace

    if (i > (Length(str)))
    {
        fprintf(stderr, "index is greater than length of string.\n");
        free(newstr);
        return 4;// chyba doplnit typ chyby
    }

    if (n > (Length(str)))
    {
        fprintf(stderr, "length of searching string is longer than length of string.\n");
        free(newstr);
        return 4;// chyba doplnit typ chyby
    }

   // memcpy (newstr, str[i] , Length(str));// pomoci c funkce
    int d = 0;
    while ( i < n) // bez pomoci c funkce
    {
        newstr[d] = str[i];
        i++;
        d++;
    }
    newstr[n] = '\0';
    return newstr;
}
*/
tBTSUzolPtr Substr(tBTSUzolPtr input, tBTSUzolPtr node_start, tBTSUzolPtr node_end)
{
    if(error != EOK)
        return NULL;
    int start = 0;
    int end = 0;
    int j = 0;
    char *strPtr;

    // overit vstupni datove typy (char*, int, int)
    if(input == NULL || node_start == NULL || node_end == NULL) {
        strPtr[0] = '!';
    }
    if(input->data.typ != tString)   // jestli se jedna opravdu o string, jinak chyba
    {
        strPtr[0] = '!';
        error = ETYP;
    }
    if((node_start->data.typ != tInt) && (node_end->data.typ != tInt))   // jestli se jedna opravdu o cisla, jinak chyba
    {
        strPtr[0] = '!';
        error = ETYP;
    }
    // dale jiz pracujeme jen s retezcem
    char* inString = input->data.value.s;
    if (start < 0 || end < 0)
    {
        fprintf(stderr, "substr: index or length is negative value. \n");
        strPtr[0] = '!';
    }
    else if ( start > (strlen(inString)))
    {
        fprintf(stderr, "index is greater than length of string.\n");
        strPtr[0] = '!';
    }
    else if ( end > (strlen(inString)))
    {

       fprintf(stderr, "length of searching string is longer than length of string.\n");
       strPtr[0] = '!';
    }
    else if (start > end)
    {

       fprintf(stderr, "index is greater than length.\n");
       strPtr[0] = '!';

    }
    else
    {
        strPtr = advMalloc(sizeof(char)*(end-start)+1);
        for (int i = start; i < end; i++)
        {
            strPtr[j] = inString[i];
            j++;
        }
        strPtr[j+1] = '\0';
    }
    char* str = advMalloc(sizeof(char)*25);
    tSymbol ptr;
    TSinitSymbol(&ptr);
    ptr.nazev = str;
    ptr.varFc = false;
    char* ret = advMalloc(sizeof(char)*strlen(strPtr)+1);
    strcpy(ret, strPtr);
    ptr.value.s = ret;
    ptr.typ = tString;

    if((TSreadSymbol(str)) != NULL) //pokud uz jmeno bylo v tabulce
        error = ETYP;
    else TSvlozSymbol(ptr); //jinak vloz do tabulky

    tBTSUzolPtr node = TSreadSymbol(str);
    return node;
}
tBTSUzolPtr Asc(tBTSUzolPtr input, tBTSUzolPtr index)
{
    char *instring = input->data.value.s;
    int i = index->data.value.i;
    int number = 0;

     if(error != EOK)
        return NULL;
    if((input == NULL) || (index == NULL)) {
        number = 0;
    }
    else if(input->data.typ != tString)   // jestli se jedna opravdu o string, jinak chyba
    {
        error = ETYP;
        number = 0;
    }
    else if(index->data.typ != tInt)   // jestli se jedna opravdu o int, jinak chyba
    {
        error = ETYP;
        number = 0;
    }
    else if ( i > strlen(instring))
    {
        number = 0;
        fprintf(stderr, "index is greater than length of string.\n");
    }
    else
    {
       number = instring[i] - 'a' + 97;
    }

    char* str = advMalloc(sizeof(char)*25);
    tSymbol ptr;
    TSinitSymbol(&ptr);
    ptr.nazev = str;
    ptr.varFc = false;
    ptr.value.i = number;
    ptr.typ = tInt;

    if((TSreadSymbol(str)) != NULL) //pokud uz jmeno bylo v tabulce
        error = ETYP;
    else TSvlozSymbol(ptr); //jinak vloz do tabulky

    tBTSUzolPtr node = TSreadSymbol(str);
    return node;
}
tBTSUzolPtr Chr(tBTSUzolPtr input)
{
    if(input->data.typ != tInt)   // jestli se jedna opravdu o int, jinak chyba
    {
        error = ETYP;
    }
    int number = input->data.value.i;
    char asci;
    if (number < 0 && number > 255)
    {
        return; // ? nedefinovany stav
    }
    asci = number + 'a' - 97;

    char* str = advMalloc(sizeof(char)*25);
    tSymbol ptr;
    TSinitSymbol(&ptr);
    ptr.nazev = str;
    ptr.varFc = false;
    ptr.value.i = asci;
    ptr.typ = tString;

    if((TSreadSymbol(str)) != NULL) //pokud uz jmeno bylo v tabulce
        error = ETYP;
    else TSvlozSymbol(ptr); //jinak vloz do tabulky

    tBTSUzolPtr node = TSreadSymbol(str);
    return node;

}
tBTSUzolPtr Length(tBTSUzolPtr input)
{
    if(input->data.typ != tString)   // jestli se jedna opravdu o string, jinak chyba
    {
        error = ETYP;
    }
    char *instring = input->data.value.s;
    int len = strlen(instring);
    char* str = advMalloc(sizeof(char)*25);
    tSymbol ptr;
    TSinitSymbol(&ptr);
    ptr.nazev = str;
    ptr.varFc = false;
    ptr.value.i = len;
    ptr.typ = tInt;

    if((TSreadSymbol(str)) != NULL) //pokud uz jmeno bylo v tabulce
        error = ETYP;
    else TSvlozSymbol(ptr); //jinak vloz do tabulky

    tBTSUzolPtr node = TSreadSymbol(str);
    return node;

}
