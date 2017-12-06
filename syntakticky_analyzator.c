/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace prekladace imperativniho jazyka IFJ17
 * Varianta:Tym 031, varianta I
 * Soubor:   syntakticky_analyzator.c
 * Datum:   14.11.2017
 * Autori:  Kozouskova Aneta	<xkozou00@stud.fit.vutbr.cz>,
 *          Sencuch Filip	    <xsencu01@stud.fit.vutbr.cz>,
 *          Nguyen QuangTrang	<xnguye11@stud.fit.vutbr.cz>,
 *          Pribyl Tomas	    <xpriby17@stud.fit.vutbr.cz>
 */

#include <ctype.h>
#include <math.h>
#include "funkce.h"
#include "garbage_collector.h"
#include "syntakticky_analyzator.h"
#include "lexikalni_analyzator.h"
#include "vyrazy.h"
#include "tagenerator.h"
#include "instrukce.h"


#define ASC_PARAMS 1
#define CHR_PARAMS 2
#define LENGTH_PARAMS 1
#define SUBSTR_PARAMS 3

tSymbol name;
tData neterm;
tData fce;
TA_Item *ta;
tBTSUzolPtr ts;
bool isExprWr = false;
bool isPrint = false;
bool isChr = false;
bool isAsc = false;
bool isLength = false;
bool isSubs = false;
bool isDim = false;
bool isLocal = false;
bool isWriteFunc = false;
tBTSUzolPtr typPrirazeni = NULL;
int nazev_fce;
tBTSUzolPtr volana_fce = NULL;
int label = 1;
tBTSUzolPtr cilovaAdresa = NULL;
tBTSUzolPtr cilovaAdresaFce = NULL;
tBTSUzolPtr for_return = NULL;
tBTSUzolPtr pole_argumentu[4] = {NULL, NULL, NULL, NULL};
int *cislo_argumentu = NULL;
int *pole_back = NULL;
int index_pb = 0;



tBTSUzolPtr functionNode; //ukazatel na funkci, ktere chci dat argumenty
tBTSUzolPtr functionNodeTmp;
tBTSUzolPtr pom;
char *functionName = NULL;

tError parser()
{
    ta_Init(&ta);// init ta
    cislo_argumentu = advMalloc(sizeof(int));
    *(cislo_argumentu) = 1;
    pole_back = (int*)advMalloc(sizeof(int)*ALL_MORE);
    error = EOK;
    tToken token = getToken();
    if(token.stav == ENDOFFILE) //pokud je soubor prazdny, chyba
        error = ESYN;
    else error = program(); //pokracuj v analyze

    if(error != EOK)
        return error;

    instrukce(&ta, ts);

    ta_Destroy(&ta);
    return error;
}

tError program()
{
    if(error != EOK) //pokud je vse v poradku, tak pokracuj
        return error;
    //isWrite = false; //nevolame zatim funkci write

    ta_InsertJump(&ta,I_JUMP,NULL,0);// vloz label do ta
    //ta->end = 0;

    if(token.stav != KLIC_SLOVO) //pokud nedostanu klicove slovo
        return  ESYN;

    error = funkce();
    if(error != EOK)
        return error;

    getToken();

    if(token.stav == KLIC_SLOVO)
    {
        if(!(strcmp(token.data, "scope"))) //v pripade ze scope nebyl posledni -> chyba
            error = ESEM;
    }

    if(token.stav != ENDOFFILE) //kod musi konci EOF
        error = ESYN;


    return error;
}

/**
 * <func>	->	EPS
 * <func>	->	EPS "declare" "function" ID "(" <params> ")" "as" <navratovy_typ> simple function
 * <func>		->	"function" ID "(" <params> ")" "as" <navratovy_typ><vardec> <list> eol  <func>
 */

tError funkce()
{
    if(error != EOK)
        return error;

    if(token.stav != KLIC_SLOVO) //pokud nedostanu klicove slovo
        return  ESYN;
    if (!(strcmp(token.data, "declare"))) // simple function
    {
        getToken();
        if ((strcmp(token.data, "function"))) // kontrola "declare" "function" ID "(" <params> ")" "as" <navratovy_typ> simple function
            return ESYN;

        tSymbol dataStromu; //vlozim do stromu jmeno funkce
        TSinitSymbol(&dataStromu);
        dataStromu.varFc = true;
        char *nazev = token.data; //vlozim data
        dataStromu.nazev = nazev;
        dataStromu.jump = label;

        if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            return ESEM;
        else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

        functionNode = TSreadSymbol(nazev);
        functionName = functionNode->data.nazev; //udelam si odkaz na jmeno soucasne funkce
        functionNodeTmp = functionNode;
        for_return = functionNode;

        if(error != EOK)
            return error;

        najdiKolize(ts);
        if(error != EOK)
        {
            fprintf(stderr, "Identifikator: \"%s\" byl jiz deklarovan\n", token.data);
            return error;
        }

        getToken();// "(" musi nasledovat leva zavorka
        if(token.stav != LEVA_ZAVORKA)
        {
            return ESYN;
        }
        else {
            error = params(); //<params>
        }
            if(error != EOK)
        {
            return error;
        }

        if(token.stav != PRAVA_ZAVORKA) // ")"
            {
                return ESYN;
            }

        ta_InsertJump(&ta,I_LABEL,NULL,label);
        label++;

        getToken();
        if(error != EOK)
            {
                return error;
            }

        if(token.stav == KLIC_SLOVO) // musi nasledovat as coz je klicova slova
        {
            if((strcmp(token.data, "as"))) //"end"
                return ESYN;
            getToken();
            if(error != EOK)
            {
                return error;
            }
            if(token.stav != KLIC_SLOVO)
                return ESYN;
            if (((strcmp(token.data, "integer")) && ((strcmp(token.data, "double"))) && ((strcmp(token.data, "string")))))// test navratove hodnoty
                return ESYN;
             else {
                ta_Insert(&ta,I_GOTO,NULL,NULL,NULL);
                error = funkce();
             }
        }
        else
        {
            return ESYN;
        }
    return error;

    }
    if((strcmp(token.data, "function"))) //"function"
        return ESYN;

    getToken();
    if(token.stav != IDENTIFIK) //pokud nedostanu identifikator
        return ESYN;

    if(!(strcmp(token.data, "scope"))) //pokud mame "scope"
    {
        tSymbol dataStromu; //vlozim do stromu jmeno funkce
        TSinitSymbol(&dataStromu);
        dataStromu.varFc = true;
        char *nazev = token.data; //vlozim data
        dataStromu.nazev = nazev;
        dataStromu.jump = label;
        label++;
        if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            return ESEM;
        else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

        functionNode = TSreadSymbol(nazev);
        functionName = functionNode->data.nazev; //udelam si odkaz na jmeno soucasne funkce

        if(error != EOK)
            return error;

        najdiKolize(ts);
        if(error != EOK)
        {
            fprintf(stderr, "Identifikator: \"%s\" byl jiz deklarovan\n", token.data);
            return error;
        }


        getToken();
        if(token.stav != LEVA_ZAVORKA) // "("
            return ESYN;
        getToken();
        if(token.stav != PRAVA_ZAVORKA) // ")"
            return ESYN;

        ta_InsertJump(&ta,I_LABEL,NULL,0);

        getToken();
        if(error != EOK)
            return error;

        if((strcmp(token.data, "end"))) //pokud funkce neni prazdna
            error = body(); //<body>


        if(error != EOK)
            return error;

        if(token.stav == KLIC_SLOVO) //klicove slovo
        {
            if((strcmp(token.data, "end"))) //"end"
                return ESYN;

            getToken();
            if(error != EOK)
                return error;
            if ((strcmp(token.data, "scope")))
                return ESYN;
            /*getToken();
            if(error != EOK)
                return error;
            if(token.stav != EOL) //"eol"
                return ESYN;*/
        }
        else return ESYN;

        return error;
    }
    else //jinak jina funkce
    {
        tSymbol dataStromu; //vlozim do stromu jmeno funkce
        TSinitSymbol(&dataStromu);
        dataStromu.varFc = true;
        char *nazev = token.data; //vlozim data
        dataStromu.nazev = nazev;
        dataStromu.jump = label;

        if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            return ESEM;
        else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

        functionNode = TSreadSymbol(nazev);
        functionName = functionNode->data.nazev; //udelam si odkaz na jmeno soucasne funkce
        functionNodeTmp = functionNode;
        for_return = functionNode;

        if(error != EOK)
            return error;

        najdiKolize(ts);
        if(error != EOK)
        {
            fprintf(stderr, "Identifikator: \"%s\" byl jiz deklarovan\n", token.data);
            return error;
        }

        getToken();// "("
        if(token.stav != LEVA_ZAVORKA)
        {
            return ESYN;
        }
        else {
            error = params(); //<params>
        }
            if(error != EOK)
        {
            return error;
        }

        if(token.stav != PRAVA_ZAVORKA) // ")"
            {
                return ESYN;
            }

        ta_InsertJump(&ta,I_LABEL,NULL,label);
        label++;

        getToken();
        if(error != EOK)
            {
                return error;
            }
        if (token.stav != KLIC_SLOVO)
            return ESYN;
        else
        {
            if((strcmp(token.data, "as")))
            {
                return ESYN;
            }
        }
        getToken();
        if(error != EOK)
            {
                return error;
            }
        if (token.stav != KLIC_SLOVO)
        return ESYN;
        else
        {
           if (((strcmp(token.data, "integer"))) && ((strcmp(token.data, "double"))) && ((strcmp(token.data, "string"))))// test navratove hodnoty
                return ESYN;
        }
        getToken();
        if(error != EOK)
            {
                return error;
            }
        if (token.stav != EOL)
        {
            return ESYN;
        }
        getToken();
        if(error != EOK)
            {
                return error;
            }
        if((strcmp(token.data, "end"))) //pokud funkce neni prazdna
        {
            error = body(); //<body>
        }

        if(error != EOK)
            {
                return error;
            }
        if (token.stav != KLIC_SLOVO) return ESYN;
        if(token.stav == KLIC_SLOVO) //klicove slovo
        {
            if((strcmp(token.data, "end"))) //"end"
                {
                    return ESYN;
                }
        }
        ta_Insert(&ta,I_GOTO,NULL,NULL,NULL);

        getToken();
        if(error != EOK)
            {
                return error;
            }

        if(token.stav == KLIC_SLOVO) //pokud nedostanu klicove slovo
            if(!(strcmp(token.data, "function"))) //"scope"
                error = funkce(); //jdu zpracovat dalsi funkci
    }
    return error;
}

/**
 *  overim jestli jsou ve funkci deklarovane promenne a prikazy
 */
tError body()
{
    if(error != EOK)
        return error;

    error = varDeclar(); //<varDeclar>
    if(error != EOK)
        return error;

    if(!(strcmp(token.data, "end"))) //pokud dostanu "END"
        return error;

    error = list(); //<list>
    if(error != EOK)
        return error;

    return error;
}


/**
 * <list>        ->  EPS
 * <list>	->  ID <prirazeni> "eol"
 * <list>	->  "print" <print>, <list>
 * <list>	->  "if" <expr> "then" "eol" <list> "else" "eol" <list> "end" "if" <list>
 * <list>	->  "dowhile" <expr> "eol" <list> "loop" "eol" <list>
 * <list>	->  "return" <expr> "eol"
 */

tError list()
{
    if(error != EOK) // kontrola vnoreneho if
        return error;

    if(!(strcmp(token.data, "end")))//list je na konci
      {
          getToken();
          if ((strcmp(token.data, "if")))// musi koncit end if
          {
              return error;
          }
          else return ESYN;
      }

    if(!(strcmp(token.data, "else")))//list je na konci "then" u "IF"
        {
          getToken();
          if (token.stav != EOL) // musi nasledovat eol
          {
              return error;
          }
          else return ESYN;
      }

    tSymbol dataStromu; //vlozim do stromu jmeno funkce
    TSinitSymbol(&dataStromu);
    char *nazev;
    tBTSUzolPtr tmpNode = NULL;

    switch (token.stav) // zacatek kontroly , muze byt bud if , dowhile ,print, id = , a musi koncit return
    {
    case IDENTIFIK: //<list>	->  ID <prirazeni> "eol" <list> je-li je to indentifikator

        tmpNode = TSreadSymbol(token.data); //pokud najdes, tak to je funkce
        if((tmpNode != NULL) && (tmpNode->data.varFc == true)) // je-li id je funkce take testujeme jmenofunkce ()
        {
            volana_fce = tmpNode;
            getToken();
            if(error != EOK)
                return error;

            if(token.stav != LEVA_ZAVORKA) //musim mit "("
                return ESYN;

            error = args();
            if(error != EOK)
                return error;

            if(token.stav != PRAVA_ZAVORKA) //musim mit ")"
                return ESYN;


            int kam=tmpNode->data.jump; // zjistime kam se ma skocit
            ta_InsertJump(&ta,I_FCEJUMP,NULL, kam);


            getToken();
            if(error != EOK)
                return error;


            if(token.stav != EOL) //musim mit eol
                return ESYN;

            break;
        }
        else if((!(strcmp(token.data, "Chr"))) || (!(strcmp(token.data, "substr"))) || (!(strcmp(token.data, "length"))) || (!(strcmp(token.data, "Asc"))))// jestli neni nahodou vest funkce
        {
            error = vest();
            if(error != EOK)
                return error;
            break;
        }

        nazev = advMalloc(strlen(functionName)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
        sprintf(nazev,"#%s#%s", functionName, token.data); //vlozim string do promenne

        tmpNode = TSreadSymbol(nazev);
        if(tmpNode == NULL) //jmeno neni v tabulce
        {
            return ESEM;
        }

        cilovaAdresa = tmpNode;

        getToken();
        if(error != EOK)
            return error;

        if(token.stav != PRIRAZENI) //musim mit "="
        {
            fprintf(stderr, "Ocekavan znak: \"=\"\n");
            return ESYN;
        }

        error = prirazeni();
        if(error != EOK)
            return error;

        break;

    case KLIC_SLOVO: //klicove slovo
        //if((strcmp(token.data, "input"))) //input musi byt prirazeny nejsem si jist
           // error = ESYN;

        if(!(strcmp(token.data, "print"))) //<list>		->  "print" <print> <list>
            error = print();

        else if(!(strcmp(token.data, "if"))) //<list>	->  "if" <expr> "then" "eol" <list> "else" "eol" <list> "end" "if" <list>
        {
            int else_label = (label);
            int end_label = ++(label);
            (label)++;
            getToken(); //EXPR
            if(error != EOK)
                return error;

            error = pparser(); //vyhodnotim podminku

            ta_InsertJump(&ta,I_FJUMP,TSreadSymbol(neterm.data.nazev),else_label);

            if(error != EOK)
                return error;

            if((strcmp(token.data, "then")))
            {
                return ESYN;
            }
            getToken();
            if (token.stav != EOL)
            {
                return ESYN;
            }
            getToken(); //Nacti neco do listu
            if(error != EOK)
                return error;

            error = list();
            if(error != EOK)
                return error;

            ta_InsertJump(&ta,I_JUMP,TSreadSymbol(neterm.data.nazev),end_label);

            ta_InsertJump(&ta,I_LABEL,NULL, else_label);


            if((strcmp(token.data, "else")))
                return ESYN;
            getToken();
            if(token.stav  != EOL)
                return ESYN;
            getToken(); //Nacti neco do listu
            if(error != EOK)
                return error;

            error = list();
            if(error != EOK)
                return error;;

            ta_InsertJump(&ta,I_LABEL,NULL, end_label);

            if((strcmp(token.data, "end"))) //"end"
                return ESYN;

            getToken(); //"if"
            if(error != EOK)
                return error;

            if((strcmp(token.data, "if"))) //""
            {
                fprintf(stderr, "Ocekavan : \"if\"\n");
                return ESYN;
            }

        }
        else if(!(strcmp(token.data, "dowhile"))) //<list>	->  "dowhile" <expr> "eol" <list> "loop" "eol" <list>
        {
            int dowhile_label = (label);
            int dowhile_end_label = ++(label);
            label++;

            getToken(); //EXPR
            if(error != EOK)
                return error;

            ta_InsertJump(&ta, I_LABEL, NULL, dowhile_label);

            error = pparser();//////////////////////////////////

            ta_InsertJump(&ta, I_FJUMP, TSreadSymbol(neterm.data.nazev), dowhile_end_label);

            if(error != EOK)
                return error;

            if(error != EOK)
                return error;

            if(token.stav != EOL)// eol
                return ESYN;

            getToken(); //Nacti neco do listu
            if(error != EOK)
                return error;

            error = list();
            if(error != EOK)
                return error;

            ta_InsertJump(&ta, I_JUMP, TSreadSymbol(neterm.data.nazev), dowhile_label);

            if((strcmp(token.data, "loop"))) //"end"
                {
                    fprintf(stderr, "Ocekavan : \"loop\n");
                    return ESYN;
                }
            ta_InsertJump(&ta, I_LABEL,NULL, dowhile_end_label);

           /* getToken(); //"eol"
            if(error != EOK)
                return error;

            if(token.stav != EOL) //
            {
                fprintf(stderr, "Ocekavan : \"\\n\"\n");
                return ESYN;
            }
                */
        }

        else if(!(strcmp(token.data, "return"))) //<list>	->  "return" <expr> "eol"
        {
            getToken(); //EXPR
            if(token.stav != EOL)   //"eol"
            {
                error = pparser();

                if(error != EOK)
                    return error;

                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(neterm.data.nazev), NULL, for_return);

                if(error != EOK)
                    return error;

                if(token.stav != EOL) //"eol"
                {
                    fprintf(stderr, "Ocekavan znak: \"\\n\"\n");
                    return ESYN;
                }
            }
            ta_Insert(&ta, I_GOTO,NULL,NULL,NULL);
        }
        else return ESYN;

        if(error != EOK)
            return error;

        break;

    default: //pokud je cokoli jineho
        return ESYN;
    }

    getToken();
    if(error != EOK)
        return error;

    error = list();
    if(error != EOK)
        return error;

    return error;
}


/**
 * <print>		->	"(" <expr> ")" "eol"
 */
tError print()
{
    getToken();
    if(error != EOK)
        return error;

    isPrint = true; //zapamatuji si, ze jsem ve funkci print

    error = args();

    if(error != EOK)
        return error;

    getToken();
    if(error != EOK)
        return error;

    if(token.stav != EOL) //musi byt "\n"
        return ESYN;


    isPrint = false; //uz nejsem ve funkci write, tak nevyhodnocuji vyrazy
    return error;
}


/**
 * <read>	->  "input" "id" "EOL"
 */
tError input()
{
    nazev_fce=1;
    getToken();
    if(error != EOK)
        return error;

    if(token.stav != IDENTIFIK) //ID
        return ESYN;

    getToken();
    if(error != EOK)
        return error;

    if((token.stav == RETEZEC) || (token.stav == INTEGER) || (token.stav == DOUBLE) || (token.stav == EXP)) //musi byt STRING nebo CISLO
    {

        nameID++; //prictu k pocitadlu
        char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
        sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne
        tSymbol dataStromu; //vlozim do stromu jmeno funkce
        TSinitSymbol(&dataStromu);
        dataStromu.varFc = false;
        dataStromu.nazev = nazev;
        dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
        if(token.stav == RETEZEC)
        {
            dataStromu.value.s=token.data;
            dataStromu.typ = tString; //typ je string
        }
        else
        {
            dataStromu.typ = tDouble; //typ je double
            dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
        }
        if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
        {
            return ESEM;
        }
        else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

        ta_Insert(&ta, I_INPUT, TSreadSymbol(nazev),NULL, cilovaAdresa);

        getToken();
        if(error != EOK)
            return error;

        /*if(token.stav != PRAVA_ZAVORKA) //musi byt ")"
            return ESYN;
        */
        /*getToken();
        if(error != EOK)
            return error;
            */
        if(token.stav != EOL) //musi byt "eol"
            return ESYN;

        return error;
    }
    else return ESEM;
}


/**
 * <vest>			->	"substr" "(" string, number, number ")" "eol"
 * <vest>			->	"length" "(" string")" "eol"
 * <vest>			->	"Asc" "(" string ")" "eol"
 * <vest>			->	"Chr" "(" number ")" "eol"
 */

tError vest()
{
    if(error != EOK)
        return  error;

    pole_argumentu[0] = NULL;
    pole_argumentu[1] = NULL;
    pole_argumentu[2] = NULL;
    pole_argumentu[3] = NULL;

    int i = 0;

    if(isExprWr)
        isPrint = false;

    if(!(strcmp(token.data, "Chr"))) i = 1;
    else if(!(strcmp(token.data, "Asc"))) i = 2;
    else if(!(strcmp(token.data, "length"))) i = 3;
    else if(!(strcmp(token.data, "substr"))) i = 4;

    else return ESYN; //token neni vestavena funkce

    getToken();
    if(error != EOK)
        return error;

    if(token.stav != LEVA_ZAVORKA) //musi byt "("
        return ESYN;

    switch(i)
    {
    case 1: //Chr();
        isChr = true;
        error = args();

        if(error != EOK)
            return error;

        ta_Insert(&ta, I_Chr ,pole_argumentu[0], NULL, cilovaAdresa);
        if(isExprWr)
            neterm.data = cilovaAdresa->data;
        break;


    case 2: //Asc();
        isAsc = true;

        error = args();

        if(error != EOK)
            return error;

        ta_Insert(&ta, I_Asc ,pole_argumentu[0], NULL, cilovaAdresa);
        if(isExprWr)
            neterm.data = cilovaAdresa->data;
        break;


    case 3: //length();
        isLength = true;
        error = args();
        if(error != EOK)
            return error;

        ta_Insert(&ta, I_Length , pole_argumentu[0], NULL, cilovaAdresa);
        if(isExprWr)
            neterm.data = cilovaAdresa->data;
        break;

    case 4: //substr();
        isSubs = true;
        error = args();
        if(error != EOK)
            return error;

        ta_Insert(&ta, I_SUBS , pole_argumentu[0], pole_argumentu[1], pole_argumentu[2]);
        ta_Insert(&ta, I_SUBS , NULL, NULL, cilovaAdresa);
        if(isExprWr)
            neterm.data = cilovaAdresa->data;
        break;

    default: //tento pripad by snad nemel ani nikdy nastat... :-/
        return ESYN;
    }
    isAsc = false;
    isChr = false;
    isSubs = false;
    isLength = false;
    (*cislo_argumentu) = 1;
    if(token.stav != PRAVA_ZAVORKA) //musi byt ")"
        return ESYN;

    getToken();
    if(error != EOK)
        return error;

    if(isExprWr)
    {
        isPrint = true;
        if((token.stav != CARKA)&&(token.stav != PRAVA_ZAVORKA)) //musi byt ","
            return ESYN;
    }
    else if(token.stav != EOL) //musi byt "EOL"
        return ESYN;

    return error;
}

/**
 * <vardec>	->	EPS
 * <vardec>	->	"dim" ID <idt> as <type> "EOL" <vardec>
 */

tError varDeclar()
{
    if(token.stav != KLIC_SLOVO) //zrejme nejsou zadne deklarace
        {
            return error;
        }
    if((strcmp(token.data, "dim"))) //nejsou zadne deklarace
        {
            return error;
        }

    getToken();
    if(error != EOK)
        {
            return error;
        }
    if(token.stav != IDENTIFIK) //ID... za dim musi byt ID
       {
           return ESYN;
       }

    if((TSreadSymbol(token.data)) != NULL) //promenna ma stejne jmeno jako funkce
        {
            return ESEM;
        }

    tSymbol dataStromu; //NACTENI NAZEV PROMENNE A TEST JESTLI NENI JIZ DEKLAROVAN
    TSinitSymbol(&dataStromu);
    dataStromu.varFc = false;
    char *nazev = advMalloc(strlen(functionName)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
    sprintf(nazev,"#%s#%s", functionName, token.data); //vlozim string do promenne
    name.nazev = nazev;
    dataStromu.nazev = nazev;
    if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
    {
        fprintf(stderr, "Identifikator: \"%s\" byl jiz deklarovan\n", token.data);
        return ESEM;
    }

    TSvlozSymbol(dataStromu); //jinak vloz do tabulky
    tBTSUzolPtr tmpNode = TSreadSymbol(nazev);

    getToken(); // musi nasledovat as
    if(error != EOK)
        return error;
    if (token.stav != KLIC_SLOVO)
        return ESYN;
    if ((strcmp(token.data, "as")))// nenasel as
        return ESYN;
    getToken(); //datovy typ
    if(error != EOK)
        return error;

    if (token.stav != KLIC_SLOVO)// musi nasledovat klicova slova , protoze navratovy typ je integer , double nebo string
    {
        return ESYN;
    }
    else
    {
        if (((strcmp(token.data, "integer"))) && ((strcmp(token.data, "double"))) && ((strcmp(token.data, "double"))))// jestli neobsahuje integer double nebo string chyba
        {
            return ESYN;
        }
        else if ((strcmp(token.data, "integer")))// integer bez prirazeni > implicitni nula
        {
            getToken();
            if (token.stav != PRIRAZENI)
            {
                ta_Insert(&ta, I_ASSIGN, tmpNode, 0, cilovaAdresa);
                return error;
            }
            else // kdzy token je =
            {
                isDim = true;
                error = prirazeni();
                isDim = false;
            }

        }
        else if ((strcmp(token.data, "double")))// double bez prirazeni > implicitni 0.0
        {
            getToken();
            if (token.stav != PRIRAZENI)
            {
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(neterm.data.nazev), NULL, cilovaAdresa);
                return error;
            }
            else// kdyz token je =
            {
                isDim = true;
                error = prirazeni();
                isDim = false;
            }
        }
        else // string bez prirazeni > implicitni !""
        {
            getToken();
            if (token.stav != PRIRAZENI)
            {
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(neterm.data.nazev), NULL , cilovaAdresa);
                return error;
            }
            else// kdyz token je =
            {
                isDim = true;
                error = prirazeni();
                isDim = false;
            }
        }
    }
    return error;

}

/**
 * <prirazeni>		->	"=" <litexpr>
 */
tError prirazeni()
{
    if(error != EOK)
        return error;

    if(token.stav != PRIRAZENI)//"="
        return error;

    error = litExpr();
    if(error != EOK)
        return error;

    return error;
}

/**
 * <litexpr>		->	<expr>	"EOL"
 * <litexpr>		->	<vest>
 * <litexpr>		->	<input>
 * <litexpr>		->	ID"(" ")"
 */
tError litExpr() // muzeme priradit k promenne bud funkci, hodnotu promenne, hodnotu nebo vestfunkci
{
    if(isWriteFunc == false)
        getToken();
    if(error != EOK)
        return error;

    tBTSUzolPtr tmpNode = NULL;// = TSreadSymbol(nazev);
    tBTSUzolPtr tmpNode2 = NULL;// = TSreadSymbol(token.data); //pokud je token se jmenem funkce
    char *nazev = NULL;

    switch (token.stav)//test co prirazujeme
    {
    case KLIC_SLOVO: //<litexpr> -> <input>
        if(!(strcmp(token.data, "input"))) // hodnota promenne
        {
            error = input();
            break;
        }

    case IDENTIFIK: //ID je ve skutecnosti <litexpr> -> <vest>
        if(!(strcmp(token.data, "asc")))
        {
            error = vest(); //vestaveny prikaz
            break;
        }

        else if(!(strcmp(token.data, "chr")))
        {
            error = vest(); //vestaveny prikaz
            break;
        }

        else if(!(strcmp(token.data, "length")))
        {
            error = vest(); //vestaveny prikaz
            break;
        }
        else if(!(strcmp(token.data, "substr")))
        {
            error = vest(); //vestaveny prikaz
            break;
        }

        if(error != EOK) //nekde ve <vest> nastala chyba
            return error;

    default: //pokud to neni input ani vestavena funkce, tak to musi byt <EXPR>

        if(token.stav == IDENTIFIK)
        {
            tmpNode2 = TSreadSymbol(token.data); //pokud je token se jmenem funkce
            if(tmpNode2 == NULL) //pokud jsem nenasel funkci
            {
                nazev = advMalloc(strlen(functionName)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
                sprintf(nazev,"#%s#%s", functionName, token.data); //vlozim string do promenne
                tmpNode2 = TSreadSymbol(nazev);
                if(tmpNode2 == NULL) //pokud jsem NEnasel promennou
                {
                    fprintf(stderr, "Promenna: \"%s\" nebyla deklarovana\n", token.data);
                    return ESEM;
                }
            }
            else //nasel jsem funkci
            {
                tmpNode = tmpNode2;
            }
        }


        if (((tmpNode != NULL)) && ((tmpNode->data.varFc) == true))
        {
            volana_fce = tmpNode;
            cilovaAdresaFce = tmpNode;
            getToken(); //pokusim se nacist zavorku
            if(error != EOK)
                return error;

            if(token.stav != LEVA_ZAVORKA) // "("
                return ESYN;

            bool bylPrint = isPrint; //pokud jsme ve print
            isPrint = false;

            error = args();

            isPrint = bylPrint;

            if(error != EOK)
                return error;

            if(isPrint== false)
            {
                if(token.stav != PRAVA_ZAVORKA) // ")"
                    return ESYN;
                getToken();
            }


            int kam = tmpNode->data.jump;
            ta_InsertJump(&ta,I_FCEJUMP,NULL, kam);

            if(error != EOK)
                return error;
            if(isWriteFunc == false)
            {
                if(token.stav != EOL) // "\n"
                    return ESYN;
            }


            ta_Insert(&ta, I_ASSIGN, cilovaAdresaFce, NULL, cilovaAdresa);
        }
        else //jinak je promenna nebo cislo ,str atd...
        {
            error = pparser();
            if(error != EOK)
                return error;

            ta_Insert(&ta, I_ASSIGN, TSreadSymbol(neterm.data.nazev), NULL, cilovaAdresa);
            if(error != EOK)
                return error;
            if(token.stav != EOL) // "\n"
                return ESYN;
        }
    }

    if(error != EOK)
        return error;

    return error;
}

tError args()
{
    getToken(); //pokusim se nacist ID
    if(error != EOK)
        return error;

    if(token.stav == PRAVA_ZAVORKA) // ")" //funkce nema zadne parametry
        return error;

    if(token.stav == IDENTIFIK) // ID
    {
        if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
        {
            tBTSUzolPtr tmpNode = TSreadSymbol(token.data);
            if(tmpNode == NULL) //pokud jsem nenarazil na volani funkce
            {
                error = pparser();

                if(error != EOK)
                    return error;

                ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
            }
            else
            {
                isWriteFunc = true;
                nameID++; //prictu k pocitadlu
                char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
                sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

                tSymbol dataStromu; //vlozim do stromu jmeno funkce
                TSinitSymbol(&dataStromu);
                dataStromu.varFc = false;
                dataStromu.nazev = nazev;
                //dataStromu.typ = tDouble;

                if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
                {
                    return ESEM;
                }
                else TSvlozSymbol(dataStromu); //jinak vloz do tabulky


                tBTSUzolPtr tmpCil = cilovaAdresa;
                cilovaAdresa = TSreadSymbol(nazev);
                error = litExpr();
                ta_Insert(&ta, I_PRINT, cilovaAdresa,NULL, NULL);
                if(error != EOK)
                    return error;

                //ta_Insert(&ta, I_ASSIGN, tmpNode, NULL, cilovaAdresa);
                cilovaAdresa = tmpCil;
                isWriteFunc = false;
                isPrint = true;
                getToken();
                if(error != EOK)
                    return error;

                //if(token.stav != CARKA) // ","
                // return ESYN;
            }
        }
        else if(isPrint == false) //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            char *nazev = advMalloc(strlen(functionName)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
            sprintf(nazev,"#%s#%s", functionName, token.data); //vlozim string do promenne
            tBTSUzolPtr tmpNode = TSreadSymbol(nazev);
            if(tmpNode == NULL) //jmeno neni v tabulce
            {
                return ESEM;
            }
            if(isAsc || isChr || isSubs || isLength)
            {
                pole_argumentu[0] = tmpNode;
            }
            else
            {
                volana_fce = volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, tmpNode, NULL, volana_fce);

            }
        }
    }
    else if((token.stav == DOUBLE))
    {
        if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
        }

        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne
            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazev = nazev;
            dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
            dataStromu.typ = tDouble; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }

            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isAsc || isChr || isSubs || isLength)
            {
                pole_argumentu[0] = TSreadSymbol(nazev);
            }
            else
            {
                volana_fce = volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

            }
        }
    }
    else if((token.stav == INTEGER))
    {
        if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne
            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazev = nazev;
            dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
            dataStromu.typ = tDouble; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }
            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isAsc || isChr || isSubs || isLength)
            {
                pole_argumentu[0] = TSreadSymbol(nazev);
            }
            else
            {
                volana_fce = volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

            }
        }
    }
    else if((token.stav == EXP))
    {
        if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string.
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne
            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazev = nazev;
            dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
            dataStromu.typ = tDouble; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }
            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isAsc || isLength || isSubs || isChr)
            {
                pole_argumentu[0] = TSreadSymbol(nazev);
            }
            else
            {
                volana_fce = volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);
            }
        }
    }
    else if((token.stav == RETEZEC))
    {
        if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne
            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazev = nazev;
            dataStromu.value.s = token.data; //prevedu string na hodnotu
            dataStromu.typ = tString; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }
            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isAsc || isChr || isSubs || isLength)
            {
                pole_argumentu[0] = TSreadSymbol(nazev);
            }
            else
            {
                volana_fce = volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);
            }
        }
    }
    else if((token.stav == KLIC_SLOVO))
    {
        if((!strcmp(token.data, "true")) || (!strcmp(token.data, "false")))
        {
            if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
            {
                error = pparser();

                if(error != EOK)
                    return error;

                ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
            }
            else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
            {
                nameID++; //prictu k pocitadlu
                char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
                sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne
                tSymbol dataStromu; //vlozim do stromu jmeno funkce
                TSinitSymbol(&dataStromu);
                dataStromu.varFc = false;
                dataStromu.nazev = nazev;
                if(!(strcmp(token.data, "true")))
                    dataStromu.value.b = true; //prevedu string na hodnotu
                else dataStromu.value.b = false;
                dataStromu.typ = tBool; //typ je double
                if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
                {
                    return ESEM;
                }
                else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

                if(isChr || isAsc || isSubs || isLength)
                {
                    pole_argumentu[0] = TSreadSymbol(nazev);
                }
                else
                {
                    volana_fce = volana_fce->data.nextNode;
                    ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);
                }

            }
        }
    }
    else if((token.stav == MINUS))
    {
        if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            getToken();
            if(error != EOK)
                return error;

            if((token.stav == DOUBLE) || (token.stav == INTEGER) || (token.stav == EXP))
            {
                nameID++; //prictu k pocitadlu
                char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
                sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne
                tSymbol dataStromu; //vlozim do stromu jmeno funkce
                TSinitSymbol(&dataStromu);
                dataStromu.varFc = false;
                dataStromu.nazev = nazev;
                dataStromu.value.d = (atof(token.data))*(-1); //prevedu string na hodnotu
                dataStromu.typ = tDouble; //typ je double
                if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
                {
                    return ESEM;
                }
                else TSvlozSymbol(dataStromu); //jinak vloz do tabulky
                volana_fce = volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

            }
            else return ESEM;
        }
    }
    else if(token.stav == LEVA_ZAVORKA) // ID
    {
        if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
        }
        else return ESYN;
    }

    else return ESEM;

    error = argsNext();
    if(error != EOK)
        return error;

    return error;
}
tError argsNext()
{
    if(isPrint == false) //pokud jsme ve printu, tak token byl uz nacten v pparser()
        getToken();

    if(error != EOK)
        return error;

    if(token.stav == PRAVA_ZAVORKA) // ")" //funkce nema dalsi parametry
        return error;

    if(token.stav != CARKA) // "," //ted musi byt carka
        return ESYN;      //<params_next>	->	"," ID <params_next>

    getToken();
    if(error != EOK)
        return error;

    if(token.stav == IDENTIFIK) // ID
    {
        if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
        {
            tBTSUzolPtr tmpNode = TSreadSymbol(token.data);
            if(tmpNode == NULL) //pokud jsem nenarazil na volani funkce
            {
                error = pparser();

                if(error != EOK)
                    return error;

                ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
            }
            else
            {
                isWriteFunc = true;
                nameID++; //prictu k pocitadlu
                char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
                sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne
                tSymbol dataStromu; //vlozim do stromu jmeno funkce
                TSinitSymbol(&dataStromu);
                dataStromu.varFc = false;
                dataStromu.nazev = nazev;
                //dataStromu.typ = tDouble;

                if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
                {
                    return ESEM;
                }
                else TSvlozSymbol(dataStromu); //jinak vloz do tabulky


                tBTSUzolPtr tmpCil = cilovaAdresa;
                cilovaAdresa = TSreadSymbol(nazev);
                error = litExpr();
                ta_Insert(&ta, I_PRINT, cilovaAdresa,NULL, NULL);
                if(error != EOK)
                    return error;

                //ta_Insert(&ta, I_ASSIGN, tmpNode, NULL, cilovaAdresa);
                cilovaAdresa = tmpCil;
                isWriteFunc = false;
                isPrint = true;
                getToken();
                if(error != EOK)
                    return error;

                //if(token.stav != CARKA) // ","
                // return ESYN;
            }
        }
        else if (isPrint == false) //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            char *nazev = advMalloc(strlen(functionName)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
            sprintf(nazev,"#%s#%s", functionName, token.data); //vlozim string do promenne
            tBTSUzolPtr tmpNode = TSreadSymbol(nazev);
            if(tmpNode == NULL) //jmeno neni v tabulce
            {
                return ESEM;
            }

            if(isAsc || isChr || isSubs || isLength)
            {
                if(((*cislo_argumentu) % 3) == 0)
                {
                    pole_argumentu[0] = (tBTSUzolPtr) advRealloc( pole_argumentu, sizeof(tBTSUzolPtr) * (*cislo_argumentu+3));
                }
                pole_argumentu[(*cislo_argumentu)]= tmpNode;
                (*cislo_argumentu)++;
            }
            else
            {
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, tmpNode, NULL, volana_fce);

            }
        }
    }
    else if((token.stav == DOUBLE))
    {
        if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne
            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazev = nazev;
            dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
            dataStromu.typ = tDouble; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }
            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isChr || isAsc || isSubs || isLength)
            {
                pole_argumentu[(*cislo_argumentu)]=TSreadSymbol(nazev);
                (*cislo_argumentu)++;
            }
            else
            {
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

            }
        }
    }
    else if((token.stav == INTEGER))
    {
        if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne
            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazev = nazev;
            dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
            dataStromu.typ = tDouble; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }
            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isAsc || isChr || isSubs || isLength)
            {
                pole_argumentu[(*cislo_argumentu)]=TSreadSymbol(nazev);
                (*cislo_argumentu)++;
            }
            else
            {
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

            }
        }
    }
    else if((token.stav == EXP))
    {
        if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne
            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazev = nazev;
            dataStromu.value.d = atof(token.data); //prevedu string na hodnotu
            dataStromu.typ = tDouble; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }
            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isAsc || isChr || isSubs || isLength)
            {
                pole_argumentu[(*cislo_argumentu)]=TSreadSymbol(nazev);
                (*cislo_argumentu)++;
            }
            else
            {
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

            }
        }
    }
    else if((token.stav == RETEZEC))
    {
        if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            nameID++; //prictu k pocitadlu
            char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
            sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne
            tSymbol dataStromu; //vlozim do stromu jmeno funkce
            TSinitSymbol(&dataStromu);
            dataStromu.varFc = false;
            dataStromu.nazev = nazev;
            dataStromu.value.s = token.data; //prevedu string na hodnotu
            dataStromu.typ = tString; //typ je double
            if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
            {
                return ESEM;
            }
            else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

            if(isAsc || isChr || isSubs || isLength)
            {
                pole_argumentu[(*cislo_argumentu)]=TSreadSymbol(nazev);
                (*cislo_argumentu)++;
            }
            else
            {
                volana_fce=volana_fce->data.nextNode;
                ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

            }
        }
    }
    else if((token.stav == KLIC_SLOVO))
    {
        if((!strcmp(token.data, "true")) || (!strcmp(token.data, "false")))
        {
            if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
            {
                error = pparser();

                if(error != EOK)
                    return error;

                ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
            }
            else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
            {
                nameID++; //prictu k pocitadlu
                char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
                sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne

                tSymbol dataStromu; //vlozim do stromu jmeno funkce
                TSinitSymbol(&dataStromu);
                dataStromu.varFc = false;
                dataStromu.nazev = nazev;
                if(!(strcmp(token.data, "true")))
                    dataStromu.value.b = true; //prevedu string na hodnotu
                else dataStromu.value.b = false;
                dataStromu.typ = tBool; //typ je double
                if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
                {
                    return ESEM;
                }
                else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

                if(isAsc || isChr || isSubs || isLength)
                {
                    pole_argumentu[(*cislo_argumentu)]=TSreadSymbol(nazev);
                    (*cislo_argumentu)++;
                }
                else
                {
                    volana_fce=volana_fce->data.nextNode;
                    ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

                }

            }
        }
    }
    else if((token.stav == MINUS))
    {
        if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
        }
        else //jinak se jedna o argumenty funkci, ktere nejsou vyrazy
        {
            getToken();
            if(error != EOK)
                return error;

            if((token.stav == DOUBLE) || (token.stav == EXP))
            {
                nameID++; //prictu k pocitadlu
                char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
                sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne
                tSymbol dataStromu; //vlozim do stromu jmeno funkce
                TSinitSymbol(&dataStromu);
                dataStromu.varFc = false;
                dataStromu.nazev = nazev;
                dataStromu.value.d = (atof(token.data))*(-1); //prevedu string na hodnotu
                dataStromu.typ = tDouble; //typ je double
                if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
                {
                    return ESEM;
                }
                else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

                if(isAsc || isChr || isSubs || isLength)
                {
                    pole_argumentu[(*cislo_argumentu)]=TSreadSymbol(nazev);
                    (*cislo_argumentu)++;
                }
                else
                {
                    volana_fce = volana_fce->data.nextNode;
                    ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

                }

            }
            if((token.stav == INTEGER) || (token.stav == EXP))
            {
                nameID++; //prictu k pocitadlu
                char *nazev = advMalloc(sizeof(char)*25); //alokuji pamet pro string
                sprintf(nazev,"@prom_%u",nameID++); //vlozim string do promenne
                tSymbol dataStromu; //vlozim do stromu jmeno funkce
                TSinitSymbol(&dataStromu);
                dataStromu.varFc = false;
                dataStromu.nazev = nazev;
                dataStromu.value.i = (atoi(token.data))*(-1); //prevedu string na hodnotu
                dataStromu.typ = tInt; //typ je int
                if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
                {
                    return ESEM;
                }
                else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

                if(isAsc || isChr || isSubs || isLength)
                {
                    pole_argumentu[(*cislo_argumentu)]=TSreadSymbol(nazev);
                    (*cislo_argumentu)++;
                }
                else
                {
                    volana_fce = volana_fce->data.nextNode;
                    ta_Insert(&ta, I_ASSIGN, TSreadSymbol(nazev), NULL, volana_fce);

                }

            }

            else return ESEM;
        }
    }
    else if(token.stav == LEVA_ZAVORKA) // ID
    {
        if(isPrint) //pokud potrebuji zavolat vyhodnoceni vyrazu u print()
        {
            error = pparser();

            if(error != EOK)
                return error;

            ta_Insert(&ta, I_PRINT, TSreadSymbol(neterm.data.nazev),NULL, NULL);
        }
        else return ESYN;
    }
    else return ESEM;

    error = argsNext();
    if(error != EOK)
        return error;

    return error;
}

/**
 * <params>		->	ID <params_next>
 * <params>		->	EPS
 */
tError params()// pocet parametru funkce
{
    getToken();
    functionNodeTmp->data.argCounter = 0;//counter
    if(error != EOK)
        return error;

    if(token.stav == PRAVA_ZAVORKA) // ")" //funkce nema parametry
        return error; //<params>	->	EPS

    if(token.stav == IDENTIFIK) //<params>	->	ID <params_next>
    {
        if((TSreadSymbol(token.data)) != NULL) //promenna ma stejne jmeno jako funkce
            return ESEM;

        tSymbol dataStromu; //vlozim do stromu jmeno funkce
        TSinitSymbol(&dataStromu);
        dataStromu.varFc = false;
        char *nazev = advMalloc(strlen(functionName)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
        sprintf(nazev,"#%s#%s", functionName, token.data); //vlozim string do promenne
        dataStromu.nazev = nazev;
        if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
        {
            fprintf(stderr, "Identifikator: \"%s\" byl jiz deklarovan\n", token.data);
            return ESEM;
        }
        else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

        functionNodeTmp->data.argCounter = functionNodeTmp->data.argCounter + 1; //prictu dalsi argument
        functionNode->data.nextNode = TSreadSymbol(nazev);
        functionNode = TSreadSymbol(nazev);
        functionNode->data.nextNode = NULL;
        getToken();
        if (token.stav != KLIC_SLOVO)
        {
            return ESYN;
        }
        if ((strcmp(token.data, "as")))
        {
            return ESYN;
        }
        getToken();
        if (token.stav != KLIC_SLOVO)
        {
            return ESYN;
        }
        if ((strcmp(token.data, "integer")) && (strcmp(token.data, "double")) && (strcmp(token.data, "string")))
        {
            return ESYN;
        }
        error = paramsNext(); //<params_next>
        if(error != EOK)
            return error;
    }

    return error;
}

/**
 * <params_next>	->	EPS
 * <params_next>	->	"," ID <params_next>
 */
tError paramsNext()
{
    getToken();
    if(error != EOK)
        return error;

    if(token.stav == PRAVA_ZAVORKA) // ")" //funkce nema dalsi parametry
        return error;

    if(token.stav != CARKA) // "," //ted musi byt carka
        return ESYN;      //<params_next>	->	"," ID <params_next>

    getToken(); //nacti ID
    if(error != EOK)
        return error;
    if(token.stav == IDENTIFIK) //ID
    {
        if((TSreadSymbol(token.data)) != NULL) //promenna ma stejne jmeno jako funkce
            return ESEM;

        tSymbol dataStromu; //vlozim do stromu jmeno funkce
        TSinitSymbol(&dataStromu);
        dataStromu.varFc = false;
        char *nazev = advMalloc(strlen(functionName)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
        sprintf(nazev,"#%s#%s", functionName, token.data); //vlozim string do promenne
        dataStromu.nazev = nazev;
        if((TSreadSymbol(nazev)) != NULL) //pokud uz jmeno bylo v tabulce
        {
            fprintf(stderr, "Identifikator: \"%s\" byl jiz deklarovan\n", token.data);
            return ESEM;
        }
        else TSvlozSymbol(dataStromu); //jinak vloz do tabulky

        functionNodeTmp->data.argCounter = functionNodeTmp->data.argCounter + 1; //prictu dalsi argument
        functionNode->data.nextNode = TSreadSymbol(nazev);
        functionNode = TSreadSymbol(nazev);
        functionNode->data.nextNode = NULL;
        getToken();
        if (token.stav != KLIC_SLOVO)
        {
            return ESYN;
        }
        if ((strcmp(token.data, "as")))
        {
            return ESYN;
        }
        getToken();
        if (token.stav != KLIC_SLOVO)
        {
            return ESYN;
        }
        if ((strcmp(token.data, "integer")) && (strcmp(token.data, "double")) && (strcmp(token.data, "string")))
        {
            return ESYN;
        }

        error = paramsNext(); //zavolej se zase
    }
    else return ESYN; //za carkou musi byt ID

    return error;
}

void najdiKolize(tBTSUzolPtr uzel)// TEST JESTLI NENI FUNKCE DVAKRAT DEKLAROVANA
{
    if(uzel == NULL)
        return;
    if(error != EOK)
        return ;

    if((uzel->klic[0] != '#') && (uzel->klic[0] != '@')) //pokud jsem nasel uzel s funkci
    {
        char *nazev = advMalloc(strlen(uzel->klic)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
        sprintf(nazev,"#%s#%s", uzel->klic, token.data); //vlozim string do promenne
        if((TSreadSymbol(nazev)) != NULL) //promenna uz existuje
            error = ESEM;
        else
        {
            if(uzel->lptr != NULL)
                najdiKolize(uzel->lptr);
            if(error != EOK)
                return ;

            if(uzel->rptr != NULL)
                najdiKolize(uzel->rptr);
        }
    }
}
