#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>//biblioteca utilizada para computar o tempo de execução do programa


typedef struct Interval{//estrutura que armazena intervalos que serão processados
    double inicio; //x zero
    double fim; //x n
}intervalo;

typedef struct Stack{//estrutura de pilha que armazena intevalos
    intervalo AB;
    struct Stack *prox;
}pilha;




double modulo(double a){//função que retorna o módulo de um dado numero
    if(a<0){
        return a*-1;
    }
    return a;
}

intervalo retornaIntervalo(double a, double b){//função utilizada para obter um intervalo passando o início e o fim desse como parâmetro
    intervalo inter;
    inter.fim = b;
    inter.inicio = a;
    return inter;
}



void push(intervalo inter, pilha **X){//função que inclui items na pilha
    pilha *aux = (pilha*)malloc(sizeof(pilha));
    aux->AB = inter;
    aux->prox = *X;
    *X = aux;
}

intervalo pop(pilha **X){//função que retira items da pilha
    intervalo inter;
    pilha *aux;
    inter = (*X)->AB;
    aux = *X;
    *X = (*X)->prox;
    free(aux);
    return inter;
}


double func(double x){//função que retorna f(x), ou seja é a função que será integrada pelo algorítmo
    return 1+x;//função a                                      // para intergrar outra função, descomentar a função desejada e comentar essa 
    //return sqrt(1 - pow(x,2));//função b
    //return sqrt(1 + pow(x,4));//função c
    //return sin(pow(x,2));//função d
    //return cos(exp(-1*x));//função e
    //return cos(exp(-1*x))*x;//função f
    //return cos(exp(-1*x))*(0.005*pow(x,3)+1);//função g
}

double integra(intervalo inter, double erro){
    pilha *X = NULL;
    double ret_1, ret_2, ret_3, ponto_medio, resultado=0;
    intervalo aux;
    push(inter, &X);
    while(X!=NULL){
        aux = pop(&X);
        ret_1 = (aux.fim - aux.inicio) * func(aux.inicio);
        ponto_medio = ((aux.fim - aux.inicio)/2) + aux.inicio;
        ret_2 = (ponto_medio - aux.inicio) * func(aux.inicio);
        ret_3 = (aux.fim - ponto_medio) * func(ponto_medio);
        //printf("ret1: %lf, ret2: %lf, ret3: %lf", ret_1, ret_2, ret_3);
        if(modulo(ret_1 - ret_2 - ret_3) > erro){
            push(retornaIntervalo(aux.inicio, ponto_medio), &X);
            push(retornaIntervalo(ponto_medio, aux.fim), &X);
        }else{
            resultado += ret_1;
        }
       
    }
    return resultado;
}

int main(int argc, char *argv[]){
    if(argc != 4){//verifica o numero de argumentos informados ao executar o programa
        printf("NUMERO DE ARGUMENTOS INVALIDO");
        printf("A ordem dos argumentos eh a seguinte:\n primeiro - incio do intervalo\n segundo - fim do intervalo\n terceiro - erro\n");
        return 0;
    } 

    //trecho abaixo tem a função de computar o tempo de execução do programa
    LARGE_INTEGER frequency;
    LARGE_INTEGER start;
    LARGE_INTEGER end;
    double interval;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    //trecho acima tem a função de computar o tempo de execução do programa
    
    double inicio=atof(argv[1]);
    double fim=atof(argv[2]);
    double erro=atof(argv[3]);
    double result = integra(retornaIntervalo(inicio, fim), erro);

    //trecho abaixo tem a função de computar o tempo de execução do programa    
    QueryPerformanceCounter(&end);
    interval = (double) (end.QuadPart - start.QuadPart) / frequency.QuadPart;
    //trecho acima tem a função de computar o tempo de execução do programa

    printf("tempo de execucao: %lf\n", interval);
    printf("resultado da integral: %lf\n", result);
    return 0;
}
