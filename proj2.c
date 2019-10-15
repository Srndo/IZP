/**
 * Soubor:  proj2.c
 * Datum:   2017/11/08
 * Autor:   Simon Sestak, xsesta06@stud.fit.vutbr.cz
 * Projekt: Projekt 2 - Iteracni vypocty
 * Popis:   https://wis.fit.vutbr.cz/FIT/st/cwk.php?title=IZP:Projekt2&csid=653910&id=12214
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
enum ERROR {E_SUCCESS, E_ARGC, E_CISLO, E_INTERVAL, E_HELP, E_FUN};

double taylor_tan(double x, unsigned int n);
double cfrac_tan(double x, unsigned int n);
double f_abs(double cislo);
int arg_tan(int argc, char *argv[]);
int arg_m(int argc, char *argv[], int m, double vyska);
double f_abs(double cislo);
int err(int E);
int help();

int main(int argc, char *argv[])
{
    if(argc <= 1)
    {
        fprintf(stderr,"Pre napovedu zadaj -h (--help).\n");
        return err(E_HELP);
    }

    if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0 )
    {
        help();
        return err(E_SUCCESS);
    }

    else if(strcmp(argv[1], "--tan") == 0)
    {
        return err(arg_tan(argc, argv));
    }

    else if(strcmp(argv[1], "-m") == 0)
    {
        return err(arg_m(argc, argv, 2, 1.5));      //2 je cislo argumentu od ktorého bude funkcia ziskavať hodnoty
    }                                               // 1.5 je implicitne nastavena vyska pokial ju parametrom -c nezmenime

    else if(strcmp(argv[1], "-c") == 0)
    {
        return err(arg_m(argc, argv, 4, strtod(argv[2], NULL))); //4 je cislo argumentu od ktorého bude funkcia ziskavať hodnoty
    }

    if(argc < 4)
        return err(E_ARGC);

    return err(E_FUN);
}

/**
* Funkcia na zistenie ci vsetky argumenty splnaju podmienky pre vyratanie tangensu.
* Funkcia navracia hodnotu podla toho ako bola ukoncena.
* @param argc je pocet argumentov
* @param *argv[] je ukazovatel na pole argumentov
*/

int arg_tan(int argc, char *argv[])
{
    if(argc != 5)
        return E_ARGC;

    int N = strtod(argv[3], NULL);
    int M = strtod(argv[4], NULL);

    if(N == 0 || M == 0 || strtod(argv[2], NULL) == 0)  //strtod(argv[2], NULL) je argument uhla alfa
    {
        return E_CISLO;
    }

    if(0 < N && M < 14)                                 //podmienka na obmedzenie N a M
    {
        if(N > M)                                       //podmienka ze N nesmie byt vacsie ako M
        {
            return E_ARGC;
        }
        for(int i = N; i <= M; i++)                     //cyklus na vypis a vypocet tangensu
        {
            double knihovna = tan(strtod(argv[2], NULL));
            double taylor = taylor_tan(strtod(argv[2], NULL), i);
            double TE = knihovna - taylor;
            double zlomky =  cfrac_tan(strtod(argv[2], NULL), i);
            double CE = knihovna - zlomky;
            printf("%d %e %e %e %e %e\n",i, knihovna, taylor, f_abs(TE), zlomky, f_abs(CE));
        }
        return E_SUCCESS;
    }
    else
    {
        return E_INTERVAL;
    }
}

/**
* Funkcia na zistenie ci vsetky argumenty splnaju podmienky na vyratanie vzdialenosti od objektu a vysky objektu.
* Funkcia navracia hodnotu podla toho ako bola ukoncena.
* @param argc je pocet argumentov
* @param *argv[] je ukazovatel na pole argumentov
* @param m je cislo argumenta
* @param vyska je vyska v ktorej sa nachadza meraci pristroj
*/

int arg_m(int argc, char *argv[], int m, double vyska)
{
    if(argc != 4 && argc != 6)
        return E_ARGC;

    double alfa = strtod(argv[m], NULL);
    if(0 < alfa && alfa <= 1.4)                         //Podmienka na obmedzenie uhlu alfa
    {
        double dlzka = vyska / cfrac_tan(alfa, 10);     //10 je pocet iteraci pre presny vysledok na 10 desatinnych miesto vo funkcii cfrac_tan
        double v;
        double beta = strtod(argv[m + 1], NULL);
        if(0 < beta && beta <= 1.4)                     //Podmienka na obmedzenie uhlu beta
        {
            v = cfrac_tan(beta, 10) * dlzka;            //vyrata vysku
        }
        else
        {
            return E_INTERVAL;
        }
        printf("%.10e\n", dlzka);
        printf("%.10e\n",v + vyska);
        return E_SUCCESS;
    }
    else
    {
        return E_INTERVAL;
    }
}

/**
* Funkcia na vyratanie tangensu pomocou zretazenych zlomkov.
* Funkcia navracia vyslednu hodnotu tangensu.
* @param x je uhol s ktorym bude funkcia pocitat
* @param pocet_iteraci je pocet iteracii ktore musi funkcia vykonat pre vyratanie tangensu
*/

double cfrac_tan(double x, unsigned int pocet_iteraci)
{
    double menovatel = 1.0;
    double citatel = 0;
    double vysledok = 0;
    for(; pocet_iteraci >= 1; pocet_iteraci--)
    {
        citatel = (2 * pocet_iteraci - 1) / x;
        vysledok = menovatel / (citatel - vysledok);
    }
    return vysledok;
}

/**
* Funkcia na vyratanie tangensu pomocou taylorovho polymonu.
* Funkcia navracia vyslednu hodnotu tangensu.
* @param x je uhol s ktorym bude funkcia pocitat
* @param pocet_iteraci je pocet iteracii ktore musi funkcia vykonat pre vyratanie tangensu
*/

double taylor_tan(double x, unsigned int pocet_iteraci)
{
    static double citatel[13] = {1, 1, 2, 17, 62, 1382, 21844, 929569, 6404582, 443861162, 18888466084, 113927491862, 58870668456604};
    static double menovatel[13] = {1, 3, 15, 315, 2835, 155925, 6081075, 638512875, 10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375};
    double x_vzorec = x;
    double tan_x = 0;
    for(unsigned int i = 0; i < pocet_iteraci; ++i)
    {
        tan_x = tan_x + (x_vzorec * citatel[i] / menovatel[i]);
        x_vzorec *= x * x;
    }
    return tan_x;
}

/**
* Funkcia na ziskanie aboslutnej hodnoty cisla.
* Funkcia navracia absolutnu hodnotu cisla.
* @param cislo je cislo z ktoreho funkcia navracia absolutnu hodnotu
*/

double f_abs(double cislo)
{
    double abs_cislo = cislo;
    if (abs_cislo < 0.0)
        abs_cislo *= -1.0;
    return abs_cislo;
}

/**
* Funkcia na vypis helpu.
* Funkcia navracia ze bola ukoncena spravne.
*/

int help()
{
    printf("Vytvoril Simon Sestak (xsesta06)\n\n");
    printf("Program sluzi na jednoduche jednoduche vypocty uhlu tangens alebo vypocty vzdialenosti od objektu a vysky objektu.\n\n");
    printf("Program mozete spustat: ./proj2 --help ./proj2 --tan A N M ./proj2 [-c X] -m A [B]\n\n");
    printf("Popis argumentov:\n--help (-h)  vypise na obrazovku napovedu k programu.\n");
    printf("--tan        umozni vyratat tangens uhlu A v M iteraciach, vypise iteracie od N po M. (0 < N <= M < 14)\n");
    printf("-m           vyrata vzdialenost od meraneho bodu, A je uhol alfa (tento uhol zviera pristroj vo vyske 1,5m so spodkom meraneho objektu).\n");
    printf("                                                  0 < A <= 1.4 < PI/2.\n");
    printf("                                                  B je uhol beta (tento uhol zviera pristroj vo vyske 1,5m s vrcholom meraneho objektu).\n");
    printf("                                                  0 < B <= 1.4 < PI/2.\n");
    printf("-c           je volitenlny argument, vdaka ktoremu mozte zmenit v akej vyske X sa nachadza meraci pristroj.\n\n");
    return E_SUCCESS;
}

/**
* Funkcia na vypis errorovej hlasky.
* Funkcia navracia hodnotu podla toho ako bola ukoncena. Ukonci sa tym istym errorom ktory dostala.
* @param E je hodnota erruru
*/

int err(int E)
{
    switch(E)
    {
    case E_SUCCESS:
        return E_SUCCESS;
    case E_ARGC:
        fprintf(stderr,"Chyba v argumentoch.\n");
        return E_ARGC;

    case E_CISLO:
        fprintf(stderr,"Argument musi byt cislo.\n");
        return E_CISLO;

    case E_INTERVAL:
        fprintf(stderr,"Argument nepatri do urceneho intervalu.\n");
        return E_INTERVAL;

    case E_FUN:
        fprintf(stderr,"Chyba pri behu funkcie.\n");
        return E_FUN;
    }
    return E_FUN;
}
