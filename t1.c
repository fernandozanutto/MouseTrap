#Alecsander Hennig e Fernando Zanutto
#302492 e 302340

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <locale.h>
#include <pthread.h>
#include <windowsx.h>
#include <mmsystem.h>
#include "structs.h"

#define NUM_GATOS 4
#define NUM_PORTAS 7
#define ALTURA_INICIAL_JOGO 5
#define COLUNAS_MAPA 27
#define LINHAS_MAPA 11

#define LARGURA_BLOCO 5
#define ALTURA_BLOCO 5

#define BLACK         0
#define DARKBLUE      1
#define GREEN         2
#define CYAN          3
#define RED           4
#define MAGENTA       5
#define BROWN         6
#define LIGHTGRAY     7
#define DARKGRAY      8
#define BLUE          9
#define LIGHTGREEN    10
#define LIGHTBLUE     11
#define LIGHTRED      12
#define LIGHTMAGENTA  13
#define YELLOW        14
#define WHITE         15

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

int mover_porta = 0;


void *thread_play_sound(void *arg){

	char *caminho = (char *) arg;

	PlaySound(caminho, NULL, SND_SYNC | SND_LOOP | SND_FILENAME);
}

void tocar_som(char caminho[]){
	pthread_t thread;

	pthread_create(&thread, NULL, thread_play_sound, caminho);

}


/**
*retorna se o valor está entre outros dois valores
*/
int between(int value, int lower, int higher){
    return value > lower && value < higher;
}

//Define uma nova direcao para um gato (0 ou 1 em X ou 0 ou 1 em Y)
void setDirecao(GATO *gato) {		
    int dirX1, dirX2, dirY1, dirY2;

	if(rand() % 2 == 0){
		gato->direcao.mov_y = 0;
		do {
			dirX1 = rand() % 2;
			dirX2 = rand() % 2;
		} while (dirX1 - dirX2 == 0);
		gato->direcao.mov_x = (dirX1 - dirX2);
	} else {
		gato->direcao.mov_x = 0;
		do {
			dirY1 = rand() % 2;
			dirY2 = rand() % 2;
		} while (dirY1 - dirY2 == 0);
		gato->direcao.mov_y = (dirY1-dirY2);
	}
}

// Move um gato. Verifica se a proxima posicao do gato é possivel (nao tem paredes ou gatos no caminho nem vai pra fora dos limites do mapa) e, se for possivel, atualiza a 
// posicao do gato. Se nao for possivel, lhe dá uma nova direção.
int mover_gato(GATO *gato, char (*mapa)[LINHAS_MAPA][COLUNAS_MAPA], char (*novo_mapa)[LINHAS_MAPA][COLUNAS_MAPA], char fase[LINHAS_MAPA][COLUNAS_MAPA]) {
    if (between(gato->posicao.x + gato->direcao.mov_x, 0, COLUNAS_MAPA) && between(gato->posicao.y + gato->direcao.mov_y, 0, LINHAS_MAPA)) {
        if((*mapa)[gato->posicao.y + gato->direcao.mov_y][gato->posicao.x + gato->direcao.mov_x]  != 'X' && (*mapa)[gato->posicao.y + gato->direcao.mov_y][gato->posicao.x + gato->direcao.mov_x]  !=
'T' && (*mapa)[gato->posicao.y + gato->direcao.mov_y][gato->posicao.x + gato->direcao.mov_x]  != 'G') {
            gato->posicao_antiga.x = gato->posicao.x;
            gato->posicao_antiga.y = gato->posicao.y;
            (*novo_mapa)[gato->posicao.y][gato->posicao.x] = fase[gato->posicao.y][gato->posicao.x];
            gato->posicao.x += gato->direcao.mov_x;
            gato->posicao.y += gato->direcao.mov_y;
            (*novo_mapa)[gato->posicao.y][gato->posicao.x] = 'G';
            //setDirecao(gato);      //Essa linha faz que o movimento dos gatos randomizado a cada movimento que fizerem;
        }
        else{
            setDirecao(gato);
        }
    }
    else {
        setDirecao(gato);
        return 0;
    }

    return 1;
}

// serve para iniciar todos os gatos e colocá-los dentro de um vetor
int iniciar_gatos(GATO gatos[], char (*mapa)[LINHAS_MAPA][COLUNAS_MAPA]) {

    int cont_gatos = 0;

    for (int c = 0; c < COLUNAS_MAPA; c++) {
        for(int l = 0; l < LINHAS_MAPA; l++) {
            if ((*mapa)[l][c] == 'G') {
                GATO tempGato;
                tempGato.posicao.x = c;
                tempGato.posicao.y = l;
				tempGato.posicao_inicial.x = c;
                tempGato.posicao_inicial.y = l;
                setDirecao(&tempGato);

                gatos[cont_gatos] = tempGato;
                cont_gatos++;
            }
        }
    }
    return 0;
}

// desenha o cabeçalho
void desenhar_cabecalho(RATO rato) {
   textbackground(BLACK);
   textcolor(WHITE);
   gotoxy(1, 2);
   cprintf("PONTOS: %d VIDAS: %d       ", rato.pontos, rato.vidas);
}

// desenha o rodapé
void desenhar_rodape(int jogo_pausado){

    textbackground(BLACK);
    textcolor(WHITE);

    gotoxy(10, LINHAS_MAPA * ALTURA_BLOCO + ALTURA_INICIAL_JOGO + 3);
    if(jogo_pausado){
        cprintf("JOGO PAUSADO!             ");
		gotoxy(10, LINHAS_MAPA*ALTURA_BLOCO + ALTURA_INICIAL_JOGO + 5);
		cprintf("[N] NOVO JOGO  [C] CARREGAR JOGO  [S] SALVAR JOGO  [Q] SAIR DO JOGO  [V] VOLTAR     ");
    } else {
        cprintf("                          ");
		gotoxy(10, LINHAS_MAPA*ALTURA_BLOCO + ALTURA_INICIAL_JOGO + 5);
		cprintf("[W] CIMA  [A] ESQUERDA  [S] BAIXO  [D] DIREITA  [TAB] PAUSAR  [B] MOVER PORTAS     ");
    }

    
}

// desenha as sprites ( 5 x 5 ) dos objetos
void desenhar_sprite(SPRITE sprite, int c, int l){

	textbackground(sprite.backgroundcolor);
	textcolor(sprite.textcolor);

	for(int i=1; i<=ALTURA_BLOCO; i++){
        gotoxy((c*LARGURA_BLOCO)+1, (l*ALTURA_BLOCO) + ALTURA_INICIAL_JOGO + i);
        cprintf(sprite.linha[i-1]);
	}
}

// le o mapa e prepara a fase que serve de guia para o consumo de queijos
void preparar_fase(char mapa[LINHAS_MAPA][COLUNAS_MAPA], char fase[LINHAS_MAPA][COLUNAS_MAPA], RATO *rato, int *queijos_faltando) {

	int cont_queijos = 0;

    for (int c = 0; c < COLUNAS_MAPA; c++) {
        for(int l = 0; l < LINHAS_MAPA; l++) {
            //M - rato
            //X - parede
            //O - osso
            //Q - queijo
            //T - porta
            //G - gato

			if(mapa[l][c] == 'Q'){
				cont_queijos++;
			}

            if(mapa[l][c] == 'M') {
                fase[l][c] = ' ';
                rato->posicao.x = c;
                rato->posicao.y = l;
                rato->posicao_inicial.x = c;
                rato->posicao_inicial.y = l;
            } else if(mapa[l][c] == 'G') {
                fase[l][c] = 'Q';
				cont_queijos++;
            }
            else if(mapa[l][c] == 'T'){
                fase[l][c] = ' ';
            }
            else {
                fase[l][c] = mapa[l][c];
            }
        }
    }
	*queijos_faltando = cont_queijos;
}

//desenha o mapa (quando uma fase é carregada)
void desenhar_mapa(char mapa[LINHAS_MAPA][COLUNAS_MAPA], RATO rato) {

	SPRITE sprite;

    for (int c = 0; c < COLUNAS_MAPA; c++) {
        for(int l = 0; l < LINHAS_MAPA; l++) {

            char atual = mapa[l][c];
            /*
            M - rato
            X - parede
            O - osso
            Q - queijo
            T - porta
            G - gato
            */
            switch(atual) {
            case 'X':
                sprite = sprite_parede;
                break;
            case 'O':
                sprite = sprite_osso;
                break;
            case 'M':
                sprite = rato.sprite;
                break;
            case 'Q':
                sprite = sprite_queijo;
                break;
            case 'T':
                sprite = sprite_porta;
                break;
            case 'G':
                sprite = sprite_gato;
                break;
            default:
                sprite = sprite_fundo;
            }
			desenhar_sprite(sprite, c, l);
        }
    }
}

//atualiza a tela comparando o vetor mapa com as possíveis alteraçoes que estao em novo_mapa;
void atualizar_mapa(char (*mapa)[LINHAS_MAPA][COLUNAS_MAPA], char (*novo_mapa)[LINHAS_MAPA][COLUNAS_MAPA], RATO rato) {

	SPRITE sprite;
    for (int c = 0; c < COLUNAS_MAPA; c++) {
        for(int l = 0; l < LINHAS_MAPA; l++) {
            if ((*mapa)[l][c] != (*novo_mapa)[l][c]) {
                char atual = (*novo_mapa)[l][c];
                (*mapa)[l][c] = (*novo_mapa)[l][c];
                /*
                M - rato
                X - parede
                O - osso
                Q - queijo
                T - porta
                G - gato
                */
				switch(atual) {
                case 'X':
					sprite = sprite_parede;
					break;
				case 'O':
					sprite = sprite_osso;
					break;
				case 'M':
					sprite = rato.sprite;
					break;
				case 'Q':
					sprite = sprite_queijo;
					break;
				case 'T':
					sprite = sprite_porta;
					break;
				case 'G':
					sprite = sprite_gato;
					break;
				default:
					sprite = sprite_fundo;
                }

				desenhar_sprite(sprite, c, l);
            }
			else if((*mapa)[l][c] == ' '){

			}
        }
    }
}

//inicia as portas e as coloca no array de portas
void iniciar_portas(PORTA portas[NUM_PORTAS], char (*mapa)[LINHAS_MAPA][COLUNAS_MAPA]) {
    int cont_portas = 0;
    for (int c = 0; c < COLUNAS_MAPA; c++) {
        for(int l = 0; l < LINHAS_MAPA; l++) {
            if ((*mapa)[l][c] == 'T') {
                PORTA tempPorta;
                tempPorta.atual.x = c;
                tempPorta.atual.y = l;
                tempPorta.possivel.x = c+1;
                tempPorta.possivel.y = l+1;
				tempPorta.sprite = sprite_porta;

                portas[cont_portas] = tempPorta;
                cont_portas++;
            }
        }
    }
}

// verifica se as portas estao bloqueadas e move as que estao com a movimentacao livre
void mover_portas(PORTA *porta, char (*fase)[LINHAS_MAPA][COLUNAS_MAPA], char(*mapa)[LINHAS_MAPA][COLUNAS_MAPA], char (*novo_mapa)[LINHAS_MAPA][COLUNAS_MAPA]){
    for (int i = 0; i < NUM_PORTAS; i++){
		if((*novo_mapa)[porta->possivel.y][porta->possivel.x]  != 'G' && (*novo_mapa)[porta->possivel.y][porta->possivel.x]  != 'M'){
			POSICAO temp = porta->atual;
			porta->atual = porta->possivel;
			porta->possivel = temp;
			(*novo_mapa)[porta->possivel.y][porta->possivel.x] = fase[porta->possivel.y][porta->possivel.x];
			(*novo_mapa)[porta->atual.y][porta->atual.x] = 'T';
        }
        porta = porta + 1;
    }
}

/**
*retorna 1 se deve atualizar cabecalho
*/
//Move o rato de acordo com a direcao dada pelo teclado para ele, caso nao existam condiçoes que nao o permitam, como paredes. 
//Essa funcao tambem verifica se o rato comeu um osso ou queijo e se passou por cima de um gato que está derrotado.
int mover_rato(RATO *rato, GATO gatos[], char (*fase)[LINHAS_MAPA][COLUNAS_MAPA], char(*mapa)[LINHAS_MAPA][COLUNAS_MAPA], char (*novo_mapa)[LINHAS_MAPA][COLUNAS_MAPA], int *queijos_faltando){

	if (between(rato->posicao.x + rato->direcao.mov_x, 0, COLUNAS_MAPA) && between(rato->posicao.y + rato->direcao.mov_y, 0, LINHAS_MAPA)) {

        char nova_pos = (*mapa)[rato->posicao.y + rato->direcao.mov_y][rato->posicao.x + rato->direcao.mov_x];

        if(nova_pos  != 'X' && nova_pos  != 'T') {
            rato->posicao_antiga.x = rato->posicao.x;
            rato->posicao_antiga.y = rato->posicao.y;
			(*novo_mapa)[rato->posicao.y][rato->posicao.x] = (*fase)[rato->posicao.y][rato->posicao.x];
			for(int i = 0; i < NUM_GATOS; i++){
				if(gatos[i].vivo == 0){
					if(gatos[i].posicao_inicial.x == rato->posicao.x){
							if(gatos[i].posicao_inicial.y == rato->posicao.y){
								(*novo_mapa)[rato->posicao.y][rato->posicao.x] = 'G';
						}
					}
				}
			}
			rato->posicao.x += rato->direcao.mov_x;
			rato->posicao.y += rato->direcao.mov_y;

			(*novo_mapa)[rato->posicao.y][rato->posicao.x] = 'M';
			if((*fase)[rato->posicao.y][rato->posicao.x] == 'Q'){
				rato->pontos+=10;
				*queijos_faltando = *queijos_faltando - 1;
				(*fase)[rato->posicao.y][rato->posicao.x] = ' ';

				return 1;
			} else if ((*fase)[rato->posicao.y][rato->posicao.x] == 'O'){
                rato->sprite = sprite_cachorro;
                rato->cachorro = 1;
				rato->cont_cachorro = 0;
                (*fase)[rato->posicao.y][rato->posicao.x] = ' ';
				tocar_som("dog_bark.wav");
            }
        }
    }

    return 0;
}

//carrega o arquivo do mapa e salva no mapa e novo_mapa
void carregar_mapa(FILE *arquivo, char (*mapa)[LINHAS_MAPA][COLUNAS_MAPA], char (*novo_mapa)[LINHAS_MAPA][COLUNAS_MAPA]){

    char ch;
    int linha = 0, coluna = 0;

    ch = getc(arquivo);

    while(ch != EOF){

        if(ch == '\n'){
            linha++;
            coluna = 0;
        }
        else{
            (*mapa)[linha][coluna] = ch;
			(*novo_mapa)[linha][coluna] = ch;
            coluna++;
        }
        ch = getc(arquivo);
    }
}

//serve para colocar os gatos nas suas posicoes iniciais quando o rato eh capturado
void reiniciar_gatos(GATO gatos[], char (*novo_mapa)[LINHAS_MAPA][COLUNAS_MAPA]){
    for (int i = 0; i < NUM_GATOS; i++){
        (*novo_mapa)[gatos[i].posicao.y][gatos[i].posicao.x] = ' ';
        gatos[i].posicao.x = gatos[i].posicao_inicial.x;
        gatos[i].posicao.y = gatos[i].posicao_inicial.y;
        (*novo_mapa)[gatos[i].posicao.y][gatos[i].posicao.x] = 'G';
    }
}

//verifica a colisao entre ratos e gatos
int checar_colisao(RATO *rato, GATO gatos[], int n_gatos, char (*novo_mapa)[LINHAS_MAPA][COLUNAS_MAPA]){
    int bateu = 0;
    for(int i = 0; i < n_gatos; i++) {
		if(rato->posicao.x == gatos[i].posicao.x && rato->posicao.y == gatos[i].posicao.y) {
            bateu = 1;
        }
		else if(rato->posicao.x == gatos[i].posicao_antiga.x && rato->posicao.y == gatos[i].posicao_antiga.y && rato->posicao_antiga.x == gatos[i].posicao.x && rato->posicao_antiga.y == gatos[i].posicao.y) {
            bateu = 1;
        }
        if(bateu) {
            if(gatos[i].vivo) {
                if(rato->cachorro) {
                    bateu = 0;
                    gatos[i].vivo = 0;
                    gatos[i].cont_vivo = 0;
					(*novo_mapa)[gatos[i].posicao.y][gatos[i].posicao.x] = ' ';
					(*novo_mapa)[rato->posicao.y][rato->posicao.x] = 'M';
                    (*novo_mapa)[gatos[i].posicao_inicial.y][gatos[i].posicao_inicial.x] = 'G';
                    gatos[i].posicao.x = gatos[i].posicao_inicial.x;
                    gatos[i].posicao.y = gatos[i].posicao_inicial.y;
					rato->pontos+=50;
					tocar_som("cat.wav");
					desenhar_cabecalho(*rato);
                } else {
                    (*novo_mapa)[rato->posicao.y][rato->posicao.x] = ' ';
                    rato->posicao.x = rato->posicao_inicial.x;
                    rato->posicao.y = rato->posicao_inicial.y;
					rato->direcao.mov_x = 0;
					rato->direcao.mov_y = 0;
                    (*novo_mapa)[rato->posicao.y][rato->posicao.x] = 'M';
                    reiniciar_gatos(gatos, novo_mapa);
                }
            }
            else{
                bateu = 0;
            }
        }
    }

    return bateu;
}

//carrega um jogo salvo dum arquivo binario
int carregar_jogo(ESTADO_JOGO jogo){
	
	textbackground(BLACK);
	textcolor(WHITE);
	clrscr();

	FILE *arq;
    if(!(arq = fopen("save.bin", "rb"))){
		return 0;
	}

    SAVE_JOGO save_jogo;

	fread(&save_jogo, sizeof(save_jogo), 1, arq);

	*(jogo.rato) = save_jogo.rato;
	jogo.nivel = save_jogo.nivel;

	memcpy(jogo.gatos, save_jogo.gatos, sizeof(GATO)*NUM_GATOS);
	memcpy(jogo.portas, save_jogo.portas, sizeof(PORTA)*NUM_PORTAS);

	memcpy(jogo.fase, save_jogo.fase, sizeof(char)*LINHAS_MAPA*COLUNAS_MAPA);
	memcpy(jogo.mapa, save_jogo.mapa, sizeof(char)*LINHAS_MAPA*COLUNAS_MAPA);
	memcpy(jogo.novo_mapa, save_jogo.mapa, sizeof(char)*LINHAS_MAPA*COLUNAS_MAPA);


	fclose(arq);
	
	return 1;
}

// salva um jogo num arquivo binario
void salvar_jogo(ESTADO_JOGO jogo){

    FILE *arq;
    arq = fopen("save.bin", "wb");

    SAVE_JOGO save_jogo;

	save_jogo.nivel = jogo.nivel;
	save_jogo.rato = *(jogo.rato);

	char (*temp)[LINHAS_MAPA][COLUNAS_MAPA] = jogo.fase;
	char (*temp2)[LINHAS_MAPA][COLUNAS_MAPA] = jogo.mapa;

	memcpy(save_jogo.fase, temp, sizeof(char)*LINHAS_MAPA*COLUNAS_MAPA);
	memcpy(save_jogo.mapa, temp2, sizeof(char)*LINHAS_MAPA*COLUNAS_MAPA);

	memcpy(save_jogo.gatos, jogo.gatos, sizeof(GATO)*NUM_GATOS);
	memcpy(save_jogo.portas, jogo.portas, sizeof(PORTA)*NUM_PORTAS);

	fwrite(&save_jogo, sizeof(save_jogo), 1, arq);

	fclose(arq);
}

//reinicia o jogo
int novo_jogo(ESTADO_JOGO *estado_jogo){
	
	textbackground(BLACK);
	textcolor(WHITE);
	clrscr();

	estado_jogo->nivel = 1;
	estado_jogo->rato->pontos = 0;
	estado_jogo->rato->vidas = 3;
	estado_jogo->rato->sprite = sprite_rato;
	estado_jogo->rato->cachorro = 0;
	estado_jogo->rato->cont_cachorro = 0;
	estado_jogo->rato->direcao.mov_x = 0;
	estado_jogo->rato->direcao.mov_y = 0;

	FILE *arq;
	char nome_arquivo[100];
	sprintf(nome_arquivo, "nivel%02d.txt", estado_jogo->nivel);
    if((arq = fopen(nome_arquivo, "r")) == NULL){
		return 0;
	}

	carregar_mapa(arq, estado_jogo->mapa, estado_jogo->novo_mapa);

    preparar_fase(estado_jogo->mapa, estado_jogo->fase, estado_jogo->rato, &estado_jogo->queijos_faltando);

    iniciar_portas(estado_jogo->portas, estado_jogo->mapa);
    iniciar_gatos(estado_jogo->gatos, estado_jogo->mapa);

	return 1;
}

//avança para a proxima fase 
int proxima_fase(ESTADO_JOGO *estado_jogo){

	estado_jogo->nivel = estado_jogo->nivel + 1;
	estado_jogo->rato->sprite = sprite_rato;
	estado_jogo->rato->cachorro = 0;
	estado_jogo->rato->cont_cachorro = 0;
	estado_jogo->rato->direcao.mov_x = 0;
	estado_jogo->rato->direcao.mov_y = 0;

	FILE *arq;
	char nome_arquivo[100];
	puts(nome_arquivo);
	sprintf(nome_arquivo, "nivel%02d.txt", estado_jogo->nivel);

    if((arq = fopen(nome_arquivo, "r")) == NULL){
		return 0;
	}

	carregar_mapa(arq, estado_jogo->mapa, estado_jogo->novo_mapa);

    preparar_fase(estado_jogo->mapa, estado_jogo->fase, estado_jogo->rato, &estado_jogo->queijos_faltando);

    iniciar_portas(estado_jogo->portas, estado_jogo->mapa);
    iniciar_gatos(estado_jogo->gatos, estado_jogo->mapa);

	return 1;
}


//uma thread que serve apenas para ler o teclado e executar os possiveis comandos sem travar o resto do programa (e tambem possibilitar a mudanca de direcao do rato
//durante o sleep do main)
void *thread_ler_teclado(void *arg){

	ESTADO_JOGO *jogo = arg;

    while(1){

        char a = _getch();

        if(a == 9){ //9 = TAB
            desenhar_rodape(!jogo->jogo_pausado);
			jogo->jogo_pausado = !jogo->jogo_pausado;
        }

        if (jogo->jogo_pausado == 0){
            int x = jogo->rato->direcao.mov_x, y = jogo->rato->direcao.mov_y;

            if(a == 224){
                a = _getch();
            } else {
                a = toupper(a);
            }

            if(a == 'S' || a == KEY_DOWN) {
                y = 1;
                x = 0;
            } else if (a == 'W' || a == KEY_UP) {
                y = -1;
                x = 0;
            } else if(a == 'D' || a == KEY_RIGHT) {
                x = 1;
                y = 0;
            } else if(a == 'A' || a == KEY_LEFT) {
                x = -1;
                y = 0;
            } else if(a == 'B'){
                mover_porta = 1;
            }

            jogo->rato->direcao.mov_x = x;
            jogo->rato->direcao.mov_y = y;
        } else {
			a = toupper(a);

            if (a == 'C'){
                if(carregar_jogo(*jogo)){
					desenhar_cabecalho(*jogo->rato);
					desenhar_mapa(jogo->mapa, *jogo->rato);
				}
            } else if(a == 'S'){
                salvar_jogo(*jogo);
            } else if(a == 'N'){
                novo_jogo(jogo);
				desenhar_cabecalho(*jogo->rato);
				desenhar_mapa(jogo->mapa, *jogo->rato);
            } else if(a == 'Q'){
				exit(0);
			} else if(a == 'V'){
				desenhar_rodape(!jogo->jogo_pausado);
				jogo->jogo_pausado = !jogo->jogo_pausado;
			}
        }
    }
}

// exibe os valores de fim de jogo (pontuacao, nivel, etc)
void finalizar_jogo(ESTADO_JOGO *estado_jogo){

	estado_jogo->jogo_pausado = 1;

	textbackground(BLACK);
	textcolor(WHITE);
	clrscr();

	gotoxy(20, 10);
	cprintf("FIM DE JOGO!");
	gotoxy(20,11);
	cprintf("PONTUACAO FINAL: %d", estado_jogo->rato->pontos);
	gotoxy(20,12);
	cprintf("NIVEL ATINGIDO: %d", estado_jogo->nivel);

	desenhar_rodape(estado_jogo->jogo_pausado);
}

//executa o jogo
int main() {
	
	window(1, 1, 200, 250);

	pthread_t thread;

    setlocale(LC_ALL, "");

    time_t t;
    srand((unsigned) time(&t));

	ESTADO_JOGO *estado_jogo = malloc(sizeof(ESTADO_JOGO));
	GATO gatos[NUM_GATOS];
	PORTA portas[NUM_PORTAS];
	RATO rato;

    char mapa[LINHAS_MAPA][COLUNAS_MAPA];
	char novo_mapa[LINHAS_MAPA][COLUNAS_MAPA];
	char fase[LINHAS_MAPA][COLUNAS_MAPA];

	estado_jogo->jogo_pausado = 0;
	estado_jogo->rato = &rato;
	estado_jogo->fase = &fase;
	estado_jogo->mapa = &mapa;
	estado_jogo->novo_mapa = &novo_mapa;
	estado_jogo->gatos = &gatos;
	estado_jogo->portas = &portas;

    while(novo_jogo(estado_jogo) == 0){
		gotoxy(1, 1);
		printf("Arquivo do mapa não encontrado, favor conferir e então apertar qualquer tecla para tentar novamente");
		_getch();
	}

	clrscr();

	pthread_create(&thread, NULL, thread_ler_teclado, estado_jogo);

    desenhar_cabecalho(rato);
	desenhar_mapa(mapa, rato);
    desenhar_rodape(estado_jogo->jogo_pausado);

	Sleep(1000);

    while(1) {

        if(estado_jogo->jogo_pausado == 0){
            if(mover_rato(&rato, &gatos, &fase, &mapa, &novo_mapa, &estado_jogo->queijos_faltando)){
                //se o rato se moveu, atualiza o cabeçalho
                desenhar_cabecalho(rato);
            }
            if(mover_porta){
                mover_porta = 0;
                mover_portas(&portas, &fase, &mapa, &novo_mapa);
            }

			for(int j = 0; j < NUM_GATOS; j++) {
				if(!gatos[j].vivo){
					gatos[j].cont_vivo += 1;
				}
				else {
					mover_gato(&gatos[j], &mapa, &novo_mapa, fase);
				}
				if(gatos[j].cont_vivo >= 25){
					gatos[j].vivo = 1;
					gatos[j].cont_vivo = 0;
				}
			}

            if(checar_colisao(&rato, &gatos, NUM_GATOS, &novo_mapa)){
                rato.vidas--;
                desenhar_cabecalho(rato);
            }
			if(rato.cachorro){
				rato.cont_cachorro += 1;
				if(rato.cont_cachorro > 25){
					rato.cachorro = 0;
					rato.sprite = sprite_rato;
				}
			}
			atualizar_mapa(&mapa, &novo_mapa, rato);

			if(rato.vidas == -1){
				finalizar_jogo(estado_jogo);
			}

			if(estado_jogo->queijos_faltando == 0){
				if(proxima_fase(estado_jogo)){
					desenhar_cabecalho(rato);
					desenhar_mapa(mapa, rato);
				} else {
					finalizar_jogo(estado_jogo);
				}
			}

            Sleep(200);
        } else {
			Sleep(1);
		}
    }

    return 0;
}
