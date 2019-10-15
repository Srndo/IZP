/**
 * Kostra programu pro 3. projekt IZP 2017/18
 *
 * Jednoducha shlukova analyza
 * Unweighted pair-group average
 * https://is.muni.cz/th/172767/fi_b/5739129/web/web/usrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#define NDEBUG // vypnutie
#include <assert.h>
#include <string.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

#define K_AVG 0
#define K_MIN 1
#define K_MAX 2
/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif


struct obj_t
{
    int id;
    float x;
    float y;
};

struct cluster_t
{
    int size;
    int capacity;
    struct obj_t *obj;
};

/** Funkcia inicializuje cluster.
* @param struct cluster_t *c - je ukazovatel na cluster ktory sa inicializuje
* @param int cap - je kapacita na ktoru sa inicializuje kapacita clustra
* @return nema navratovu hodnotu
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    c->capacity = cap;
    c->size = 0;
    c->obj = NULL;

    if(cap > 0)
    {
        c->obj = malloc(sizeof(struct obj_t) * cap);

        if(c->obj == NULL)
        {
            c->capacity = 0;
        }
    }
    //Ked sa alokacia nepodari malloc nahra null a podmienka sa nesplni
}

/** Funkcia vycisti a inicializuje cluster na prazdny cluster.
* @param struct cluster_t *c - je ukazovatel na cluster ktory sa ide vycistit a inicializovat
* @return nema navratovu hodnotu
*/
void clear_cluster(struct cluster_t *c)
{
    //uvolni pamet
    free(c->obj);
    //inicializuje na prazdny shluk
    init_cluster(c, 0);

}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/** Funkcia zvacsi capacitu clustra ak je to potrebne.
* @param struct cluster_t *c - je ukazovatel na struct s ktorym funkcia pracuje
* @param int new_cap - je velkost novej capacity
* @return funkcia navracia upraveny cluster
*/
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/** Funkcia prida objekt na koniec clustra.
* @param struct cluster_t *c - je ukazovatel na struct s ktorym funkcia pracuje
* @param struct obj_t obj - objekt ktory funkcia pridava
* @return nema navratovu hodnotu
*/
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if(c->capacity > c->size)
    {
        c->obj[c->size++] = obj;
    }
    else
    {
        resize_cluster(c, c->capacity + CLUSTER_CHUNK);
        c->obj[c->size++] = obj;
    }
}

void sort_cluster(struct cluster_t *c);

/** Funkcia pridava objekty do clustra, v pripade potreby cluster rozsiri.
* @param struct cluster_t *c1 - je cluster do ktoreho sa pridavaju objekty
* @param struct cluster_t *c2 - je cluster z ktoreho sa pridavaju objekty
* @return nema navratovu hodnotu
*/
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    //prida objekt do clustru, funkcia append zaroven obsahuje aj potrebne rozsirenie pamate
    for(int i = 0; i < c2->size; i++)
        append_cluster(c1, c2->obj[i]);

    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/** Funkcia odstrani cluster z pola.
* @param struct cluster_t *carr - je pole obsahujuce clustre
* @param int narr - je pocet poloziek v poli carr
* @param int idx - je index shluku na odstranenie
* @return funkcia navracia novy pocet clustorov v poli
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    clear_cluster(&carr[idx]);

    for(int i = idx; i < narr - 1; i++)
        carr[i] = carr[i + 1];

    return narr - 1;
}

/** Funkcia vyrata Euklidovsku vzdialenost medzi dvoma objektami.
* @param struct obj_t *o1 - je ukazovatel na objekt 1
* @param struct obj_t *o2 - je ukazovatel na objekt 2
* @return navracia Euklidovsku vzdialenost
*/
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    float x = o2->x - o1->x;
    float y = o2->y - o1->y;

    return sqrtf((x * x) + (y * y));
}

int premium_case = K_AVG; //implicitne nastavene pre prípad ze by nebol zadany 4 argument

/** Funkcia vyrata vzdialenost medzi dvoma clustrami.
* @param cluster_t *c1 - je ukazovatel na cluster 1
* @param cluster_t *c2 - je ukazovatel na cluster 2
* @return navracia priemernu vzdialenost dvoch clustrov
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c1 != NULL);
    assert(c1->size > 0);

    float distance_min = INT_MAX;
    float distance_max = 0;
    float distance_sum = 0;

    for(int i = 0; i < c1->size; i++)
    {
        for(int j = 0; j < c2->size; j++)
        {
            float tmp = obj_distance(&c1->obj[i], &c2->obj[j]);
            distance_sum += tmp;

            if (tmp < distance_max)
                distance_max = tmp;


            if (tmp < distance_min)
                distance_min = tmp;
        }
    }
    switch(premium_case)
    {
    case K_AVG:
        return distance_sum / (c1->size * c2->size);
    case K_MIN:
        return distance_min;
    case K_MAX:
        return distance_max;
    }

    return distance_sum / (c1->size * c2->size);
}

/** Funkcia najde dva najblizsie clustre.
* @param struct cluster_t *carr - ukazovatel na pole v ktorom su ulozene clustre
* @param int narr - je pocet clustrov v poli
* @param int *c1 - je ukazovatel na prvy cluster z dvojice
* @param int *c2 - je ukazovatel na druhy cluster z dvojice
* @return nema navratovu hodnotu
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float vzdialenost;
    float najmensia_vzdialenost = INT_MAX;

    //dva cykly prejdu vsetky shluky a vyrataju kazdemu vzdialenost
    for(int i = 0; i < narr; i++)
    {
        for(int j = 0; j < narr; j++)
        {
            if( i != j)
            {
                vzdialenost = cluster_distance(&carr[i], &carr[j]);
                if(vzdialenost < najmensia_vzdialenost)
                {
                    najmensia_vzdialenost = vzdialenost;
                    *c1 = i;
                    *c2 = j;
                }
            }
        }
    }
}

/** Pomocna funkcia na radenie objektov.
*/
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/** Funkcia zoradi objekty v clustri vzostupne.
* @param struct cluster_t *c - je ukazovatel na cluster
* @return nema navratovu hodnotu
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/** Funkcia na vypisanie shluku na stdout.
* @param struct cluster_t *c - je ukazovatel na cluster ktory sa ma vypisat
* @return nema navratovu hodnotu
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/** Funkcia nacita objekty zo suboru, pre kazdy objekt vytvori cluster a ulozi ho do pola clustrov.
* @param char *filename - je nazov suboru z ktoreho sa nacitavaju objekty
* @param struct cluster_t **arr - je ukazatel na prvu polozku pola struktury clustrov
* @return v pripade chyby do **arr ulozi hodnotu NULL
* @return navracia pocet nacitanych objektov clustra
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "ERROR: Nepodarilo sa otvorit subor %s\n", filename);
        *arr = NULL;
        return 0;
    }

    int c = 0, rows = 0, position = 0;
    const char count[] = "count=";

    while (c != '\n')
    {
        c = getc(file);
        if (position > 5)
        {
            int num = c - '0';
            if (0 <= num && num <= 9)
            {
                rows *= 10;
                rows += num;
            }
        }
        else
        {
            if (count[position] != c)
            {
                fprintf(stderr, "ERROR: Zly format suboru, pocet nebol najdeny\n");
                *arr = NULL;
                return 0;
            }
        }
        position++;
    }

    struct cluster_t *clusters = malloc(rows * sizeof(struct cluster_t));
    *arr = clusters;
    if (clusters == NULL)
    {
        *arr = NULL;
        return rows;
    }

    int row = 1, id;
    float x, y;
    char endChar;

    while (rows >= row)
    {
        int n = fscanf(file, "%i%f%f%c", &id, &x, &y, &endChar); //ak sa vsetko nacita do n sa priradi hodnota 4

        if (n == 4 || (n == 3 && rows == row))
        {
            if (endChar == '\n' || endChar == '\r' || endChar == EOF)
            {
                struct cluster_t cluster;
                init_cluster(&cluster, 1);
                cluster.size = 1;

                if (x < 0.0 || x > 1000.0 || y < 0.0 || y > 1000.0)
                {
                    fprintf(stderr, "ERROR: Suradnice objektu musia byt vacsie ako 0 a mensie ako 1000\n");
                    *arr = NULL;

                    return 0;
                }

                clusters[row - 1] = cluster;
                clusters[row - 1].obj->id = id;
                clusters[row - 1].obj->x = x;
                clusters[row - 1].obj->y = y;
                row++;
            }
            else
            {
                *arr = NULL;
                fprintf(stderr, "ERROR: Zly format suboru\n");
                break;
            }
        }
        else
        {
            *arr = NULL;
            fprintf(stderr, "ERROR: Nieje tu %d objektov!\n", rows);
            break;
        }
    }

    fclose(file);
    return rows;
}

/** Funkcia vypise pole zhlukov.
* @param struct cluster_t *carr - ukazovatel na prvu polozku pola
* @param int narr - je pocet zhlukov na vypisanie
* @return nema navratovu hodnotu
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    if(argc == 3 || argc == 2 || argc == 4)
    {
        int N = 1;
        if(argc == 3 || argc == 4)
        {
            char *ptr;
            N = strtol(argv[2], &ptr, 10);
            if(ptr != 0 && N <= 0)
            {
                fprintf(stderr,"ERROR: N musi byt vacsie od nuly");
                return EXIT_FAILURE;
            }
            if(argc == 4)
            {
                if(strcmp(argv[3], "--min") == 0)
                    premium_case = K_MIN;

                else if(strcmp(argv[3], "--max") == 0)
                    premium_case = K_MAX;

                else if(strcmp(argv[3], "--avg") == 0)
                    premium_case = K_AVG;

                else
                {
                    fprintf(stderr, "ERROR: Chyba v argumentoch");
                    return EXIT_FAILURE;
                }
            }
        }

        struct cluster_t *clusters;
        int pocet = load_clusters(argv[1], &clusters);

        if(!clusters)
            return EXIT_FAILURE;

        else if (N > pocet)
        {
            fprintf(stderr, "ERROR: Je tu menej objektov ako N");
            return EXIT_FAILURE;
        }

        while(pocet > N)
        {
            int c1;
            int c2;
            find_neighbours(clusters, pocet, &c1, &c2);

            merge_clusters(&clusters[c1], &clusters[c2]);

            pocet = remove_cluster(clusters, pocet, c2);
        }

        print_clusters(clusters, pocet);

        for(int i = 0; i < pocet; i++)
            clear_cluster(&clusters[i]);

        free(clusters);

    }
    else
    {
        fprintf(stderr, "ERROR: Zle argumenty");
        return EXIT_FAILURE;
    }
    return 0;
}
