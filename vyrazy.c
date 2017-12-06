
/**
 * Predmet: IFJ / IAL
 * Projekt: Implementace prekladace imperativniho jazyka IFJ17
 * Varianta:Tym 031, varianta I
 * Soubor:   vyrazy.c
 * Datum:   14.11.2017
 * Autori:  Kozouskova Aneta	<xkozou00@stud.fit.vutbr.cz>,
 *          Sencuch Filip	    <xsencu01@stud.fit.vutbr.cz>,
 *          Nguyen QuangTrang	<xnguye11@stud.fit.vutbr.cz>,
 *          Pribyl Tomas	    <xpriby17@stud.fit.vutbr.cz>
 */

 /* Hlavickove soubory
 */
#include "vyrazy.h"
#include "garbage_collector.h"

#define Asc   1
#define Chr   2
#define Length   3
#define SUBSTR 4
#define FUNC   5
#define NOVEST 6
int jeInt = 0;
int jeDouble = 0;

//inicializace &zasobniku
void VSinit(tVZasobnik *zasobnik)
{
    zasobnik->vrchol = NULL;
}

//vlozime prvek na &zasobnik
void VSpush(tVZasobnik *zasobnik, tData item)
{
    tZPrvekptr pom;

    // alokujeme element
    pom = advMalloc(sizeof(struct tZasPrvek));
    if (pom == NULL)
    {
        error = EOST;
        return;
    }
    // nastavime hodnoty
    pom->item = item;
    pom->ptr = zasobnik->vrchol;
    zasobnik->vrchol = pom;
}

//odstranime prvek z vrcholu zasobniku
void VSpop(tVZasobnik *zasobnik)
{

    tZPrvekptr pom;

    // kdyz neni prazdny
    if (zasobnik->vrchol != NULL)
    {
        pom = zasobnik->vrchol;
        zasobnik->vrchol = zasobnik->vrchol->ptr;
    }
}

//precteme z vrcholu &zasobniku
void VStop(tVZasobnik *zasobnik, tData *item)
{
    *item = zasobnik->vrchol->item;
}

//kombinace toppop
void VStoppop(tVZasobnik *zasobnik, tData *item)
{
    if (!(VSempty(zasobnik)))
    {
        VStop(zasobnik, item);
        VSpop(zasobnik);
    }
}

//kontrola zda neni prazdny
bool VSempty(tVZasobnik *zasobnik)
{
    if (zasobnik->vrchol != NULL)
        return false;

    return true;
}

//definujeme max. velikost precedencni tabulky
#define MAXTABINDEX 17

/**
 * Inicializace precedencni tabulky
 */

const tPrvkyindexu Ptabulka [MAXTABINDEX][MAXTABINDEX] =
{              // 1 2  3  4  5  6  7  8  9  10 11 12 13 14  15 16 17
    //	          +  -  /  *  \  (  )  = <>  <= >= <  > ID  F  ,  $
    /*1  +  */  { V, V, M, M, M, M, V, V, V, V, V, V, V, M, M, M, M },
    /*2  -  */  { V, V, M, M, M, M, V, V, V, V, V, V, V, M, M, M, M },
    /*3  /  */  { V, V, V, V, V, M, V, V, V, V, V, V, V, M, M, M, M },
    /*4  *  */  { V, V, V, V, V, M, V, V, V, V, V, V, V, M, M, M, M },
    /*5  \  */  { V, V, M, M, V, M, V, V, V, V, V, V, V, M, M, M, M },
    /*6  ( */   { M, M, M, M, M, M, R, M, M, M, M, M, M, M, M, R, M },
    /*7  ) */   { V, V, V, V, V, E, V, V, V, V, V, V, V, E, E, V, V },
    /*8  = */   { M, M, M, M, M, M, V, V, V, V, V, V, V, M, M, M, M },
    /*9 <> */   { M, M, M, M, M, M, V, V, V, V, V, V, V, M, M, M, M },
    /*10 <=  */ { M, M, M, M, M, M, V, V, V, V, V, V, V, M, M, M, M },
    /*11 >=  */ { M, M, M, M, M, M, V, V, V, V, V, V, V, M, M, M, M },
    /*12 <  */  { M, M, M, M, M, M, V, V, V, V, V, V, V, M, M, M, M },
    /*13 >  */  { M, M, M, M, M, M, V, V, V, V, V, V, V, M, M, M, M },
    /*14 ID */  { V, V, V, V, V, E, V, V, V, V, V, V, V, E, E, V, V },
    /*15 F  */  { E, E, E, E, E, R, E, E, E, E, E, R, E, E, E, E, E },
    /*16 ,  */  { M, M, M, M, M, R, M, M, M, M, M, M, M, M, M, E, E },
    /*17 $  */  { M, M, M, M, M, M, E, M, M, M, M, M, M, M, M, E, E },
};

unsigned int nameID = 0;
int vst;

//funkce prevede token na prislusny index do precedencni tabulky,v pripade ID a konstant je ulozi i do tabulky symbolu
tError fetchIndex(tToken token, tData *sloupec, int *counter)
{
    char *nazevModif;
    tBTSUzolPtr pom;

    switch (token.stav)
    {
    case PLUS:
        sloupec->p = PLUS;
        break;				//+
    case MINUS:
        sloupec->p = MINUS;
        break;				//-
    case KRAT:
        sloupec->p = KRAT;
        break;				// *
    case DELENO_DESET:
        sloupec->p = DELENO_D;
        break;				// /
    case DELENO_CELY:
        sloupec->p = DELENO_C;
        break;              // \ A
    case JE_ROVNO:
        sloupec->p = ROVNITKO;
        break;			//=
    case NENI_ROVNO:
        sloupec->p = NEROVNITKO;
        break; 		// <>
    case MENSI_ROVNO:
        sloupec->p = MENSIROVNO;
        break; 		//<=
    case VETSI_ROVNO:
        sloupec->p = VETSIROVNO;
        break;		//>=
    case VETSI:
        sloupec->p = VICE;
        break;				//>
    case MENSI:
        sloupec->p = MENE;
        break;				//<
    case LEVA_ZAVORKA:
        sloupec->p = LZAVORKA;
        (*counter)++ ;
        break;			//(
    case PRAVA_ZAVORKA:
        sloupec->p = PZAVORKA;
        (*counter)-- ;
        break;			//)
//narazili jsme na číselnou konstantu
    case INTEGER:
        sloupec->p = ID;				//nastavime pseudoznak zasobniku
        sloupec->data.varFc = false;
        sloupec->data.typ = tInt; //inicializace hodnot pro vlozeni do TS
        sloupec->data.value.d = atoi(token.data);
        char *nazev = advMalloc(sizeof(char)*25);	//generovani klice pro TS
        sprintf(nazev,"@prom_%u",nameID++);
        sloupec->data.nazev = nazev;
        if((TSreadSymbol(nazev)) != NULL)  //pokud uz jmeno bylo v tabulce
        {
            return ESEM;
        }
        TSvlozSymbol(sloupec->data);
        jeInt = 1;
        break;

    case DOUBLE:
    case EXP:
        sloupec->p = ID;				//nastavime pseudoznak zasobniku
        sloupec->data.varFc = false;
        sloupec->data.typ = tDouble; //inicializace hodnot pro vlozeni do TS
        sloupec->data.value.d = atof(token.data);
        char *nazev1 = advMalloc(sizeof(char)*25);	//generovani klice pro TS
        sprintf(nazev1,"@prom_%u",nameID++);
        sloupec->data.nazev = nazev1;
        if((TSreadSymbol(nazev1)) != NULL)  //pokud uz jmeno bylo v tabulce
        {
            return ESEM;
        }
        TSvlozSymbol(sloupec->data);
        jeDouble = 1;
        break;
        //string
    case RETEZEC:
        sloupec->p = ID;
        sloupec->data.varFc = false;
        sloupec->data.typ = tString;
        if(token.data != NULL)
            sloupec->data.value.s = token.data;
        else sloupec->data.value.s = "\0";
        char *str = advMalloc(sizeof(char)*25);
        sprintf(str,"@prom_%u",nameID++);
        sloupec->data.nazev = str;
        if((TSreadSymbol(str)) != NULL)  //pokud uz jmeno bylo v tabulce
        {
            fprintf(stderr,"2nemuzeme vlozit, stejna promenna\n");
            return ESEM;
        }
        TSvlozSymbol(sloupec->data);
        break;
        //ID
    case IDENTIFIK:
        if(isLocal) {
            error = ESEM;
            return ESEM;
        }
        if(!(strcmp(token.data, "asc"))) vst=Asc;
        else if(!(strcmp(token.data, "chr"))) vst=Chr;
        else if(!(strcmp(token.data, "length"))) vst=Length;
        else if(!(strcmp(token.data, "substr"))) vst=SUBSTR;
        //else if(!(strcmp(token.data, functionName))){  vst=FUNC;}
        else
        {
            nazevModif = advMalloc(strlen(functionName)+(strlen(token.data))+3); //delka funkce + delka promenne + 2*# + 1*\0
            if(functionName != NULL && token.data !=NULL)
                sprintf(nazevModif,"#%s#%s", functionName, token.data);//vlozim string do promenne
            if((pom = TSreadSymbol(nazevModif)) == NULL)   //pokud neni bylo v tabulce
            {
                fprintf(stderr,"promenna neni v tabulce chyba\n");
                return ESEM;
            }
            sloupec->p = ID;
            sloupec->data.nazev = pom->data.nazev;
            sloupec->data.typ = pom->data.typ;
            sloupec->data.nextNode=pom->data.nextNode;
        }
        break;
        //klicove slovo znaci bud NULL nebo BOOL nebo konec
    case KLIC_SLOVO:
        //narazili jsme na boolean
        if(!(strcmp(token.data, "false")) || !(strcmp(token.data, "true")))
        {
            sloupec->p = ID;
            sloupec->data.varFc = false;
            sloupec->data.typ = tBool;
            if(!(strcmp(token.data, "true")))
                sloupec->data.value.b = true;  //prevedu string na hodnotu
            else sloupec->data.value.b = false;
            char *b = advMalloc(sizeof(char)*25);
            sprintf(b,"@prom_%u",nameID++);
            sloupec->data.nazev = b;
            if((TSreadSymbol(b)) != NULL)  //pokud uz jmeno bylo v tabulce
            {
                fprintf(stderr,"5nemuzeme vlozit, stejna promenna\n");
                return ESEM;
            }
            TSvlozSymbol(sloupec->data);
        }

        if(!(strcmp(token.data, "then")) || !(strcmp(token.data, "do")))
        {
            sloupec->p = KDOLAR;
        }
        break;

    case CARKA:
        sloupec->p = KDOLAR;
        break;
    case STREDNIK:
        sloupec->p = KDOLAR;
        break;
    default:
        fprintf(stderr,"Chyba spatny znak fetchindex\n");
        return ESYN;
    }
    return EOK;
}

//funkce provadejici redukci &zasobniku podle pravidel
//E->ID E->(E) E->E OP E NETERM-> E
tError redukce(tVZasobnik *zasobnik, tVZasobnik *zasobnik2 )
{
    tData pom;   		//polozky zasobniku
    tData pom2;
    tData pom3;
    tData pom4;
    TSinitSymbol(&pom.data);
    TSinitSymbol(&pom2.data);
    TSinitSymbol(&pom3.data);
    TSinitSymbol(&pom4.data);

    int ok;			//spravny operator
    size_t perator;	//promenna pro ulozeni operatoru

//zkontrolujeme vrchol zasobniku
    VStop(zasobnik, &pom);
    while(pom.p != MENSITKO && (!VSempty(zasobnik)))   //dokud nenarazime na < odkladame na pomocny zasobnik
    {
        VStoppop(zasobnik, &pom2);
        VSpush(zasobnik2, pom2);
        VStop(zasobnik, &pom);
    }
//pokud jsme nasli mensitko
    if(pom.p == MENSITKO)
    {
        VStoppop(zasobnik2, &pom2);	//precteme odlozeny prvek a vyjmeme jej
        //prvni nacteny id	a prvni pravidlo E->id
        if(pom2.p == ID)
        {
            VSpop(zasobnik);	//odstraneni < z hlavniho zasobniku
            neterm = pom2;
            neterm.p = NETERM;

            //neterm.data.nazov=id;
            TSvlozSymbol(neterm.data);
            VSpush(zasobnik, neterm);		//a nahrazeni neterminalem E
            return EOK;
        }
        //prvni nactena leva zavorka (
        else if(pom2.p == LZAVORKA)
        {
            if(!VSempty(zasobnik2))	//za zavorkou musi byt odlozeny dalsi prvek
                VStoppop(zasobnik2, &pom3); //precteme dalsi odlozeny a vyjmeme
            else
            {
                fprintf(stderr,"Chyba prebytecna zavorka\n");
                return ESYN;
            }
            //za zavorkou musi byt neterminal E
            if(pom3.p == NETERM)
            {
                if(!VSempty(zasobnik2))	//za E musi byt prava zavorka
                    VStoppop(zasobnik2, &pom4); //precteme dalsi odlozeny a vyjmeme
                else
                {
                    fprintf(stderr,"Chybi E\n");
                    return ESYN;
                }
                if((pom4.p == PZAVORKA) && (VSempty(zasobnik2)))   //nasli jsme E->(E)
                {
                    VSpop(zasobnik);	//odstranime < ze zasobniku
                    neterm = pom3;
                    neterm.p = NETERM;

                    TSvlozSymbol(neterm.data);
                    VSpush(zasobnik, neterm); //a vlozime E

                    return EOK;
                }
                else
                {
                    fprintf(stderr,"Za E->(E) jsou nadbytecne znaky\n");
                    return ESYN;
                }
            }
            else
            {
                fprintf(stderr,"Chybi E\n");
                return ESYN;
            }
        }
        //prvni nacteny neterminal E
        else if(pom2.p == NETERM)   //printf("dat typ %d\n",pom2.data.typ);
        {
            if(!VSempty(zasobnik2))	//za E musi byt operator
                VStoppop(zasobnik2, &pom3); //precteme dalsi odlozeny a vyjmeme
            else
            {
                fprintf(stderr,"Chybi perator v E op E\n");
                return ESYN;
            }
            //zjistime operator
            switch(pom3.p)   //kontrola a prirazeni instrukce
            {
            case PLUS:
                perator = I_ADD;
                ok = 1;
                break;
            case MINUS:
                perator = I_SUB;
                ok = 1;
                break;
            case KRAT:
                perator = I_MUL;
                ok = 1;
                break;
            case DELENO_CELY:
                perator = I_DIVC;
                ok = 1;
                break;
            case DELENO_DESET:
                perator = I_DIVD;
                ok = 1;
                break;
            case ROVNITKO:
                perator = I_EQUAL;
                ok = 1;
                break;
            case NEROVNITKO:
                perator = I_NEQUAL;
                ok = 1;
                break;
            case MENSIROVNO:
                perator = I_ELESS;
                ok = 1;
                break;
            case VETSIROVNO:
                perator = I_EMORE;
                ok = 1;
                break;
            case VICE:
                perator = I_MORE;
                ok = 1;
                break;
            case MENE:
                perator = I_LESS;
                ok = 1;
                break;
            default:
                fprintf(stderr,"Neplatny operator v E op E\n");
                return ESYN;
            }
            //nacitame posledni potrebny nonterminal E
            if(ok == 1)
            {
                if(!VSempty(zasobnik2))	//za E musi byt operator
                    VStoppop(zasobnik2, &pom4); //precteme dalsi odlozeny a vyjmeme
            }
            else
            {
                fprintf(stderr,"Chybi posledni operandv E op E\n");
                return ESYN;
            }
            //nasli jsme E->E op E
            if((pom4.p == NETERM) && (VSempty(zasobnik2)))   //printf("dat typ %d\n",pom4.data.typ);
            {
                VSpop(zasobnik);	//odstranime < ze zasobniku

                if((perator == I_ADD) ||	//semant. kontrola aritmetickych operaci
                        (perator == I_SUB)  ||
                        (perator == I_MUL)  ||
                        (perator == I_DIVD) ||
                        (perator == I_DIVC))
                {
                    //pokud mame int
                    if((jeInt == 1) && (jeDouble == 0))
                    {
                        neterm.data.typ = tInt;  //vysledny neterminal bude double
                        neterm.p = NETERM;
                        char *aritm = advMalloc(sizeof(char)*25);
                        sprintf(aritm,"@prom_%u",nameID++);
                        neterm.data.nazev = aritm;
                        VSpush(zasobnik, neterm); //a vlozime E
                        TSvlozSymbol(neterm.data);
                        ta_Insert(&ta, perator, TSreadSymbol(pom2.data.nazev),TSreadSymbol(pom4.data.nazev), TSreadSymbol(neterm.data.nazev));
                        jeInt = 0;
                        return EOK;
                    }
                 else
                 {
                        neterm.data.typ = tDouble;  //vysledny neterminal bude double
                        neterm.p = NETERM;
                        char *aritm = advMalloc(sizeof(char)*25);
                        sprintf(aritm,"@prom_%u",nameID++);
                        neterm.data.nazev = aritm;
                        VSpush(zasobnik, neterm); //a vlozime E
                        TSvlozSymbol(neterm.data);
                        ta_Insert(&ta, perator, TSreadSymbol(pom2.data.nazev),TSreadSymbol(pom4.data.nazev), TSreadSymbol(neterm.data.nazev));
                        jeDouble = 0;
                        jeInt = 0;
                        return EOK;
                 }
                }
                //semant. kontrola porovnani
                else if ((perator == I_EQUAL) ||
                         (perator == I_NEQUAL) ||
                         (perator == I_ELESS) ||
                         (perator == I_EMORE) ||
                         (perator == I_MORE) ||
                         (perator == I_LESS))
                {

                    neterm.data.typ = tBool;
                    neterm.p = NETERM;
                    char *por = advMalloc(sizeof(char)*25);
                    sprintf(por,"@prom_%u",nameID++);
                    neterm.data.nazev = por;
                    VSpush(zasobnik, neterm); //a vlozime E
                    TSvlozSymbol(neterm.data);
                    ta_Insert(&ta, perator, TSreadSymbol(pom2.data.nazev),TSreadSymbol(pom4.data.nazev), TSreadSymbol(neterm.data.nazev));
                    return EOK;
                }
                else
                {
                    fprintf(stderr,"Nastala chyba v ramci redukci vyrazu.\n");
                }

            }
        }
        else
        {
            fprintf(stderr,"Chyba znak odporujici pravidlu\n");
            return ESYN;
        }
    }
    else
    {
        fprintf(stderr,"Chyba < neni na zasobniku\n");
        return ESYN;
    }
    return EOK;
}

//hlavni ridici funkce precedencni analyzy postupne bere tokeny a vyhodnocuje je jako vyraz
tError pparser()
{
    int ctemedal = 1;	//po redukci nenacitame novy znak
    tError ok;			//kontrola redukce
    tError fetchok;		//kontrola fetchindex
    int counter = 0;	//osetreni zavorek u print

    tVZasobnik zasobnik;	//hlavni zasobnik
    VSinit(&zasobnik);		//inicializace
    tVZasobnik zasobnik2;	//pomocny zasobnik
    VSinit(&zasobnik2);
    //pomocne promenne pro praci se zasobnikama
    tData vrchol;	//overeni zda je na vrcholu terminal
    tData vyber;	//prelevani prvku mezi zasobniky
    tData sloupec;
    tData radek;	//levy index do precedencni tabulky
    tData konec;
    //vlozime pocatecni znak dolar na zasobnik
    tData pom;
    pom.p = KDOLAR;
    VSpush(&zasobnik,pom);

    //hlavni smycka zpracovavani vyrazu
    do
    {
        if(ctemedal == 1)
        {
            if((fetchok = fetchIndex(token,&sloupec,&counter)) == EOK)   //pomocna promenna pro ziskani praveho indexu tabulky
            {
                if(vst == Asc || vst == Chr || vst == Length || vst == SUBSTR || vst == FUNC)
                {

                    vst = NOVEST;
                    isExprWr = true;

                    error = vest();

                    if(error != EOK)
                        return error;
                    isExprWr = false;
                    //uvolneni zasobniku
                    while(!VSempty(&zasobnik))
                        VSpop(&zasobnik);
                    while(!VSempty(&zasobnik2))
                        VSpop(&zasobnik2);
                    return EOK;
                }
            }
            else
                break;
        }

        VStop(&zasobnik, &radek);
        //neterminaly preskakujeme
        while(radek.p > KDOLAR)
        {

            VStoppop(&zasobnik, &vyber);
            VSpush(&zasobnik2, vyber);
            VStop(&zasobnik,&radek);		//nez se vyskoci z cyklu ulozime prvni terminal na zasobniku
        }

        VStop(&zasobnik, &radek);
        //navraceni predchozich neterminalu
        while(! (VSempty(&zasobnik2)))
        {
            VStoppop(&zasobnik2, &vyber);
            VSpush(&zasobnik, vyber);
            VStop(&zasobnik, &vyber);
        }
        //osetreni zavorek writu
        if(isPrint)
        {
            if(counter == -1)
                sloupec.p=KDOLAR;
        }

        if(sloupec.p > KDOLAR || radek.p > KDOLAR)
        {

            fprintf(stderr,"chyba mame vetsi index do precedencni tabulky\n");
            //uvolneni zasobniku
            while(!VSempty(&zasobnik))
                VSpop(&zasobnik);
            while(!VSempty(&zasobnik2))
                VSpop(&zasobnik2);
            return ESYN;
        }

        switch(Ptabulka[radek.p][sloupec.p])
        {
        case R:		//rovnitko pouze dame token na &zasobnik
            VSpush(&zasobnik,sloupec);
            ctemedal =1;
            break;
        case M:		//mensitko dame token na zasobnik plus <
            //dokud mame na vrcholu neterm nebo mensitko odkladej je na druhy &zasobnik
            VStop(&zasobnik,&vrchol);
            ctemedal = 1;
            while(vrchol.p > KDOLAR)
            {
                VStoppop(&zasobnik, &vyber);
                VSpush(&zasobnik2, vyber);
                VStop(&zasobnik,&vrchol);
            }
            pom.p = MENSITKO;
            VSpush(&zasobnik,pom);	//vlozeni < na zasobnik
            VStop(&zasobnik,&vrchol);
            //navraceni vyjmutych neterminalu zpet
            while(! (VSempty(&zasobnik2)))
            {
                VStoppop(&zasobnik2, &vyber);
                VSpush(&zasobnik, vyber);
            }
            VSpush(&zasobnik,sloupec);	//vlozeni terminalu na zasobnik
            break;
        case V:
            //vetsitko dame token na &zasobnik a redukujeme podle pravidla
            ok = redukce(&zasobnik, &zasobnik2);
            if(ok != EOK)
            {
                if(ok == ESYN)
                {
                    fprintf(stderr,"chyba v redukcich\n");
                    //uvolneni zasobniku
                    while(!VSempty(&zasobnik))
                        VSpop(&zasobnik);
                    while(!VSempty(&zasobnik2))
                        VSpop(&zasobnik2);
                    return ESYN;
                }
                else
                {
                    fprintf(stderr,"semanticka chyba v redukcich\n");
                    //uvolneni zasobniku
                    while(!VSempty(&zasobnik))
                        VSpop(&zasobnik);
                    while(!VSempty(&zasobnik2))
                        VSpop(&zasobnik2);
                    return ESEM;
                }
            }
            ctemedal = 0;
            break;
        case E:		//pokud je v tabulce prazdno -> syn. chyba
            fprintf(stderr,"V tabulce bylo prazdno\n");
            //uvolneni zasobniku
            while(!VSempty(&zasobnik))
                VSpop(&zasobnik);
            while(!VSempty(&zasobnik2))
                VSpop(&zasobnik2);
            return ESYN;
        }

        //dokud mame na vrcholu neterm nebo mensitko odkladej je na druhy &zasobnik
        VStop(&zasobnik,&vrchol);
        while(vrchol.p > KDOLAR)
        {
            VStoppop(&zasobnik, &vyber);
            VSpush(&zasobnik2, vyber);
            VStop(&zasobnik,&vrchol);
        }
        VStop(&zasobnik, &konec);	//zjistujeme zda uz nejsme na konci &zasobniku(hledame $)
        while(!(VSempty(&zasobnik2)))
        {
            VStoppop(&zasobnik2, &vyber);
            VSpush(&zasobnik, vyber);
        }

        //nacteme prvni token
        if(ctemedal == 1)
        {
            getToken();
            if(error != EOK)
            {
                //uvolneni zasobniku
                while(!VSempty(&zasobnik))
                    VSpop(&zasobnik);
                while(!VSempty(&zasobnik2))
                    VSpop(&zasobnik2);
                return error;
            }
        }

    }
    while (!((sloupec.p == KDOLAR) && (konec.p == KDOLAR)));	//dokud nenactu EOF $ a na zasobniku neni dolar
    //uvolneni zasobniku


    while(!VSempty(&zasobnik))
        VSpop(&zasobnik);
    while(!VSempty(&zasobnik2))
        VSpop(&zasobnik2);

    //osetreni zda nenastala chyba pri prevadeni tokenu
    if(fetchok == ESYN)
    {
        fprintf(stderr,"syntakticka chyba u fetchindex\n");
        return ESYN;
    }
    if(fetchok == ESEM)
    {
        fprintf(stderr,"semanticka chyba u fetchindex\n");
        return ESEM;
    }
    return EOK;
}
