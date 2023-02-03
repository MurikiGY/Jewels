#include <stdio.h>

#include "utils.h"


//Teste de inicialização
void must_init(bool test, const char *description){
 
   if(test) return;
 
   printf("couldn't initialize %s\n", description);
   exit(1);
}

