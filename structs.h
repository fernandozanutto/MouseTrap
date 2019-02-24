#include <conio.h>

#define NUM_GATOS 4
#define NUM_PORTAS 7

#define LARGURA_BLOCO 5
#define ALTURA_BLOCO 5
#define COLUNAS_MAPA 27
#define LINHAS_MAPA 11

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


typedef struct {
	char linha[ALTURA_BLOCO][LARGURA_BLOCO+1];
	int backgroundcolor, textcolor;
} SPRITE;

SPRITE sprite_rato = {
	{
    "(\\ /)",
	" o o ",
	"     ",
	" -*- ",
	"  m  "
	},
	BROWN, WHITE
};

SPRITE sprite_osso = {
	{"  8  ",
	"  I  ",
	"8===8",
	"  I  ",
	"  8  "},
	LIGHTGRAY, BLUE
};

SPRITE sprite_porta = {
	{"|---|",
	"|   |",
	"|  O|",
	"|   |",
	"|___|"},
	DARKGRAY, WHITE
};

SPRITE sprite_gato = {
	{"/\\ /\\",
	" O O ",
	"~ ^ ~",
	" vvv ",
	"\\___/",},
	DARKGRAY, WHITE
};

SPRITE sprite_gato_derrotado = {
	{"/\\ /\\",
	" O O ",
	"~ ^ ~",
	" vvv ",
	"\\___/",},
	LIGHTGRAY, WHITE
};

SPRITE sprite_queijo = {
	{"     ",
	"     ",
	"  o  ",
	"     ",
	"     "},
	LIGHTGRAY, YELLOW
};

SPRITE sprite_fundo = {
	{"     ",
	"     ",
	"     ",
	"     ",
	"     "},
	LIGHTGRAY,	LIGHTGRAY
};

SPRITE sprite_parede = {
	{"     ",
	"     ",
	"     ",
	"     ",
	"     "},
	RED, WHITE
};

SPRITE sprite_cachorro = {
    {
        "     ",
        "/O O\\",
        "  T  ",
        "( w )",
        "     ",
    },
	BROWN, WHITE
};



typedef struct str_posicao {
    int x;
    int y;
} POSICAO;

typedef struct str_direcao {
    int mov_x;
    int mov_y;
} DIRECAO;

typedef struct str_gato {
    int vivo;
    int cont_vivo;
    POSICAO posicao;
    POSICAO posicao_antiga;
    POSICAO posicao_inicial;
    DIRECAO direcao;
    SPRITE sprite;
} GATO;

typedef struct RATO {
    POSICAO posicao;
    POSICAO posicao_antiga;
    POSICAO posicao_inicial;
    DIRECAO direcao;
    SPRITE sprite;
    int cachorro;
	int pontos;
	int vidas;
	int cont_cachorro;
} RATO;

typedef struct PORTA {
    POSICAO atual;
    POSICAO possivel;
    SPRITE sprite;
} PORTA;

typedef struct {
	int nivel;
	int queijos_faltando;
    RATO rato;
    char mapa[LINHAS_MAPA][COLUNAS_MAPA];
    char fase[LINHAS_MAPA][COLUNAS_MAPA];
    GATO gatos[NUM_GATOS];
	PORTA portas[NUM_PORTAS];
} SAVE_JOGO;

typedef struct {
	int nivel;
	int queijos_faltando;
	int jogo_pausado;
	RATO *rato;
	char (*mapa)[LINHAS_MAPA][COLUNAS_MAPA];
	char (*novo_mapa)[LINHAS_MAPA][COLUNAS_MAPA];
	char (*fase)[LINHAS_MAPA][COLUNAS_MAPA];
	GATO (*gatos)[NUM_GATOS];
	PORTA (*portas)[NUM_PORTAS];
} ESTADO_JOGO;