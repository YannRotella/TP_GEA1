#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <inttypes.h>
#include <stdlib.h>

/***************************** PARTIE 1 - SPECIFICATIONS ET LFSRs **************************/

//Structure LFSR, contient deux entiers de 64 bits, l'état courant et le polynôme de rétroaction.
typedef struct {
    uint64_t state;
    uint64_t mask;
} LFSR;

//Definition des polynômes de rétroaction
static const uint64_t MASK_A = 0x2C7646EE;
static const uint64_t MASK_B = 0x510781C7;
static const uint64_t MASK_C = 0x245F670A;

//Table de vérité de la fonction f
static const uint8_t truthTableF[128] = {0,0,1,1,1,0,0,1,1,0,1,1,1,0,1,1,0,0,1,0,1,1,0,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,0,0,0,1,1,1,1,0,1,1,0,1,0,0,0,1,1,1,1,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,1,0,0,1,0,1,0,1,0,0,1,1,0,1,0,1,1,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,0,1,0,1,0,0,1,0,1,1,1,0,0,0,0,0,1,1,1,1,1,1};

//Taps de GEA-1
static const int F_A[7] = {8, 30, 17, 9, 5, 28, 23};
static const int F_B[7] = {19, 4, 31, 30, 2, 10, 26};
static const int F_C[7] = {22, 2, 0, 29, 13, 32, 28};

//Application de la fonction f à un entier de 8 bits (7 bits seulement d'entrée pour f)
uint8_t f(uint8_t in) {return truthTableF[in];}

//Une petite fonction pour imprimer des entiers de 64 bits en binaire.
void printBinaire(uint64_t x,int nbBits){
    for (int i = nbBits-1; i >= 0; i--)
    {
        printf("%" PRIX64 , (x>>i) & 1ULL);
        if (i > 0) {printf(",");}  
    }
}

//TODO ICI 1
//Fonction de mise à jour d'un LFSR
void updateLFSR(LFSR *lfsr, int length, uint64_t input){
    uint64_t msb = (lfsr->state >> (length - 1)) & 1;
    uint64_t bit = msb ^ (uint64_t)(input & 1);
    if (bit) {lfsr->state ^= lfsr->mask;}
    //UNE LIGNE A RAJOUTER ICI Q1
}

//TODO ICI 2
//Fonction d'initialisation de GEA-1
void init(uint64_t S, LFSR *a, LFSR *b, LFSR *c) {
    a->state = 0;
    b->state = 0;
    c->state = 0;
    a->mask = MASK_A;
    b->mask = MASK_B;
    c->mask = MASK_C;
    uint64_t SB = 0; //TODO
    uint64_t SC = 0; //TODO
    for (int i = 63; i >= 0; i--)
    {
        updateLFSR(a, 31, (S >> i) & 1);
        updateLFSR(b, 32, (SB >> i) & 1);
        updateLFSR(c, 33, (SC >> i) & 1);
    }
}

//Calcule les 7 bits d'entrée de la fonction f
uint8_t inputF(uint64_t state, const int fin[7]) {
    return ((state >> fin[0]) & 1ULL)
         | (((state >> fin[1]) & 1ULL) << 1)
         | (((state >> fin[2]) & 1ULL) << 2)
         | (((state >> fin[3]) & 1ULL) << 3)
         | (((state >> fin[4]) & 1ULL) << 4)
         | (((state >> fin[5]) & 1ULL) << 5)
         | (((state >> fin[6]) & 1ULL) << 6);
}

//Calcule 64 bits de sortie de suite chiffrante //TODO
uint64_t stream(LFSR *a, LFSR *b, LFSR *c) {
    uint64_t stream = 0;
    for (int i = 0; i < 64; i++) {
        uint8_t z = 0;//TODO ICI
        if (z == 1) {
            //Rajoute un bit dans le mot de 64 bits
            stream ^= (1ULL << i);
        }
        // mise à jour
    }
    return stream;
}

// Fonction à faire impression de matrices
void printRepresentation(void) {
   //TODO
}

/*********************************** PARTIE 2 - ATTAQUE **********************************/
typedef struct {
    uint64_t output;
    uint64_t input;
} TAB;

//TODO VECTEURS ICI EN static const uint64_t

//Fonction de comparaison spécifique à la structure TAB
static int compare_TAB(const void *a, const void *b) {
    const TAB *ea = (const TAB *)a;
    const TAB *eb = (const TAB *)b;
    if (ea->output < eb->output) return -1;
    if (ea->output > eb->output) return 1;
    if (ea->input < eb->input) return -1;
    if (ea->input > eb->input) return 1;
    return 0;
}

//Fonction de recherche Dichotomique
static TAB *binary_search_TAB(TAB *table, size_t n, uint64_t target) {
    size_t left = 0;
    size_t right = n;
    while (left < right) {
        size_t mid = (left + right) / 2;
        if (table[mid].output == target) return &table[mid];
        if (table[mid].output < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return NULL;
}

//Codes de gray, pour ne pas à multiplier des matrices en C et aller plus vite
void grayCodes(int nbBits){
    for (uint64_t i = 0; i < (1 << nbBits); ++i)
    {
        uint64_t gray = i^(i>>1);
        printf("0x%08" PRIX64 "\n", gray);
    }
}

//ATTAQUE TOTALE
uint64_t attack(uint64_t z){
    uint64_t nbIterV = 1;
    uint64_t nbIterKer = 1;
    int logSize = 7;
    for (uint64_t v = 0; v < nbIterV; ++v)
    {
        uint64_t truev = 0;//modifier pour avoir le vrai vecteur associé
        uint64_t currentGray = 0;
        LFSR A,B,C;
        uint64_t S = truev;
        init(S,&A,&B,&C);
        TAB currentB;
        currentB.output = stream(&A,&B,&C);
        currentB.input = currentGray;
        //TODO Mets la première valeur dans L
        for (uint64_t i = 1; i < (1<<logSize); ++i)
        {
            uint64_t newGray = i^(i>>1);
            //TODO PAS MAL DE LIGNES
        }
        //TODO PAS MAL DE LIGNES ICI
    }
    return 0;
}

int main(void){
    LFSR A, B, C;
    uint64_t S = 1ULL;
    printf("0x%08" PRIX64 "\n", S);
    init(S,&A,&B,&C);
    printf("0x%08" PRIX64 "\n", A.state);
    printf("0x%08" PRIX64 "\n", B.state);
    printf("0x%08" PRIX64 "\n", C.state);
    uint64_t z = stream(&A,&B,&C);
    printf("0x%08" PRIX64 "\n", z);
    return 0;
}
