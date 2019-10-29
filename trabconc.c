#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <windows.h>//biblioteca utilizada para computar o tempo de execução do programa

typedef struct Interval{//estrutura que armazena intervalos que serão processados
    double inicio;
    double fim; 
}intervalo;

typedef struct Stack{//estrutura de pilha que armazena intevalos
    intervalo AB;
    struct Stack *prox;
}pilha;


double erro;//variavel global que guardará o valor do erro, que será utilizada pelas threads no critério de parada do algorítmo  

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

void *integra(void *arg){//função que cada thread executará
    pilha *X = NULL;//pilha na qual os intervalos serão guardados
    double ret_1, ret_2, ret_3;//variaveis que armazenam o valor da área de cada retangulo a cada iteração, sendo ret_1 a area do retangulo com base maior e ret_2 e ret_3 as areas dos retangulos de base menore
    double ponto_medio; //variavel que guarda o ponto medio do intervalo sendo trabalhado a cada iteração
    double resultado=0;//variavel que armazena o valor da integral, é incrementada ao decorrer das iterações
    double *valor_retorno;//variavel do tipo ponteiro que servirá para retornar o resultado calculado por cada thread, e será recuperado na main pela função join
    intervalo aux = *((intervalo*)arg);//variavel que recebera um novo intervalo a cada iteração e que primeiramente recebe o intervalo recebido como argumento na criação da thread
    push(aux, &X);
    while(X != NULL){
        aux = pop(&X);
        ret_1 = (aux.fim - aux.inicio) * func(aux.inicio);//calcula a area do retangulo de base maior
        ponto_medio = ((aux.fim - aux.inicio)/2) + aux.inicio;//calcula o ponto médio do intervalo
        ret_2 = (ponto_medio - aux.inicio) * func(aux.inicio);//calcula a area de um dos retangulos de base menor
        ret_3 = (aux.fim - ponto_medio) * func(ponto_medio);//calcula a area do outro retangulo de base menor
        if(modulo(ret_1 - ret_2 - ret_3) > erro){//verifica se o modulo da diferença entre o retangulo de base maior e os retangulos de base menor é maior q o erro
            push(retornaIntervalo(aux.inicio, ponto_medio), &X);//insere um novo intervalo na pilha X
            push(retornaIntervalo(ponto_medio, aux.fim), &X);//insere um novo intervalo na pilha X
        }else{
            resultado += ret_1;
        }
    }
    valor_retorno = (double*)malloc(sizeof(double));
    *valor_retorno = resultado;
    free(arg);
    pthread_exit((void*)valor_retorno);
    return NULL;
}

int main(int argc, char *argv[]){
    if(argc != 5){//verifica o numero de argumentos informados ao executar o programa
        printf("NUMERO DE ARGUMENTOS INVALIDO\n");
        printf("A ordem dos argumentos eh a seguinte:\n primeiro - incio do intervalo\n segundo - fim do intervalo\n terceiro - erro\n quarto - numero de threads\n");
        return 0;
    }



    
    double inicio=atof(argv[1]);
    double fim=atof(argv[2]);
    erro=atof(argv[3]);
    int nthread = atoi(argv[4]);//variavel que guarda o numero de threads
    double integralResultado=0;//variavel que armazena o resultado final do algoritmo apos o processamento de todas as threads
    double intervalo_tamanho = (fim - inicio)/nthread;//armazena o tamanho de cada subintervalo que será passado a cada thread
    
    intervalo *arg_thread;//variavel que gaurda o intervalo que será passado para cada thread durante sua criação
    double *thread_retorno;//variavel que guarda o valor retornado pela thread a cada iteração
    pthread_t TID[nthread];

    //trecho abaixo tem a função de computar o tempo de execução do programa
    LARGE_INTEGER frequency;
    LARGE_INTEGER start;
    LARGE_INTEGER end;
    double interval;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    //trecho acima tem a função de computar o tempo de execução do programa

    for(int i=0; i<nthread; i++){
        arg_thread = (intervalo*)malloc(sizeof(intervalo));
        *arg_thread = retornaIntervalo(inicio + i*intervalo_tamanho, inicio + i*intervalo_tamanho + intervalo_tamanho);
        pthread_create(&TID[i], NULL, integra, (void*)arg_thread);
    }

    for(int i=0; i<nthread; i++){
        pthread_join(TID[i], (void*)&thread_retorno);
        integralResultado += *thread_retorno;
        free(thread_retorno);
    }

    //trecho abaixo tem a função de computar o tempo de execução do programa
    QueryPerformanceCounter(&end);
    interval = (double) (end.QuadPart - start.QuadPart) / frequency.QuadPart;
    //trecho acima tem a função de computar o tempo de execução do programa

    printf("tempo de execucao com %d threads: %lf\n", nthread, interval);
    printf("resultado da integral: %lf\n", integralResultado);
    return 1;
}