#include <stdio.h>
#include "structs.h"

int main(){

	FILE *arq;
    arq = fopen("save.bin", "rb");

    SAVE_JOGO save_jogo;

	fread(&save_jogo, sizeof(save_jogo), 1, arq);

	printf("PONTOS: %d\n", save_jogo.rato.pontos);

	for(int i=0; i < LINHAS_MAPA; i++){
        for(int j=0; j < COLUNAS_MAPA; j++){
            printf("%c", save_jogo.mapa[i][j]);
        }
        printf("\n");
    }

    printf("\n\n\n");

    for(int i=0; i < LINHAS_MAPA; i++){
        for(int j=0; j < COLUNAS_MAPA; j++){
            printf("%c", save_jogo.fase[i][j]);
        }
        printf("\n");
    }

    for(int i=0; i<4; i++){
        printf("%d %d\n", save_jogo.gatos[i].posicao.x, save_jogo.gatos[i].posicao.y);
    }

    for(int i=0; i<7; i++){
        printf("PORTA: %d %d\n", save_jogo.portas[i].atual.x, save_jogo.portas[i].atual.y);
    }
}
