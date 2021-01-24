#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <sstream>
#include <windows.h>
#include <SDL_mixer.h>
#include <time.h>
#include <ctime>
#include <iostream>
#include <fstream>

using namespace std;

/*Declaración*/
const Uint8* estadoteclado;
int posx, posy, posxSlime, posySlime, posxDemon, posyDemon;
int posxAnterior, posyAnterior;
string nombreJugador;
bool vueltaDemon, vueltaSlime;
bool quit;
SDL_Event event;
SDL_Window* window;
SDL_Window* ventanaDerrota;
SDL_Renderer* renderer;
SDL_Surface* image;
SDL_Texture* texturapersonaje;

SDL_Surface* fondo;
SDL_Texture* texturafondo;

SDL_Surface* slime;
SDL_Texture* texturaSlime;
SDL_Rect rectSlime;

SDL_Surface* demon;
SDL_Texture* texturaDemon;
SDL_Rect rectDemon;

SDL_Surface* fondosPanel;
SDL_Texture* texturafondosPanel;

SDL_Surface* fondoVictoria;
SDL_Texture* texturaFondoVictoria;

SDL_Rect rectFuente;
SDL_Rect rectDestino;
SDL_Rect rectPanel;

SDL_bool Colision;

Mix_Chunk* efectocofre;
Mix_Chunk* chocaEnemigo;
Mix_Chunk* chocaPared;
Mix_Chunk* morir;
Mix_Chunk* aparicion;
Mix_Chunk* key;
Mix_Chunk* muertedefinitiva;

Mix_Music* soundmonedas;
Mix_Music* calabera;

//////////////////////////////////////////////////////////////////////////////////////////////
SDL_Surface* fondoMenuMapa;
SDL_Texture* texturaMenuMapa;

SDL_Surface* fondoMenuPersonaje;
SDL_Texture* texturaMenuPersonaje;

string mapaElegido;
//////////////////////////////////////////////////////////////////////////////////////////////

SDL_Surface* fondoBotones;
SDL_Texture* texturaBotones;

bool recompensaOculta = false;
bool cofreAbierto = false;
bool llaveObtenida = false;

int tipopersonaje;

unsigned tiempo;

bool continuar;

int monedas;
int monedasObtenidas;

int vidas;
int c;
int i;

//relacionado con el grafo
struct Vertice;
struct Arista;
struct Grafo;
Grafo* grafito;
int totalVertices;
int RecompensaAristaAleatoria;
/*Fin Declaración*/

struct Vertice {
    Vertice* siguiente = nullptr;
    Vertice* anterior = nullptr;
    Arista* cabezaArista = nullptr;
    bool visitado = false;

    int valor;
    int textura;
    int posicionX;
    int posicionY;
};

struct Arista {
    bool recompensa = false;
    bool activa = false;
    Arista* siguiente = nullptr;
    Arista* anterior = nullptr;
    Vertice* verOrigen = nullptr;
    Vertice* verDestino = nullptr;
};

struct Grafo {
    Vertice* cabeza = nullptr;
    Vertice* VerPersonaje = nullptr;
    Vertice* VerEnemigo1 = nullptr;
    Vertice* VerEnemigo2 = nullptr;

    //este metodo es solo de prueba no se deberia usar
    bool agregarVertice(int valor) {
        Vertice* nuevoVer = new Vertice;
        if (cabeza == nullptr) {
            nuevoVer->valor = valor;
            cabeza = nuevoVer;
            return true;
        }
        else {
            Vertice* vertaux = cabeza;
            while (vertaux != nullptr) {
                if (vertaux->valor == valor) {
                    return false;
                }
                vertaux = vertaux->siguiente;
            }
            Vertice* aux = cabeza;
            nuevoVer->valor = valor;
            cabeza = nuevoVer;
            cabeza->siguiente = aux;
            aux->anterior = cabeza;
            return true;
        }
    }

    //este es el correcto y el que se debería usar
    bool agregarVertice(int valor, int textura, int posicionX, int posicionY) {
        Vertice* nuevoVer = new Vertice;
        if (cabeza == nullptr) {
            nuevoVer->valor = valor;
            nuevoVer->textura = textura;
            nuevoVer->posicionX = posicionX;
            nuevoVer->posicionY = posicionY;
            nuevoVer->cabezaArista = nullptr;
            cabeza = nuevoVer;
            return true;
        }
        else {
            Vertice* vertaux = cabeza;
            while (vertaux != nullptr) {
                if (vertaux->valor == valor) {
                    return false;
                }
                vertaux = vertaux->siguiente;
            }
            Vertice* aux = cabeza;
            nuevoVer->valor = valor;
            nuevoVer->textura = textura;
            nuevoVer->posicionX = posicionX;
            nuevoVer->posicionY = posicionY;
            nuevoVer->cabezaArista = nullptr;
            cabeza = nuevoVer;
            cabeza->siguiente = aux;
            aux->anterior = cabeza;
            return true;
        }
    }

    //Falta terminar
    bool agregarArista(int origen, int destino) {

        if (buscarVertice(origen) == nullptr) {
            if (buscarVertice(destino) == nullptr) {
                if (buscarArista(origen, destino) != nullptr) {
                    return false;
                }

            }

        }
        else {
            Vertice* aux = cabeza;
            Vertice* verOrigen = nullptr;
            Vertice* verDestino = nullptr;
            while (aux != nullptr) {
                if (aux->valor == origen) {
                    verOrigen = aux;
                }
                if (aux->valor == destino) {
                    verDestino = aux;
                }
                if (verOrigen != nullptr && verDestino != nullptr) {        //este if representa que ya encontró ambos vertices

                    Arista* aristaaux = verOrigen->cabezaArista;
                    Arista* nuevacabeza = new Arista;
                    nuevacabeza->verOrigen = verOrigen;
                    nuevacabeza->verDestino = verDestino;
                    verOrigen->cabezaArista = nuevacabeza;
                    verOrigen->cabezaArista->siguiente = aristaaux;
                    if (aristaaux != nullptr) {
                        aristaaux->anterior = verOrigen->cabezaArista;
                    }
                    return true;
                }
                aux = aux->siguiente;
            }
        }
    }

    Vertice* buscarVertice(int valor) {
        Vertice* aux = cabeza;
        while (aux != nullptr) {
            if (aux->valor == valor) {
                return aux;
            }
            aux = aux->siguiente;
        }
        return nullptr;
    }

    Arista* buscarArista(int origen, int destino) {
        Vertice* aux = cabeza;

        while (aux != nullptr) {
            if (aux->valor == origen) {
                if (aux->cabezaArista != nullptr) {
                    Arista* auxarista = aux->cabezaArista;
                    while (auxarista != nullptr) {
                        if (auxarista->verDestino->valor == destino) {
                            return auxarista;
                        }
                    }
                }
                else {
                    break;
                }
            }
            aux = aux->siguiente;
        }
        return nullptr;
    }

};

/*
=======
/*Fin Declaración


>>>>>>> 5ab5da507073773e47527abc031432be52f7ae69
  1= muro vertical
  2= muro horizontal
  3= muro esquina superior izq
  4= muro esquina superior der
  5= muro esquina inferior izq
  6= muro esquina inferior der
  7= muro final izquierda
  8= muro final derecha
  9= muro final abajo
  10= muro final arriba
  11= muro intersección izq, arriba, der
  12= muro intersección izq, abajo, der
  13= muro intersección izq, arriba, abajo
  14= muro intersección der, arriba, abajo
  15= muro intersección total
  16= llave
  17= cofre
  18= calavera
  19= monedas
  */
int mapa[20][20];

int dungeon[20][20] = {
    {0,0,0,0,0,18,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,10,0,7,2,2,6,0,7,4,0,0,0,3,2,2,8,0,10,0},
    {0,1,0,0,0,0,0,0,0,5,4,0,0,1,0,0,0,0,1,0},
    {0,5,2,2,2,2,2,4,0,16,1,0,0,1,0,0,0,0,1,0},
    {0,0,0,0,0,0,0,5,2,2,6,0,3,6,0,7,4,0,9,0},
    {0,3,2,2,2,4,0,0,0,0,0,0,1,0,0,0,1,0,0,0},
    {0,9,0,0,0,5,4,0,7,2,12,2,6,0,10,0,5,2,2,2},
    {0,0,0,0,0,0,1,0,0,18,1,0,0,0,1,0,0,0,0,18},
    {2,4,0,0,10,0,5,2,2,2,6,0,3,2,15,2,2,2,2,2},
    {18,1,0,0,1,0,0,0,0,0,0,0,1,0,9,0,0,0,0,0},
    {0,1,0,0,5,2,2,2,4,0,3,2,6,0,0,0,7,2,4,0},
    {0,9,0,0,0,0,0,0,1,18,1,0,0,0,0,0,0,0,1,0},
    {0,0,0,0,7,2,4,0,14,2,6,0,0,0,7,12,2,2,6,0},
    {2,2,4,0,0,0,1,0,1,0,0,0,10,0,18,1,0,0,0,0},
    {17,0,9,0,0,0,1,0,1,0,7,2,11,2,2,6,0,7,4,0},
    {0,0,0,0,10,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0},
    {2,2,4,0,5,2,6,0,5,2,8,0,7,2,2,2,4,0,1,0},
    {0,18,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,5,2},
    {0,7,11,2,8,0,7,2,2,12,2,2,2,2,8,0,1,0,0,0},
    {0,0,0,0,0,0,0,0,18,1,0,0,0,0,0,0,1,0,10,0} };

int garden[20][20] = {
    {0,0,0,0,0,0,0,0,0,0,16,0,0,0,0,0,0,0,0,0},
    {0,3,2,2,2,8,0,7,2,2,2,2,8,0,7,2,2,2,4,0},
    {0,1,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,18,1,0},
    {0,5,2,8,0,7,2,2,2,2,2,2,2,2,8,0,7,2,6,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {2,8,0,7,2,2,2,2,2,2,2,2,2,2,2,2,8,0,7,2},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,3,2,2,2,8,0,7,2,2,2,2,8,0,7,2,2,2,4,0},
    {0,1,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,18,1,0},
    {0,5,2,8,0,7,2,2,2,2,2,2,2,2,8,0,7,2,6,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {2,8,0,7,2,2,2,2,2,2,2,2,2,2,2,2,8,0,7,2},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,3,2,2,2,8,0,7,2,2,2,2,8,0,7,2,2,2,4,0},
    {0,1,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,18,1,0},
    {0,5,2,8,0,7,2,2,2,2,2,2,2,2,8,0,7,2,6,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {2,4,0,0,10,0,3,2,4,0,0,3,2,4,0,10,0,0,3,2},
    {17,5,8,0,5,2,6,0,9,0,0,9,0,5,2,6,0,7,6,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} };

int desert[20][20] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16},
    {0,3,2,12,8,0,3,2,2,12,2,2,2,2,8,0,3,2,2,2},
    {0,9,18,1,0,0,9,0,0,1,0,0,0,0,0,0,9,0,0,18},
    {0,0,0,1,0,0,0,0,0,1,0,0,0,0,10,0,0,0,10,0},
    {0,3,2,11,2,2,2,8,0,9,0,7,2,2,6,0,10,0,1,0},
    {0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,9,0},
    {0,0,0,3,8,0,7,2,2,2,2,2,8,0,3,2,6,0,0,0},
    {0,7,2,6,0,0,0,0,0,0,0,0,0,0,1,0,0,0,10,0},
    {0,0,0,0,0,3,2,2,2,2,2,2,2,2,6,0,3,2,13,0},
    {0,10,0,7,2,6,0,0,0,0,0,0,0,0,0,0,1,0,1,18},
    {18,1,0,0,0,0,0,3,8,0,7,2,2,8,0,7,6,0,5,2},
    {2,11,2,8,0,10,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,5,2,6,0,3,2,12,2,2,8,0,7,2,4,0},
    {0,10,0,10,0,0,0,0,0,1,0,9,0,0,0,0,0,0,1,0},
    {0,1,0,5,2,2,8,0,7,6,0,0,0,3,2,8,0,7,6,0},
    {0,1,0,0,0,0,0,0,0,0,0,10,0,1,0,0,0,0,0,0},
    {0,5,2,4,0,7,2,2,2,2,2,6,0,5,2,2,2,2,4,0},
    {0,0,17,1,0,0,0,0,0,0,0,0,0,0,0,0,0,18,1,0},
    {0,7,2,11,8,0,7,2,2,4,0,7,2,2,2,2,2,2,6,0},
    {0,0,0,0,0,0,0,0,18,1,0,0,0,0,0,0,0,0,0,0} };

int town[20][20] = {
    {0,0,0,0,16,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,3,2,2,2,11,2,2,2,8,0,7,2,2,12,2,2,2,4,0},
    {0,1,18,0,0,0,0,0,0,0,0,0,0,0,1,0,0,18,1,0},
    {0,9,0,3,2,2,2,2,2,2,2,2,2,2,11,2,4,0,9,0},
    {0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
    {0,10,0,1,0,7,2,2,2,8,0,7,2,2,8,0,1,0,10,0},
    {0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0},
    {0,1,0,9,0,3,2,2,2,2,2,2,2,2,12,2,6,0,1,0},
    {0,1,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0},
    {0,1,0,10,0,9,0,3,2,2,2,2,8,0,1,0,10,0,1,0},
    {0,1,0,1,0,0,0,1,17,0,0,0,0,0,1,0,1,0,1,0},
    {0,1,0,9,0,10,0,5,2,2,2,2,8,0,1,0,9,0,1,0},
    {0,1,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0},
    {0,1,18,3,2,11,2,2,2,2,12,2,2,2,6,0,10,18,1,0},
    {0,5,2,6,0,0,0,0,0,0,1,0,0,0,0,0,5,2,6,0},
    {0,0,0,0,0,10,0,10,0,0,1,0,10,0,10,0,0,0,0,0},
    {0,3,2,4,0,5,2,11,4,0,5,2,11,2,6,0,3,2,4,0},
    {0,1,18,1,0,0,0,0,1,0,0,0,0,0,0,0,1,18,1,0},
    {0,1,0,5,2,8,0,7,11,2,2,2,8,0,7,2,6,0,1,0},
    {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0} };

/*Lista con la ubicación de las recompensas*/
int recompensas[20];

//Declaración de todos los métodos
bool llenarGrafo(int matriz[20][20]);
bool llenarVertices(int matriz[20][20]);
bool llenarAristas(int matriz[20][20]);
bool sePuedeCaminarEn(int matriz[20][20], int posx, int posy);
void pantallaVictoria();
void pantallaDerrota();
void pintarmapa(SDL_Rect personaje);
void panel();
bool existeColisionArriba(int posxPersonaje, int posyPersonaje);
bool existeColisionAbajo(int posxPersonaje, int posyPersonaje);
bool existeColisionIzquierda(int posxPersonaje, int posyPersonaje);
bool existeColisionDerecha(int posxPersonaje, int posyPersonaje);
void monedaRandom(int recompensaactual);
void inicializar();
void leerEvento();
void escribirVidas();
void existecolisionenemigo();
void movimientoenemigo();
void tiempoYSprites();
void finalizar();
void menuPrincipal();
void menuCargarPartidas();
void menuCrearPartida();
void menuRankings();
bool guardarPartida();
void menuEliminarPartidas();
//Fin declaracion de metodos

void ventanaPersonaje() {

    rectFuente = { 0,0,800,640 };

    while (!quit) {


        while (SDL_PollEvent(&event) != NULL)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(-1);
                break;
            }
        }

        Uint32 ticks = SDL_GetTicks();
        Uint32 sprite = (ticks / 100) % 5;

        SDL_Rect rectFArquera = { sprite * 32, 32, 32, 32 };
        SDL_Rect rectDArquera = { 120, 350, 32, 32 };

        SDL_Rect rectFMago = { sprite * 32, 64, 32, 32 };
        SDL_Rect rectDMago = { 290,  350, 32, 32 };

        SDL_Rect rectFHada = { sprite * 32, 0, 32, 32 };
        SDL_Rect rectDHada = { 480, 350, 32, 32 };

        SDL_Rect rectFCaballero = { sprite * 32, 96, 32, 32 };
        SDL_Rect rectDCaballero = { 655,  350, 32, 32 };


        SDL_RenderCopy(renderer, texturaMenuPersonaje, &rectFuente, &rectFuente);

        SDL_RenderCopy(renderer, texturapersonaje, &rectFArquera, &rectDArquera);
        SDL_RenderCopy(renderer, texturapersonaje, &rectFMago, &rectDMago);
        SDL_RenderCopy(renderer, texturapersonaje, &rectFHada, &rectDHada);
        SDL_RenderCopy(renderer, texturapersonaje, &rectFCaballero, &rectDCaballero);

        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);



        if (event.button.button == SDL_BUTTON_LEFT)
        {
            int x = event.button.x;
            int y = event.button.y;

            //arquera
            if ((x > 120) && (x < 152) && (y > 350) && (y < 384))

            {
                tipopersonaje = 32;
                Mix_PlayMusic(soundmonedas, 0);
                break;

            }
            //mago 
            else if ((x > 290) && (x < 322) && (y > 350) && (y < 384))

            {
                tipopersonaje = 64;
                Mix_PlayMusic(soundmonedas, 0);
                break;
            }
            //hada 
            else if ((x > 480) && (x < 512) && (y > 350) && (y < 384))

            {
                tipopersonaje = 0;
                Mix_PlayMusic(soundmonedas, 0);
                break;
            }
            //caballero
            if ((x > 655) && (x < 687) && (y > 350) && (y < 384))
            {
                tipopersonaje = 96;
                Mix_PlayMusic(soundmonedas, 0);
                break;
            }

        }
    }
}

void ventanaMapas() {
    rectFuente = { 0,0,800,640 };

    while (!quit) {

        while (SDL_PollEvent(&event) != NULL)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(-1);
                break;
            }
        }

        SDL_RenderCopy(renderer, texturaMenuMapa, &rectFuente, &rectFuente);
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);

        if (event.button.button == SDL_BUTTON_LEFT)
        {

            int x = event.button.x;
            int y = event.button.y;

            //garden
            if ((x > 135) && (x < 328) && (y > 170) && (y < 320))

            {
                mapaElegido = "garden";
                for (int y = 0; y < 20; y++) {
                    for (int x = 0; x < 20; x++) {
                        mapa[y][x] = garden[y][x];
                    }
                }
                posySlime = 4 * 32;
                posxSlime = 0 * 32;
                posyDemon = 12 * 32;
                posxDemon = 32 * 19;
                break;
            }

            //dungeon 
            else if ((x > 472) && (x < 665) && (y > 170) && (y < 320))
            {
                mapaElegido = "dungeon";
                for (int y = 0; y < 20; y++) {
                    for (int x = 0; x < 20; x++) {
                        mapa[y][x] = dungeon[y][x];
                    }
                }
                posySlime = 2 * 32;
                posxSlime = 3 * 32;
                posyDemon = 17 * 32;
                posxDemon = 6 * 19;
                break;
            }

            //town 
            else if ((x > 135) && (x < 328) && (y > 400) && (y < 550))
            {
                mapaElegido = "town";
                for (int y = 0; y < 20; y++) {
                    for (int x = 0; x < 20; x++) {
                        mapa[y][x] = town[y][x];
                    }
                }
                posySlime = 0;
                posxSlime = 9 * 32;
                posyDemon = 12 * 32;
                posxDemon = 0;
                break;
            }
            //desert
            else if ((x > 472) && (x < 665) && (y > 400) && (y < 550))
            {
                mapaElegido = "desert";
                for (int y = 0; y < 20; y++) {
                    for (int x = 0; x < 20; x++) {
                        mapa[y][x] = desert[y][x];
                    }
                }
                posySlime = 0;
                posxSlime = 0;
                posyDemon = 15 * 32;
                posxDemon = 0;
                break;

            }
        }
    }
}

bool llenarGrafo(int matriz[20][20]) {
    if (llenarVertices(matriz) && llenarAristas(matriz)) {
        return true;
    }
    else {
        return false;
    }
}

bool llenarVertices(int matriz[20][20]) {
    int cont = 0;
    for (int posy = 19; posy >= 0; posy--) {
        for (int posx = 19; posx >= 0; posx--) {
            if (sePuedeCaminarEn(matriz, posy, posx)) {
                grafito->agregarVertice(cont, matriz[posy][posx], posy, posx);
                cont++;
            }

        }
    }
    totalVertices = cont;
    return true;
}

bool llenarAristas(int matriz[20][20]) {
    int cont = 0;
    //Dos for para recorrer la matriz valor por valor
    for (int posy = 19; posy >= 0; posy--) {
        for (int posx = 19; posx >= 0; posx--) {
            if (sePuedeCaminarEn(matriz, posy, posx)) {
                if (posy > 0) {
                    if (sePuedeCaminarEn(matriz, posy - 1, posx)) {
                        grafito->agregarArista(cont, cont - 20);
                    }
                }
                if (posy < 19) {
                    if (sePuedeCaminarEn(matriz, posy + 1, posx)) {
                        grafito->agregarArista(cont, cont + 20);
                    }
                }
                if (posx > 0) {
                    if (sePuedeCaminarEn(matriz, posy, posx - 1)) {
                        grafito->agregarArista(cont, cont - 1);
                    }
                }
                if (posx < 19) {
                    if (sePuedeCaminarEn(matriz, posy, posx + 1)) {
                        grafito->agregarArista(cont, cont + 1);
                    }
                }
                cont++;
            }

        }
    }
    return true;
}

bool sePuedeCaminarEn(int matriz[20][20], int posy, int posx) {
    if (matriz[posy][posx] == 0 || matriz[posy][posx] == 16 || matriz[posy][posx] == 17 || matriz[posy][posx] == 18 || matriz[posy][posx] == 19) {
        return true;
    }
    else {
        return false;
    }
}

void pantallaVictoria() {

    rectFuente = { 0,0,640,640 };
    rectDestino = { 80, 0, 640, 640 };

    while (!quit) {


        while (SDL_PollEvent(&event) != NULL)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(-1);
                break;
            }
        }
        SDL_RenderCopy(renderer, texturaFondoVictoria, &rectFuente, &rectDestino);
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);

    }
    SDL_DestroyWindow(ventanaDerrota);

}

void pantallaDerrota() {

    rectFuente = { 32,0,640,640 };
    rectDestino = { 80, 0, 640, 640 };

    while (!quit) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        while (SDL_PollEvent(&event) != NULL)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(-1);
                break;
            }
        }
        SDL_RenderCopy(renderer, texturafondosPanel, &rectFuente, &rectDestino);
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);

    }
    SDL_DestroyWindow(ventanaDerrota);

    SDL_Delay(3000);

}

void pintarmapa(SDL_Rect personaje) {
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            //////////////////////////////////////////////////////////////////////////////////////////////
            if (mapaElegido == "dungeon")
            {
                rectFuente = { 64, 1152, 32, 32 };//piso (dungeon)}
            }
            else if (mapaElegido == "garden")
            {
                rectFuente = { 96, 0, 32, 32 };//piso(garden)
            }
            else if (mapaElegido == "desert")
            {
                rectFuente = { 192, 3648, 32, 32 };//piso (desert)
            }
            else if (mapaElegido == "town")
            {
                rectFuente = { 32, 1184, 32, 32 };//piso (town)
            }
            //////////////////////////////////////////////////////////////////////////////////////////////
            rectDestino = { 32 * j, 32 * i, 32, 32 };
            switch (mapa[i][j])
            {
            case 0://Suelo
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 1://muro vertical
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                rectFuente = { 0, 832, 32, 32 };
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 2://muro horizontal
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                rectFuente = { 0, 864, 32, 32 };
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 4://muro esquina superior izq
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                rectFuente = { 96, 832, 32, 32 };
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 3://muro esquina superior der
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                rectFuente = { 64, 832, 32, 32 };
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 5://muro esquina inferior izq
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                rectFuente = { 64, 864, 32, 32 };
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 6://muro esquina inferior der
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                rectFuente = { 96, 864, 32, 32 };
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 8://muro final izquierda
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                rectFuente = { 128, 832, 32, 32 };
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 7://muro final derecha
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                rectFuente = { 128, 864, 32, 32 };
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 9://muro final abajo
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                rectFuente = { 32, 864, 32, 32 };
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 10://muro final arriba
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                rectFuente = { 32, 832, 32, 32 };
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 11://muro intersección izq, arriba, der
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                rectFuente = { 160, 832, 32, 32 };
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 12://muro intersección izq, abajo, der
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                rectFuente = { 192, 832, 32, 32 };
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 13://muro intersección izq, arriba, abajo
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                rectFuente = { 192, 864, 32, 32 };
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 14://muro intersección der, arriba, abajo
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                rectFuente = { 160, 864, 32, 32 };
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 15://muro intersección total
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                rectFuente = { 224, 832, 32, 32 };
                SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                break;
            case 16://llave
                Colision = SDL_HasIntersection(&personaje, &rectDestino);
                if (Colision) {
                    if (llaveObtenida != true) {
                        Mix_PlayChannel(-1, key, 0);
                    }
                    llaveObtenida = true;
                    SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                    rectFuente = { 224, 4192, 32, 32 };
                    SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                    rectPanel = { 704,544,32,32 };
                    rectFuente = { 0, 32, 32, 32 };
                    SDL_RenderCopy(renderer, texturafondosPanel, &rectFuente, &rectPanel);
                }
                else {
                    if (llaveObtenida == false) {
                        SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                        rectFuente = { 224, 4192, 32, 32 };
                        SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                    }
                    else
                    {
                        SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                        rectPanel = { 704,195,32,32 };
                        rectFuente = { 0, 32, 32, 32 };
                        SDL_RenderCopy(renderer, texturafondosPanel, &rectFuente, &rectPanel);
                    }

                }
                break;
            case 17://cofre                                                                       
                Colision = SDL_HasIntersection(&personaje, &rectDestino);
                if (Colision && llaveObtenida == true && cofreAbierto == false) {
                    Mix_PlayChannel(-1, efectocofre, 0);
                    cofreAbierto = true;
                    SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                    rectFuente = { 192, 3456, 32, 32 };
                    SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                    pantallaVictoria();
                }
                else {
                    if (cofreAbierto == false) {
                        SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                        rectFuente = { 192, 3424, 32, 32 };
                        SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                    }
                    else
                    {
                        SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                        rectFuente = { 192, 3456, 32, 32 };
                        SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                    }
                }
                break;
            case 18://calavera
                Colision = SDL_HasIntersection(&personaje, &rectDestino);
                if (Colision) {
                    SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                    rectFuente = { 192, 4192, 32, 32 };
                    SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                    Mix_PlayMusic(calabera, 0);
                }
                else {
                    SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                    rectFuente = { 192, 4192, 32, 32 };
                    SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                }
                break;
            case 19://recompensa Monedas
                Colision = SDL_HasIntersection(&personaje, &rectDestino);
                if (Colision) {
                    mapa[i][j] = 0;
                    Mix_PlayMusic(soundmonedas, 0);
                    monedas += 32;
                }
                else {

                    SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                    rectFuente = { 64, 3963, 32, 32 };
                    SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectDestino);
                }
                break;

            default:
                break;
            }
        }
    }
}

void panel() {
    rectFuente = { 64, 3963, 32, 32 };//moneda
    rectPanel = { 690,302,32,32 };
    SDL_RenderCopy(renderer, texturafondo, &rectFuente, &rectPanel);
    rectFuente = { monedas , 640, 32, 32 };//contador moneda
    rectPanel = { 722,300,32,32 };
    SDL_RenderCopy(renderer, texturafondosPanel, &rectFuente, &rectPanel);

    SDL_Rect rectFGuardar = { 196,32, 138, 45 };

    SDL_Rect rectDGuardar = { 655,550,130,40 };

    SDL_RenderCopy(renderer, texturaBotones, &rectFGuardar, &rectDGuardar);

    if (recompensaOculta) {
        rectFuente = { 0 , 64, 32, 32 };//contador moneda
        rectPanel = { 704,420,32,32 };
        SDL_RenderCopy(renderer, texturafondosPanel, &rectFuente, &rectPanel);
    }


    if (event.button.button == SDL_BUTTON_LEFT)
    {
        if (event.button.state == SDL_PRESSED) {

        }
        else if (event.button.state == SDL_RELEASED) {
            int x = event.button.x;
            int y = event.button.y;

            if ((x > 655) && (x < 785) && (y > 550) && (y < 590))

            {
                Mix_PlayMusic(soundmonedas, 0);
                guardarPartida();
            }
        }
        
        //BOTON PAUSA    }
    }
}

bool existeColisionArriba(int posxPersonaje, int posyPersonaje) {
    int posyMuro;
    int posxMuro;
    bool colision;
    for (int fila = 0; fila < 20; fila++)
    {
        for (int columna = 0; columna < 20; columna++) {
            if (mapa[fila][columna] == 1 || mapa[fila][columna] == 2 || mapa[fila][columna] == 3 || mapa[fila][columna] == 4 || mapa[fila][columna] == 5 || mapa[fila][columna] == 6 || mapa[fila][columna] == 7 || mapa[fila][columna] == 8
                || mapa[fila][columna] == 9 || mapa[fila][columna] == 10 || mapa[fila][columna] == 11 || mapa[fila][columna] == 12 || mapa[fila][columna] == 13 || mapa[fila][columna] == 14 || mapa[fila][columna] == 15) {
                posxMuro = columna * 32;
                posyMuro = (fila * 32) + 32;
                if (posxMuro == posxPersonaje && posyMuro == posyPersonaje) {
                    Mix_PlayChannel(-1, chocaPared, 0);
                    return true;
                }
            }
        }
    }
    return false;
}

bool existeColisionAbajo(int posxPersonaje, int posyPersonaje) {
    int posyMuro;
    int posxMuro;
    bool colision;
    for (int fila = 0; fila < 20; fila++)
    {
        for (int columna = 0; columna < 20; columna++) {
            if (mapa[fila][columna] == 1 || mapa[fila][columna] == 2 || mapa[fila][columna] == 3 || mapa[fila][columna] == 4 || mapa[fila][columna] == 5 || mapa[fila][columna] == 6 || mapa[fila][columna] == 7 || mapa[fila][columna] == 8
                || mapa[fila][columna] == 9 || mapa[fila][columna] == 10 || mapa[fila][columna] == 11 || mapa[fila][columna] == 12 || mapa[fila][columna] == 13 || mapa[fila][columna] == 14 || mapa[fila][columna] == 15) {
                posxMuro = columna * 32;
                posyMuro = (fila * 32) - 32;
                if (posxMuro == posxPersonaje && posyMuro == posyPersonaje) {
                    Mix_PlayChannel(-1, chocaPared, 0);
                    return true;
                }
            }
        }
    }
    return false;
}

bool existeColisionIzquierda(int posxPersonaje, int posyPersonaje) {
    int posyMuro;
    int posxMuro;
    bool colision;
    for (int fila = 0; fila < 20; fila++)
    {
        for (int columna = 0; columna < 20; columna++) {
            if (mapa[fila][columna] == 1 || mapa[fila][columna] == 2 || mapa[fila][columna] == 3 || mapa[fila][columna] == 4 || mapa[fila][columna] == 5 || mapa[fila][columna] == 6 || mapa[fila][columna] == 7 || mapa[fila][columna] == 8
                || mapa[fila][columna] == 9 || mapa[fila][columna] == 10 || mapa[fila][columna] == 11 || mapa[fila][columna] == 12 || mapa[fila][columna] == 13 || mapa[fila][columna] == 14 || mapa[fila][columna] == 15) {
                posxMuro = (columna * 32) + 32;
                posyMuro = fila * 32;
                if (posxMuro == posxPersonaje && posyMuro == posyPersonaje) {
                    Mix_PlayChannel(-1, chocaPared, 0);
                    return true;
                }
            }
        }
    }
    return false;
}

bool existeColisionDerecha(int posxPersonaje, int posyPersonaje) {
    int posyMuro;
    int posxMuro;
    bool colision;
    for (int fila = 0; fila < 20; fila++)
    {
        for (int columna = 0; columna < 20; columna++) {
            if (mapa[fila][columna] == 1 || mapa[fila][columna] == 2 || mapa[fila][columna] == 3 || mapa[fila][columna] == 4 || mapa[fila][columna] == 5 || mapa[fila][columna] == 6 || mapa[fila][columna] == 7 || mapa[fila][columna] == 8
                || mapa[fila][columna] == 9 || mapa[fila][columna] == 10 || mapa[fila][columna] == 11 || mapa[fila][columna] == 12 || mapa[fila][columna] == 13 || mapa[fila][columna] == 14 || mapa[fila][columna] == 15) {
                posxMuro = (columna * 32) - 32;
                posyMuro = fila * 32;
                if (posxMuro == posxPersonaje && posyMuro == posyPersonaje) {
                    Mix_PlayChannel(-1, chocaPared, 0);
                    return true;
                }
            }
        }
    }
    return false;
}

void monedaRandom(int recompensaactual) {

    int posrandom = int(rand() % 400);
    int cont = 0;
    bool salir = false;
    for (int fila = 0; fila < 20; fila++) {
        for (int columna = 0; columna < 20; columna++) {
            if (cont == posrandom && mapa[fila][columna] == 0) {
                mapa[fila][columna] = 19;
                recompensas[recompensaactual] == posrandom;
                salir = true;
                break;
            }
            else {
                if (cont >= posrandom && mapa[fila][columna] == 0) {
                    mapa[fila][columna] = 19;
                    recompensas[recompensaactual] == cont;
                    salir = true;
                    break;
                }
            }
            cont++;
        }
        if (salir) {
            break;
        }
    }
}

void generarRecompensaSecretaAleatoria() {
    Vertice* vertaux = grafito->cabeza;
    Arista* arisaux;
    srand(time(NULL));
    RecompensaAristaAleatoria = rand() % totalVertices - 1;
    cout << RecompensaAristaAleatoria;
    while (vertaux != nullptr) {
        if (vertaux->valor == RecompensaAristaAleatoria) {
            vertaux->cabezaArista->recompensa = true;
            break;
        }
        vertaux = vertaux->siguiente;
    }

}

//Se termina despues de la aleatoria
void recompensaOcultaObtenida() {

    Vertice* vertaux = grafito->cabeza;
    Arista* arisaux;
    while (vertaux != nullptr) {
        if ((vertaux->posicionX * 32 == posyAnterior) && (vertaux->posicionY * 32 == posxAnterior)) {
            arisaux = vertaux->cabezaArista;
            while (arisaux != nullptr) {
                if ((arisaux->verDestino->posicionX * 32 == posy) && (arisaux->verDestino->posicionY * 32 == posx) && arisaux->recompensa == true) {
                    recompensaOculta = true;
                }
                arisaux = arisaux->siguiente;
            }
        }
        vertaux = vertaux->siguiente;
    }

}

void inicializar() {
    quit = false;
    posx = 608;
    posy = 608;
    posxAnterior = posx;
    posyAnterior = posy;
    posxSlime = 0;
    posySlime = 0;
    posxDemon = 192;
    posyDemon = 608;
    vueltaDemon = true;
    vueltaSlime = true;
    vidas = 3;
    llaveObtenida = false;
    grafito = new Grafo;
    monedasObtenidas = 0;

    continuar = true;

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    /*Comando para reproducir la musica, solo necesita una muscia de fondo*/
    //Mix_PlayingMusic(musica, -1);
    estadoteclado = SDL_GetKeyboardState(NULL);

    window = SDL_CreateWindow("SDL2 Moving Wizard", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 640, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    image = IMG_Load("personajes.png");
    texturapersonaje = SDL_CreateTextureFromSurface(renderer, image);

    slime = IMG_Load("slime.png");
    texturaSlime = SDL_CreateTextureFromSurface(renderer, slime);

    demon = IMG_Load("demon.png");
    texturaDemon = SDL_CreateTextureFromSurface(renderer, demon);

    fondosPanel = IMG_Load("fondosPanel.png");
    texturafondosPanel = SDL_CreateTextureFromSurface(renderer, fondosPanel);

    fondoVictoria = IMG_Load("fondosVictoria.png");
    texturaFondoVictoria = SDL_CreateTextureFromSurface(renderer, fondoVictoria);
    SDL_FreeSurface(fondoVictoria);

    fondo = IMG_Load("fondos.png");
    texturafondo = SDL_CreateTextureFromSurface(renderer, fondo);
    SDL_FreeSurface(fondo);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    fondoMenuMapa = IMG_Load("menuMapas.png");
    texturaMenuMapa = SDL_CreateTextureFromSurface(renderer, fondoMenuMapa);
    SDL_FreeSurface(fondoMenuMapa);

    fondoMenuPersonaje = IMG_Load("menuPersonaje.png");
    texturaMenuPersonaje = SDL_CreateTextureFromSurface(renderer, fondoMenuPersonaje);

    fondoBotones = IMG_Load("botones.png");
    texturaBotones = SDL_CreateTextureFromSurface(renderer, fondoBotones);

    //Mix_Init(MIX_INIT_MP3);
    //if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) { //Se inicializa la funcion para el audio
        //fprintf(stderr, "Error no se puede iniciar el sistema", SDL_GetError());
    //}
    //Mix_FreeChunk(Sonido);
    key = Mix_LoadWAV("Key.wav");  //Se atribulle el .wav (El wav se atribulle con Mix_LoadWAV)
    efectocofre = Mix_LoadWAV("Cofre.wav");
    chocaPared = Mix_LoadWAV("ChocaPared.wav");
    chocaEnemigo = Mix_LoadWAV("ChocaEnemigo.wav");
    aparicion = Mix_LoadWAV("Aparicion.wav");
    morir = Mix_LoadWAV("Morir.wav");
    muertedefinitiva = Mix_LoadWAV("MuerteDefinitiva.wav");
    soundmonedas = Mix_LoadMUS("Monedas.mp3");
    calabera = Mix_LoadMUS("Calabera.mp3");//Se atribulle el .mp3 (El mp3 se atribulle con Mix_LoadMUS) 

    //////////////////////////////////////////////////////////////////////////////////////////////
    Mix_PlayChannel(-1, aparicion, 0);
    ventanaPersonaje();
    ventanaMapas();
    Mix_PlayChannel(-1, aparicion, 0);
    //////////////////////////////////////////////////////////////////////////////////////////////
    int recompensaActual = 0;
    for (int x = 0; x < 20; x++) {
        monedaRandom(recompensaActual);
        recompensaActual++;
    }
    llenarGrafo(mapa);
    generarRecompensaSecretaAleatoria();
    tiempo = clock();
}

void inicializarCarga() {
    quit = false;
    continuar = true;
    grafito = new Grafo;


    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    /*Comando para reproducir la musica, solo necesita una muscia de fondo*/
    //Mix_PlayingMusic(musica, -1);
    estadoteclado = SDL_GetKeyboardState(NULL);

    window = SDL_CreateWindow("SDL2 Moving Wizard", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 640, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    image = IMG_Load("personajes.png");
    texturapersonaje = SDL_CreateTextureFromSurface(renderer, image);

    slime = IMG_Load("slime.png");
    texturaSlime = SDL_CreateTextureFromSurface(renderer, slime);

    demon = IMG_Load("demon.png");
    texturaDemon = SDL_CreateTextureFromSurface(renderer, demon);

    fondosPanel = IMG_Load("fondosPanel.png");
    texturafondosPanel = SDL_CreateTextureFromSurface(renderer, fondosPanel);

    fondoVictoria = IMG_Load("fondosVictoria.png");
    texturaFondoVictoria = SDL_CreateTextureFromSurface(renderer, fondoVictoria);
    SDL_FreeSurface(fondoVictoria);

    fondo = IMG_Load("fondos.png");
    texturafondo = SDL_CreateTextureFromSurface(renderer, fondo);
    SDL_FreeSurface(fondo);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    fondoMenuMapa = IMG_Load("menuMapas.png");
    texturaMenuMapa = SDL_CreateTextureFromSurface(renderer, fondoMenuMapa);
    SDL_FreeSurface(fondoMenuMapa);

    fondoMenuPersonaje = IMG_Load("menuPersonaje.png");
    texturaMenuPersonaje = SDL_CreateTextureFromSurface(renderer, fondoMenuPersonaje);

    fondoBotones = IMG_Load("botones.png");
    texturaBotones = SDL_CreateTextureFromSurface(renderer, fondoBotones);

    //Mix_Init(MIX_INIT_MP3);
    //if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) { //Se inicializa la funcion para el audio
        //fprintf(stderr, "Error no se puede iniciar el sistema", SDL_GetError());
    //}
    //Mix_FreeChunk(Sonido);
    key = Mix_LoadWAV("Key.wav");  //Se atribulle el .wav (El wav se atribulle con Mix_LoadWAV)
    efectocofre = Mix_LoadWAV("Cofre.wav");
    chocaPared = Mix_LoadWAV("ChocaPared.wav");
    chocaEnemigo = Mix_LoadWAV("ChocaEnemigo.wav");
    aparicion = Mix_LoadWAV("Aparicion.wav");
    morir = Mix_LoadWAV("Morir.wav");
    muertedefinitiva = Mix_LoadWAV("MuerteDefinitiva.wav");
    soundmonedas = Mix_LoadMUS("Monedas.mp3");
    calabera = Mix_LoadMUS("Calabera.mp3");//Se atribulle el .mp3 (El mp3 se atribulle con Mix_LoadMUS) 

    //////////////////////////////////////////////////////////////////////////////////////////////
    if (mapaElegido == "garden") {
        for (int y = 0; y < 20; y++) {
            for (int x = 0; x < 20; x++) {
                mapa[y][x] = garden[y][x];
            }
        }
    }
    if (mapaElegido == "dungeon") {
        for (int y = 0; y < 20; y++) {
            for (int x = 0; x < 20; x++) {
                mapa[y][x] = dungeon[y][x];
            }
        }
    }
    if (mapaElegido == "desert") {
        for (int y = 0; y < 20; y++) {
            for (int x = 0; x < 20; x++) {
                mapa[y][x] = desert[y][x];
            }
        }
    }
    if (mapaElegido == "town") {
        for (int y = 0; y < 20; y++) {
            for (int x = 0; x < 20; x++) {
                mapa[y][x] = town[y][x];
            }
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////
    int recompensaActual = 0;
    for (; monedasObtenidas < 20; monedasObtenidas++) {
        monedaRandom(recompensaActual);
        recompensaActual++;
    }
    llenarGrafo(mapa);
    generarRecompensaSecretaAleatoria();
    tiempo = clock();
}

void leerEvento() {
    while (SDL_PollEvent(&event) != NULL)
    {
        switch (event.type)
        {
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_UP) {
                if (posy <= 0 || existeColisionArriba(posx, posy)) {
                    break;
                }
                else {
                    posxAnterior = posx;
                    posyAnterior = posy;
                    posy -= 32;
                    recompensaOcultaObtenida();
                }
            }
            if (event.key.keysym.sym == SDLK_DOWN) {
                if (posy > 576 || existeColisionAbajo(posx, posy)) {
                    break;
                }
                else {
                    posxAnterior = posx;
                    posyAnterior = posy;
                    posy += 32;
                    recompensaOcultaObtenida();
                }
            }
            if (event.key.keysym.sym == SDLK_LEFT) {
                if (posx <= 0 || existeColisionIzquierda(posx, posy)) {
                    break;
                }
                else {
                    posxAnterior = posx;
                    posyAnterior = posy;
                    posx -= 32;
                    recompensaOcultaObtenida();
                }
            }
            if (event.key.keysym.sym == SDLK_RIGHT) {
                if (posx > 576 || existeColisionDerecha(posx, posy)) {
                    break;
                }
                else {
                    posxAnterior = posx;
                    posyAnterior = posy;
                    posx += 32;
                    recompensaOcultaObtenida();
                }
            }
            break;
        }
    }
}

void escribirVidas() {
    rectFuente = { 0, 0 , 32, 32 };//Imagen del corazon

    if (vidas >= 3) {
        rectPanel = { 748,90,32,32 };
        SDL_RenderCopy(renderer, texturafondosPanel, &rectFuente, &rectPanel);
    }
    if (vidas >= 2) {
        rectPanel = { 700,90,32,32 };
        SDL_RenderCopy(renderer, texturafondosPanel, &rectFuente, &rectPanel);
    }
    if (vidas >= 1) {
        rectPanel = { 652,90,32,32 };
        SDL_RenderCopy(renderer, texturafondosPanel, &rectFuente, &rectPanel);
    }
    else {
        pantallaDerrota();
    }

}

void existecolisionenemigo() {
    if ((posx == posxSlime && posy == posySlime) || (posx == posxDemon && posy == posyDemon)) {

        vidas -= 1;
        posx = 608;
        posy = 608;
        Mix_PlayChannel(-1, chocaEnemigo, 0);
        if (c == 2) {
            Mix_PlayChannel(1, muertedefinitiva, 0);
        }
        c += 1;

        llaveObtenida = false;
    }
}

void movimientoenemigo() {
    Uint32 ticks = SDL_GetTicks();

    if (ticks % 500 <= 0) {
        if (mapaElegido == "town") {
            if (vueltaDemon) {
                if (posyDemon < 608) {
                    posyDemon += 32;
                }
                else {
                    vueltaDemon = false;
                }
            }
            else {
                if (posyDemon > 384) {
                    posyDemon -= 32;
                }
                else {
                    vueltaDemon = true;
                }
            }

            if (vueltaSlime) {
                if (posxSlime < 608) {
                    posxSlime += 32;
                }
                else {
                    vueltaSlime = false;
                }
            }
            else {
                if (posxSlime > 192) {
                    posxSlime -= 32;
                }
                else {
                    vueltaSlime = true;
                }
            }
        }

        else if (mapaElegido == "garden") {
            if (vueltaDemon) {
                if (posxDemon > 0) {
                    posxDemon -= 32;
                }
                else {
                    vueltaDemon = false;
                }
            }
            else {
                if (posxDemon < 608) {
                    posxDemon += 32;
                }
                else {
                    vueltaDemon = true;
                }
            }

            if (vueltaSlime) {
                if (posxSlime < 608) {
                    posxSlime += 32;
                }
                else {
                    vueltaSlime = false;
                }
            }
            else {
                if (posxSlime > 0) {
                    posxSlime -= 32;
                }
                else {
                    vueltaSlime = true;
                }
            }
        }

        else if (mapaElegido == "dungeon") {
            if (vueltaDemon) {
                if (posxDemon > 96) {
                    posxDemon -= 32;
                }
                else {
                    vueltaDemon = false;
                }
            }
            else {
                if (posxDemon < 448) {
                    posxDemon += 32;
                }
                else {
                    vueltaDemon = true;
                }
            }

            if (vueltaSlime) {
                if (posxSlime < 256) {
                    posxSlime += 32;
                }
                else {
                    vueltaSlime = false;
                }
            }
            else {
                if (posxSlime > 64) {
                    posxSlime -= 32;
                }
                else {
                    vueltaSlime = true;
                }
            }
        }

        else if (mapaElegido == "desert") {
            if (vueltaDemon) {
                if (posyDemon < 608) {
                    posyDemon += 32;
                }
                else {
                    vueltaDemon = false;
                }
            }
            else {
                if (posyDemon > 384) {
                    posyDemon -= 32;
                }
                else {
                    vueltaDemon = true;
                }
            }

            if (vueltaSlime) {
                if (posxSlime < 608) {
                    posxSlime += 32;
                }
                else {
                    vueltaSlime = false;
                }
            }
            else {
                if (posxSlime > 0) {
                    posxSlime -= 32;
                }
                else {
                    vueltaSlime = true;
                }
            }
        }
    }

}

void tiempoYSprites() {

    Uint32 ticks = SDL_GetTicks();
    Uint32 sprite = (ticks / 100) % 5;
    Uint32 tiempoDemon = (ticks / 100) % 7;
    SDL_Rect srcrect = { sprite * 32, tipopersonaje, 32, 32 };
    SDL_Rect dstrect = { posx, posy, 32, 32 };

    SDL_Rect rectFSlime = { sprite * 32, 0, 32, 32 };
    SDL_Rect rectDSlime = { posxSlime, posySlime, 32, 32 };

    SDL_Rect rectFDemon = { tiempoDemon * 32, 0, 32, 32 };
    SDL_Rect rectDDemon = { posxDemon, posyDemon, 32, 32 };

    SDL_RenderClear(renderer);
    pintarmapa(dstrect);
    panel();
    escribirVidas();
    existecolisionenemigo();
    movimientoenemigo();
    SDL_RenderCopy(renderer, texturapersonaje, &srcrect, &dstrect);

    SDL_RenderCopy(renderer, texturaSlime, &rectFSlime, &rectDSlime);
    SDL_RenderCopy(renderer, texturaDemon, &rectFDemon, &rectDDemon);
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}

void finalizar() {
    SDL_DestroyTexture(texturapersonaje);
    SDL_FreeSurface(image);
    Mix_FreeChunk(efectocofre);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

void menuPrincipal() {

    continuar = false;
    int op = 0;
    while (op!=5 && !continuar) {
        cout << "\n------------------------------------------------------------" << endl;
        cout << "                           BIENVENIDO" << endl;
        cout << "                ---------------------------------" << endl;
        cout << " 1) Cargar Partida" << endl;
        cout << " 2) Crear Partida" << endl;
        cout << " 3) Eliminar partida" << endl;
        cout << " 4) Ver ranking" << endl;
        cout << " 5) Salir" << endl;
        cout << "------------------------------------------------------------" << endl;
        cout << "\n Elija la opcion correspondiente" << endl;
        cout << " -> ";
        cin >> op;

        switch (op) {
        case 1:
            menuCargarPartidas();
            break;
        case 2:
            menuCrearPartida();
            break;
        case 3:
            menuEliminarPartidas();
            break;
        case 4:
            menuRankings();
            break;
        case 5:
            exit(1);
            break;
        default:
            cout << "Elija una opcion valida" << endl;
            break;
        }
    }
}

void menuEliminarPartidas() {
    int op;
    cout << "\n------------------------------------------------------------" << endl;
    cout << "                        ELIMINAR PARTIDA\n" << endl;
    cout << "                ---------------------------------" << endl;
    cout << " 1) Partida 1" << endl;
    cout << " 2) Partida 2" << endl;
    cout << " 3) Partida 3" << endl;
    cout << " 4) Partida 4" << endl;
    cout << " 5) Volver a menu principal" << endl;
    cout << "------------------------------------------------------------" << endl;
    cin >> op;
    ofstream archivo1, archivo2, archivo3, archivo4;
    switch (op) {
    case 1:
        archivo1.open("partida1.txt", ios::out);
        archivo1 << "";
        cout << "\n***** Se ha eliminado con exito la partida 1 *****\n";
        break;
    case 2:
        archivo2.open("partida2.txt", ios::out);
        archivo2 << "";
        cout << "\n***** Se ha eliminado con exito la partida 2 *****\n";
        break;
    case 3:
        archivo3.open("partida3.txt", ios::out);
        archivo3 << "";
        cout << "\n***** Se ha eliminado con exito la partida 3 *****\n";
        break;
    case 4:
        archivo4.open("partida4.txt", ios::out);
        archivo4 << "";
        cout << "\n***** Se ha eliminado con exito la partida 4 *****\n";
        break;
    case 5:
        break;
    default:
        break;
    }

}

void menuCargarPartidas() {
    string nombre1, nombre2, nombre3, nombre4;
    ifstream archivoLectura1, archivoLectura2, archivoLectura3, archivoLectura4;
    int opcionPartida;
    string informacion;
    string vidasS, posxS, posyS, posxSlimeS, posySlimeS, posxDemonS, posyDemonS, llaveObtenidaS, recompensaOcultaS, monedasS, tipoPersonajeS, mapaElegidoS;

    archivoLectura1.open("partida1.txt", ios::in); //Abrir el archivo en modo lectura
    getline(archivoLectura1, nombre1);     //Obtener nombre 1 del archivo
    archivoLectura1.close(); //cerrar archivo

    archivoLectura2.open("partida2.txt", ios::in);
    getline(archivoLectura2, nombre2);    //Obtener nombre 2 del archivo
    archivoLectura2.close();

    archivoLectura3.open("partida3.txt", ios::in);
    getline(archivoLectura3, nombre3);    //Obtener nombre 3 del archivo
    archivoLectura3.close();

    archivoLectura4.open("partida4.txt", ios::in);
    getline(archivoLectura4, nombre4);    //Obtener nombre 4 del archivo
    archivoLectura4.close();

    cout << "\n------------------------------------------------------------" << endl;
    cout << "Estas son las partidas disponibles: " << endl;
    cout << " 1) Partida 1: " << nombre1 << endl;
    cout << " 2) Partida 2: " << nombre2 << endl;
    cout << " 3) Partida 3: " << nombre3 << endl;
    cout << " 4) Partida 4: " << nombre4 << endl;
    cout << " 5) Volver al menu principal" << endl;
    cout << "\n------------------------------------------------------------" << endl;
    cout << "\n Elija la opcion correspondiente" << endl;
    cout << "-> ";
    cin >> opcionPartida;

    switch (opcionPartida)
    {
    case 1:
        archivoLectura1.open("partida1.txt", ios::in); //Abrir el archivo
        if (archivoLectura1.fail()) {
            cout << "No se abrio el archivo, puede ser que no exista, vuelva a intentarlo";
            menuPrincipal();
        }
         //mientras no sea el final del archivo
        getline(archivoLectura1, nombreJugador);
        getline(archivoLectura1, vidasS);
        getline(archivoLectura1, posxS);
        getline(archivoLectura1, posyS);
        getline(archivoLectura1, posxSlimeS);
        getline(archivoLectura1, posySlimeS);
        getline(archivoLectura1, posxDemonS);
        getline(archivoLectura1, posyDemonS);
        getline(archivoLectura1, llaveObtenidaS);
        getline(archivoLectura1, recompensaOcultaS);
        getline(archivoLectura1, monedasS);
        getline(archivoLectura1, tipoPersonajeS);
        getline(archivoLectura1, mapaElegido);

        vidas = atoi(vidasS.c_str());
        posx = atoi(posxS.c_str());
        posy = atoi(posyS.c_str());
        posxSlime = atoi(posxSlimeS.c_str());
        posySlime = atoi(posySlimeS.c_str());
        posxDemon = atoi(posxDemonS.c_str());
        posyDemon = atoi(posyDemonS.c_str());
        monedas = atoi(monedasS.c_str());
        monedasObtenidas = monedas / 32;
        tipopersonaje = atoi(tipoPersonajeS.c_str());

        if (llaveObtenidaS == "0") {
            llaveObtenida = false;
        }
        else {
            llaveObtenida = true;
        }
        if (recompensaOcultaS == "0") {
            recompensaOculta = false;
        }
        else {
            recompensaOculta = true;
        }

        inicializarCarga();

        break;
    case 2:
        archivoLectura2.open("partida2.txt", ios::in); //Abrir el archivo
        if (archivoLectura2.fail()) {
            cout << "No se abrio el archivo, puede ser que no exista, vuelva a intentarlo";
            menuPrincipal();
        }
        
        getline(archivoLectura2, nombreJugador);
        getline(archivoLectura2, vidasS);
        getline(archivoLectura2, posxS);
        getline(archivoLectura2, posyS);
        getline(archivoLectura2, posxSlimeS);
        getline(archivoLectura2, posySlimeS);
        getline(archivoLectura2, posxDemonS);
        getline(archivoLectura2, posyDemonS);
        getline(archivoLectura2, llaveObtenidaS);
        getline(archivoLectura2, recompensaOcultaS);
        getline(archivoLectura2, monedasS);
        getline(archivoLectura2, tipoPersonajeS);
        getline(archivoLectura2, mapaElegido);

        vidas = atoi(vidasS.c_str());
        posx = atoi(posxS.c_str());
        posy = atoi(posyS.c_str());
        posxSlime = atoi(posxSlimeS.c_str());
        posySlime = atoi(posySlimeS.c_str());
        posxDemon = atoi(posxDemonS.c_str());
        posyDemon = atoi(posyDemonS.c_str());
        monedas = atoi(monedasS.c_str());
        monedasObtenidas = monedas / 32;
        tipopersonaje = atoi(tipoPersonajeS.c_str());

        if (llaveObtenidaS == "0") {
            llaveObtenida = false;
        }
        else {
            llaveObtenida = true;
        }
        if (recompensaOcultaS == "0") {
            recompensaOculta = false;
        }
        else {
            recompensaOculta = true;
        }

        inicializarCarga();

        break;
    case 3:
        archivoLectura3.open("partida3.txt", ios::in); //Abrir el archivo
        if (archivoLectura3.fail()) {
            cout << "No se abrio el archivo, puede ser que no exista, vuelva a intentarlo";
            menuPrincipal();
        }
        
        getline(archivoLectura3, nombreJugador);
        getline(archivoLectura3, vidasS);
        getline(archivoLectura3, posxS);
        getline(archivoLectura3, posyS);
        getline(archivoLectura3, posxSlimeS);
        getline(archivoLectura3, posySlimeS);
        getline(archivoLectura3, posxDemonS);
        getline(archivoLectura3, posyDemonS);
        getline(archivoLectura3, llaveObtenidaS);
        getline(archivoLectura3, recompensaOcultaS);
        getline(archivoLectura3, monedasS);
        getline(archivoLectura3, tipoPersonajeS);
        getline(archivoLectura3, mapaElegido);

        vidas = atoi(vidasS.c_str());
        posx = atoi(posxS.c_str());
        posy = atoi(posyS.c_str());
        posxSlime = atoi(posxSlimeS.c_str());
        posySlime = atoi(posySlimeS.c_str());
        posxDemon = atoi(posxDemonS.c_str());
        posyDemon = atoi(posyDemonS.c_str());
        monedas = atoi(monedasS.c_str());
        monedasObtenidas = monedas / 32;
        tipopersonaje = atoi(tipoPersonajeS.c_str());

        if (llaveObtenidaS == "0") {
            llaveObtenida = false;
        }
        else {
            llaveObtenida = true;
        }
        if (recompensaOcultaS == "0") {
            recompensaOculta = false;
        }
        else {
            recompensaOculta = true;
        }

        inicializarCarga();

        break;
    case 4:
        archivoLectura4.open("partida4.txt", ios::in); //Abrir el archivo
        if (archivoLectura4.fail()) {
            cout << "No se abrio el archivo, puede ser que no exista, vuelva a intentarlo";
            menuPrincipal();
        }
        
        getline(archivoLectura4, nombreJugador);
        getline(archivoLectura4, vidasS);
        getline(archivoLectura4, posxS);
        getline(archivoLectura4, posyS);
        getline(archivoLectura4, posxSlimeS);
        getline(archivoLectura4, posySlimeS);
        getline(archivoLectura4, posxDemonS);
        getline(archivoLectura4, posyDemonS);
        getline(archivoLectura4, llaveObtenidaS);
        getline(archivoLectura4, recompensaOcultaS);
        getline(archivoLectura4, monedasS);
        getline(archivoLectura4, tipoPersonajeS);
        getline(archivoLectura4, mapaElegido);

        vidas = atoi(vidasS.c_str());
        posx = atoi(posxS.c_str());
        posy = atoi(posyS.c_str());
        posxSlime = atoi(posxSlimeS.c_str());
        posySlime = atoi(posySlimeS.c_str());
        posxDemon = atoi(posxDemonS.c_str());
        posyDemon = atoi(posyDemonS.c_str());
        monedas = atoi(monedasS.c_str());
        monedasObtenidas = monedas / 32;
        tipopersonaje = atoi(tipoPersonajeS.c_str());

        if (llaveObtenidaS == "0") {
            llaveObtenida = false;
        }
        else {
            llaveObtenida = true;
        }
        if (recompensaOcultaS == "0") {
            recompensaOculta = false;
        }
        else {
            recompensaOculta = true;
        }

        inicializarCarga();

        break;
    case 5:
        menuPrincipal();
        break;
    }
}

void menuCrearPartida() {

    cout << "Digite el nombre del Jugador: ";
    cin >> nombreJugador;
    continuar = true;

    ifstream archivolectura1;
    archivolectura1.open("partida1.txt", ios::in);                 //Abrir el archivo
    if (archivolectura1.fail()) {
        archivolectura1.close();
        ofstream archivoescritura;
        archivoescritura.open("partida1.txt", ios::out);
        archivoescritura << nombreJugador << endl;
        inicializar();
    }
    else {
        ifstream archivolectura2;
        archivolectura2.open("partida2.txt", ios::in);
        if (archivolectura2.fail()) {
            archivolectura2.close();
            ofstream archivoescritura;
            archivoescritura.open("partida2.txt", ios::out);
            archivoescritura << nombreJugador << endl;
            inicializar();
        }
        else {
            ifstream archivolectura3;
            archivolectura3.open("partida3.txt", ios::in);
            if (archivolectura3.fail()) {
                archivolectura3.close();
                ofstream archivoescritura;
                archivoescritura.open("partida3.txt", ios::out);
                archivoescritura << nombreJugador << endl;
                inicializar();
            }
            else {
                ifstream archivolectura4;
                archivolectura4.open("partida4.txt", ios::in);
                if (archivolectura4.fail()) {
                    archivolectura4.close();
                    ofstream archivoescritura;
                    archivoescritura.open("partida4.txt", ios::out);
                    archivoescritura << nombreJugador << endl;
                    inicializar();
                }else{
                    cout << "Se alcanzo el limite de partidas guardadas";
                    menuPrincipal();
                }
            }
        }
    }
}

bool guardarPartida() {
    ifstream archivolectura1;
    string nombreaux;

    archivolectura1.open("partida1.txt", ios::in);                 //Abrir el archivo
    
    getline(archivolectura1, nombreaux);
    if (nombreaux._Equal(nombreJugador)) {
        archivolectura1.close();
        ofstream archivoescritura;
        archivoescritura.open("partida1.txt", ios::out);
        archivoescritura << nombreJugador << endl;
        archivoescritura << vidas << endl;
        archivoescritura << posx << endl;
        archivoescritura << posy << endl;
        archivoescritura << posxSlime << endl;
        archivoescritura << posySlime << endl;
        archivoescritura << posxDemon << endl;
        archivoescritura << posyDemon << endl;
        archivoescritura << llaveObtenida << endl;
        archivoescritura << recompensaOculta << endl;
        archivoescritura << monedas << endl;
        archivoescritura << tipopersonaje << endl;
        archivoescritura << mapaElegido << endl;
        archivoescritura.close();
        return true;
    }
    archivolectura1.close();

    ifstream archivolectura2;
    archivolectura2.open("partida2.txt", ios::in);                 //Abrir el archivo
    
    getline(archivolectura2, nombreaux);
    if (nombreaux._Equal(nombreJugador)) {
        archivolectura2.close();
        ofstream archivoescritura;
        archivoescritura.open("partida2.txt", ios::out);
        archivoescritura << nombreJugador << endl;
        archivoescritura << vidas << endl;
        archivoescritura << posx << endl;
        archivoescritura << posy << endl;
        archivoescritura << posxSlime << endl;
        archivoescritura << posySlime << endl;
        archivoescritura << posxDemon << endl;
        archivoescritura << posyDemon << endl;
        archivoescritura << llaveObtenida << endl;
        archivoescritura << recompensaOculta << endl;
        archivoescritura << monedas << endl;
        archivoescritura << tipopersonaje << endl;
        archivoescritura << mapaElegido << endl;
        archivoescritura.close();
        return true;
    }
    archivolectura2.close();
    
    ifstream archivolectura3;
    archivolectura3.open("partida3.txt", ios::in);                 //Abrir el archivo
    
    getline(archivolectura3, nombreaux);
    if (nombreaux._Equal(nombreJugador)) {
        archivolectura3.close();
        ofstream archivoescritura;
        archivoescritura.open("partida3.txt", ios::out);
        archivoescritura << nombreJugador << endl;
        archivoescritura << vidas << endl;
        archivoescritura << posx << endl;
        archivoescritura << posy << endl;
        archivoescritura << posxSlime << endl;
        archivoescritura << posySlime << endl;
        archivoescritura << posxDemon << endl;
        archivoescritura << posyDemon << endl;
        archivoescritura << llaveObtenida << endl;
        archivoescritura << recompensaOculta << endl;
        archivoescritura << monedas << endl;
        archivoescritura << tipopersonaje << endl;
        archivoescritura << mapaElegido << endl;
        archivoescritura.close();
        return true;
    }
    archivolectura3.close();

    ifstream archivolectura4;
    archivolectura4.open("partida4.txt", ios::in);                 //Abrir el archivo
    
    getline(archivolectura4, nombreaux);
    if (nombreaux._Equal(nombreJugador)) {
        archivolectura4.close();

        ofstream archivoescritura;
        archivoescritura.open("partida4.txt", ios::out);
        archivoescritura << nombreJugador << endl;
        archivoescritura << vidas << endl;
        archivoescritura << posx << endl;
        archivoescritura << posy << endl;
        archivoescritura << posxSlime << endl;
        archivoescritura << posySlime << endl;
        archivoescritura << posxDemon << endl;
        archivoescritura << posyDemon << endl;
        archivoescritura << llaveObtenida << endl;
        archivoescritura << recompensaOculta << endl;
        archivoescritura << monedas << endl;
        archivoescritura << tipopersonaje << endl;
        archivoescritura << mapaElegido << endl;
        archivoescritura.close();
        return true;
    }
}

void menuRankings() {

}

int main(int argc, char** argv)
{
    menuPrincipal();
    
    while (!quit)
    {
        tiempoYSprites();
        leerEvento();
    }

    finalizar();
    return 0;
}