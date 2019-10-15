/**
 * Soubor:  proj1.c
 * Datum:   2017/11/01
 * Autor:   Simon Sestak, xsesta06@stud.fit.vutbr.cz
 * Projekt: Projekt 1 - Prace s textem
 * Popis:   https://wis.fit.vutbr.cz/FIT/st/cwk.php?title=IZP:Projekt1&csid=653910&id=12214
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //pre funkciu strlen();
#include <ctype.h>  //pre funkciu to upper();

#define ROWS 42     //zadefinovanie konstanty pre riadky
#define COLS 100     //zadefinovanie konstanty pre znaky

/**
* Funkcia na zoradenie vystupu abecedne.
* Nema navratovu hodnotu.
* @param n je pocet vystupov
* @param vystup[n] je pole ktoremu sa predaju pismena na vystup.
*/
void uprava_vystupu(int n, char vystup[n])
{
    for (int i = 0; i < n; ++i)                 //cyklus pre zoradenie pismen abecedne
    {
        for (int j = i + 1; j < n; ++j)
        {
            if (vystup[i] > vystup[j])          //ak pismeno na indece i je vacsie ako pismeno ktore sa nachadza na indexe j (i+1)
            {
                char pomocna =  vystup[i];      //nasledne 3 riadky swapnut poziciu tych dvoch pismen
                vystup[i] = vystup[j];
                vystup[j] = pomocna;
            }
        }
    }

    for(int i = 0; i < n; i++)                  //cyklus pre vymazanie opakujucich sa vystupnych pismen
    {
        int x = i + 1;
        while((vystup[i] == vystup[x]) || x < n)         //cyklus sa vykonvava pokial dve pismena su rovnake alebo x < n
        {
            if(vystup[i]== vystup[x])               //ak sa tie dve pismena rovnaju
            {
                vystup[x] = '.';                    //druhe v poradi prepise bodkou
            }
            x++;                                    //zvysi pismeno pre porovnanie
        }
    }

    printf("Enable: ");
    for(int x = 0; x < n; x++)          //cyklus ktori vypise pismena abecedne zoradene a bodky preskoci
    {
        if(vystup[x] != '.')
            printf("%c",vystup[x]);
    }
}

/**
* Funkcia na na zistenie dlzky vstupu.
* Navracia vypocitanu dlzku vstupu.
* @param *vstup string so vstupnym argumentom programu.
*/
int dlzka_vstupu(char *vstup)
{
    int dlzka_vstupu = strlen(vstup);       //zisti dlzku vstupu
    return dlzka_vstupu;                    //navrati dlzku vstupu
}

/**
* Funkcia na zvacsenie vsetkych pismen na vstupe.
* Navracia pole s upraveným výstupom.
* @param *vstup string so vstupnym argumentom programu.
* @param dlzka_vstupu dlzka vstupu vyratana predchadzajucou funkciou.
* @param upraveny_vstup[COLS] pole v ktorom je uz zapisany vstup velkymi pismenami.
*/
char uprava_vstupu(char *vstup, int dlzka_vstupu, char upraveny_vstup[COLS])
{
    for(int i = 0; i < dlzka_vstupu; i++)       //bude vykonavat kym nezmeni cely vstup
        upraveny_vstup[i] = toupper(vstup[i]);  //do pola bude zapisovat velke pismena pre case insensitive
    return upraveny_vstup[dlzka_vstupu];        //navrati upravene pole s velkymi pismenami
}

/**
* Funkcia na ulozenie adriez z databazy do pola programu. Do pola zapisuje kazde pismeno ako velke.
* Navracia 1 alebo 0 podla toho ci vsetko vo funkcii prebehlo spravne.
* @param adresy[ROWS][COLS] pole do ktore sa ukladaju jednotlive mesta(adresy).
*/
int uloz(char adresy[ROWS][COLS])
{
    int i = 0, y = 0,c;                 //i = pocitadlo riadku, y = pocitadlo znaku, c = nacitanie znaku (int kvoli EOF)
    while(((c = getchar()) != EOF))     //cyklus sa bude vykonavat pokial nenarazí na koniec suboru
    {
        adresy[i][y] = toupper(c);      // do pola adresy zapise znak velkym pismenom
        if(adresy[i][y] == '\n')          // ak sa do pola zapise medzera
        {
            i++;                        //nastavi dalsi riadok
            if(i > ROWS || y > COLS)    //zachytenie buffer overflow
                return 0;
            y = 0;                      //nastavi znak na dalsom riadku na 0
        }
        else
            y++;                        //ak sa podmienka nevykona zvysi priestor pre dalsi znak
    }
    adresy[i][y] = '\0';                //za posledny znak v poli napise nulu
    return 1;
}

/**
* Funkcia na porovnanie vstupov s adresami.
* Nema navratovu hodnotu.
* @param dlzka_vstupu dlzka vstupu vyratana predchadzajucou funkciou.
* @param vystup[] pole pre zapisanie vystupu. (vytvorene pre pripad buducej potreby v main)
* @param vstup[] pole s upraveným vstupom.
* @param adresy[ROWS][COLS] pole adries z databazy s upravenym vstupom.
*/
void porovnaj(int dlzka_vstupu, char vystup[], char vstup[], char adresy[ROWS][COLS])
{
    int cyklus = 0;
    int p_znak = 0, p_vystupu = 0;
    int mesto_riadok;
    int argument = 0;

    for(int p_riadok = 0; adresy[p_riadok][p_znak] != '\0'; )
    {
        if(vstup[argument] != adresy[p_riadok][p_znak])         //ak sa vstup nerovna znaku preskoci riadok
            p_riadok++;

        if(vstup[argument] == adresy[p_riadok][p_znak])
        {
            int p = 0;          //pocitadlo
            while( (vstup[argument] == adresy[p_riadok][p_znak]))       //bude vykonávat cyklus porovnavanie argumentov s dalsim znakom pokial sa budu rovnat
            {
                argument++;
                p_znak++;
                p++;
            }
            if(p != dlzka_vstupu)       // ak sa nezhoduju vsetky argumenty preskoci riadok
            {
                argument = 0;           //argument nacita na 0 aby sa dalsie slovo kontrolovalo od 1. argumentu
                p_znak = 0;             //znak nacita na 0 aby sa dalsie slovo kontrolovalo od 1. znaku
                p_riadok++;             //preskoci riadok
            }
            else            //ak sa vsetky argumenty rovnali znakom
            {
                vystup[p_vystupu] = adresy[p_riadok][p_znak];           //zapise dalsi znak do vystupu
                p_vystupu++;                                            //vytvori miesto pre dalsi vystup
                argument = 0;                                   //vid 135. riadok
                p_znak = 0;                                     //vid 136. riadok
                cyklus++;                                       //pocitadlo pre to kolko krat sa vykonal cyklus - podla toho sa zistuje spravny vystup
                mesto_riadok = p_riadok;                        //do premenej mesto_riadok zapise na ktorom riadku sa nasla zhoda
                p_riadok++;                                     //riadok zvacsi o 1 a tak sa zacne porovnavanie dalsieho mesta
            }
        }
    }

    if(cyklus == 1)             //ak sa cyklus vykona iba raz to znamena ze v databaze je iba jedno mesto
    {
        printf("Found: ");
        for(int p_znak = 0; p_znak < COLS && (adresy[mesto_riadok][p_znak] != '\0' && adresy[mesto_riadok][p_znak] != '\n'); p_znak++)
            printf("%c",adresy[mesto_riadok][p_znak]);
    }

    else if(cyklus > 1)         //ak sa cyklus vykona viac krat vypsise mozne nasledujuce pismena
    {
        uprava_vystupu(p_vystupu, vystup);      //zavola sa funkcia ktora zoradi vystup abecedne, do tejto funkcie odosielam pocet prvkov vo vystupe a vystupne znaky
    }

    else if(cyklus == 0)        //ak sa cyklus nevykona tak mesto nieje v databaze
        printf("Not found.");
}

/**
* Funckia na rozvetvenie programu podla toho kolko argumentov bolo zadanych.
* Navracia 1 alebo 0 podla poctu argumentov.
* @param argc pocet vstupnych argumentov.
* @param adresy[ROWS][COLS] pole adries z databazy s upravenym vstupom.
*/
int argumenty(int argc, char adresy[ROWS][COLS])
{
    if(argc == 0)           //ak sa argument rovna 0 vypisu sa vsetky prve pismena miest
    {
        char adresy_pole[ROWS];
        for(int i = 0; i < ROWS; i++)
        {
            adresy_pole[i] = adresy[i][0];
        }
        uprava_vystupu(ROWS, adresy_pole);
        printf("\n");
        return 0;           //navrati 0 a ukonci program
    }
    else if(argc > 1)       //ak je argument vacsi ako 1 program ukonci a vypise chybu
    {
        fprintf(stderr,"Zadaj iba jeden argument.\n");
        return 0;           //navrati 0 a ukonci program
    }
    else
        return 1;
}

int main(int argc, char *argv[])
{
    argc = argc - 1;            //pocet argumentov - nazov programu (0ltny argument)
    char adresy[ROWS][COLS];
    char upraveny_vstup[COLS];
    char vystup[ROWS];

    if(uloz(adresy) == 0)               //zavolanie premennej na naplnenie pola adresy z databazy
    {
        fprintf(stderr,"Velka databaza, obmedz databazu na %i.\n",ROWS);
        fprintf(stderr,"Mesto v databaze moze obsahovat maximalne %i znakov.\n",COLS);
        return EXIT_FAILURE;
    }

    if(argumenty(argc,adresy) == 0)     //zavolanie funkcie na zistenie kolko argumentov bolo zadanych a nasledne bud ukoncenie alebo pokracovanie programu
        return EXIT_FAILURE;
    char *vstup1 = argv[1];             //zapis vstupneho argmenta do stringu

    int dlzka = dlzka_vstupu(vstup1);   //zistenie kolko pismen obsahuje vstup

    uprava_vstupu(vstup1, dlzka, upraveny_vstup);       //nastavenie pismen pre case insensitive

    porovnaj(dlzka_vstupu(vstup1), vystup, upraveny_vstup, adresy);     //porovná vstupne parametre s databazou a vypíse vysledok programu

    printf("\n");
    return 0;

}
