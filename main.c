/*********vypracoval: Martin Hric**************
 ****** Binárne rozhodovacie diagramy *******/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define pocet_premennych 15
#define pocet_volani_create 20

typedef struct node{
    struct node *left;
    struct node *right;
    long long dlzka;
    char *list;
}NODE;

typedef struct BDD{
    unsigned int premenne;
    unsigned int uzly;
    NODE *root;
}BDD;

NODE *create_node(char *list){ //funkcia na vytvorenie a nastavenie nodu

    NODE* newNode=NULL;
    newNode=(NODE*)malloc(sizeof(NODE));
    newNode->list=list;
    newNode->left= NULL;
    newNode->right=NULL;
    newNode->dlzka=strlen(list);
    return newNode;
}

void recursive_create(NODE *vrch,BDD *strom){ //rekurzivna funkcia na vytvorenie celeho BDD

    char *L=(char*)calloc((vrch->dlzka/2)+1,sizeof(char));
    char *R=(char*)calloc((vrch->dlzka/2)+1,sizeof(char));
    memcpy(L,vrch->list,vrch->dlzka/2);
    memcpy(R,vrch->list + vrch->dlzka/2,vrch->dlzka/2);
    vrch->left=create_node(L);
    strom->uzly++;
    vrch->right=create_node(R);
    strom->uzly++;
    vrch->dlzka=vrch->dlzka/2;
    if(vrch->dlzka==1) return;
    recursive_create(vrch->left,strom);
    recursive_create(vrch->right,strom);
}

BDD *bdd_create(char *bfunkcia){  //alokujem si miesto 1 BDD, nastavim a osetrim , volam rekurzivnu funkciu

    BDD *bdd=(BDD*)calloc(1,sizeof(BDD));
    NODE *new= create_node(bfunkcia);
    bdd->root=new;
    int size=(int) strlen(bfunkcia);
    bdd->premenne=(int) log2(size);
    if(bdd->premenne != ceil(bdd->premenne)){
        printf("zly vstup\n");
        return NULL;
    }
    bdd->uzly++;
    recursive_create(new,bdd);
    return bdd;
}

char bdd_use(BDD *bdd, char *vstupy){ //prejdenie celeho diagramu na zaklade zadaneho vstupu, vracia '1' / '0'
    NODE *temp=bdd->root;
    int i=0;
    while(vstupy[i] != '\0'){
        if(vstupy[i]== '1') {
            temp=temp->right;
        }
        else if(vstupy[i]=='0'){
            temp=temp->left;
        }
        i++;
    }
    if(temp->list[0]== '0') return '0';
    else return '1';
}

char *vstupy_use(long long bity,long long input){ //funkcia, ktora mi vypise na zadany pocet bitov cislo z desiatkovej do dvojkovej sustavy (input)
    long long out;
    long long z=0;
    char *output=malloc((bity+1)* sizeof(char));
    for(long long i=bity-1;i>=0;i--){
        out= input >> i;
        if(out&1) *(output+z)=1 +'0';
        else *(output+z)=0 +'0';
        z++;
    }
    *(output+z)= '\0';
    return output;
}

char *random_vektor(){ //funkcia na vytvorenie random vektora do funkcie bdd_create
    srand(time(NULL));
    int size= pow(2,pocet_premennych);
    char *vektor= (char*) calloc(size+1,sizeof(char));
    for(int i=0;i<size;i++){
        if((rand()%2)==0) vektor[i]='0';
        else vektor[i]='1';
    }
    vektor[size]='\0';
    return vektor;
}

void free_bdd(NODE *bdd){ //uvolnenie diagramu
    if(bdd==NULL) return;

    free_bdd(bdd->left);
    free_bdd(bdd->right);
    free(bdd->list);
    free(bdd);

}

int main(){

    float priemerCreate=0;
    float priemerUse=0;

    for(int i=0;i<pocet_volani_create;i++){

        float startCreate =(float) clock()/CLOCKS_PER_SEC;
        BDD *bdd= bdd_create(random_vektor());
        float endCreate =(float) clock()/CLOCKS_PER_SEC;
        priemerCreate += endCreate -startCreate;

        for(int j=0;j<pow(2,pocet_premennych);j++){

            float startUse =(float) clock()/CLOCKS_PER_SEC;
            char x=bdd_use(bdd, vstupy_use(pocet_premennych,j));
            float endUse =(float) clock()/CLOCKS_PER_SEC;

            priemerUse += endUse -startUse;
        }
        free_bdd(bdd->root);
    }
    {
        printf("\n******TEST %d premennych, v cykle %dkrat********\n", pocet_premennych, pocet_volani_create);
        printf("CREATE:\n");
        printf("celkovy cas create: %gs\n", priemerCreate);
        printf("priemer 1x create: %gs\n", priemerCreate / pocet_volani_create);
        printf("\nUSE:\n");
        printf("celkovy cas use pre vsetky create: %gs\n", priemerUse);
        printf("priemer vsetkych kombinacii use pre 1 create: %gs\n", priemerUse / pocet_volani_create);
        printf("\nSPOLU :%gs", priemerUse + priemerCreate);
        printf("\npocet uzlov v diagrame: %g", pow(2, pocet_premennych + 1) - 1);
        printf("\npocet vstupov a dlzka vstupneho vektora: %g\n", pow(2, pocet_premennych));
    }
    return 0;
}