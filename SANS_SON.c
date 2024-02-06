#include<stdio.h>
#include<string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include<time.h>
#include <SDL2/SDL_mixer.h>
#include <dirent.h>

#define SCREEN_WIDTH    1600
#define SCREEN_HEIGHT   800
#define TAILLE_LETTRE   30
#define TAILLE_DIC     23061
#define Jaune          {255,255,0}
#define White          {255,255,255}
#define Violet          {127,0,255}
#define MAX_LEN              256
#define BRUITAGE       "audio/bruitage/"
#define RED             {255,0,0}

// fonction permettant d'afficher le game over
//peut être modifier pour afficher n'importe quel texte ponctuel
void displaygameover(TTF_Font * font,SDL_Renderer* renderer){
 SDL_Color cred=RED;
 SDL_Rect rect={550,250,500,300};
 SDL_SetRenderDrawColor(renderer,cred.r,cred.g,cred.b, 255);
 SDL_Surface *textSurface = TTF_RenderText_Solid(font, "GAME OVER",cred );
 SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
 SDL_RenderCopy(renderer,textTexture, NULL, &rect);
 SDL_FreeSurface(textSurface); 
 SDL_DestroyTexture(textTexture);
}
void displayname(TTF_Font * font,SDL_Renderer* renderer){
 SDL_Color cred=RED;
 SDL_Rect rect={550,100,500,300};
 SDL_SetRenderDrawColor(renderer,cred.r,cred.g,cred.b, 255);
 SDL_Surface *textSurface = TTF_RenderText_Solid(font, "WORD WAR",cred );
 SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
 SDL_RenderCopy(renderer,textTexture, NULL, &rect);
 SDL_FreeSurface(textSurface); 
 SDL_DestroyTexture(textTexture);
}
// Structure pour représenter un bouton
typedef struct {
    SDL_Rect rect;      // Rectangle délimitant le bouton
    SDL_Color color;    // Couleur du bouton
    SDL_bool isPressed; // Indique si le bouton est enfoncé
    char *text;         // Texte du bouton
    TTF_Font *font;     // front du bouton
    
} Button;

// fonction pour faire le bouton
void drawButton(SDL_Renderer *renderer, Button *button) {
    // Dessine le rectangle du bouton
    SDL_SetRenderDrawColor(renderer, button->color.r, button->color.g, button->color.b, 255);
    if (button->isPressed) {
        SDL_SetRenderDrawColor(renderer, button->color.r / 2, button->color.g / 2, button->color.b / 2, 255);
    }
    SDL_RenderFillRect(renderer, &button->rect);

    // Dessine le texte au centre du bouton
    SDL_Color textColor = {0, 0, 0, 0}; // Couleur du texte (blanc ici)
    SDL_Surface *textSurface = TTF_RenderText_Solid(button->font, button->text, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);
    
    // Rectangle du bouton 
    SDL_Rect textRect = {
        button->rect.x + (button->rect.w - textWidth) / 2,
        button->rect.y + (button->rect.h - textHeight) / 2,
        textWidth,
        textHeight
    };

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

// Initialise un bouton avec les paramètres spécifiés
Button createButton(int x, int y, int width, int height, SDL_Color col, const char *text, TTF_Font *font) {
    Button button;
    button.rect.x = (x)/2 -width/2;
    button.rect.y = (y)/2 -height/2;
    button.rect.w = width;
    button.rect.h = height;
    button.color=col;
    button.isPressed = SDL_FALSE;
    button.text = strdup(text);
    button.font = font;
   
    return button;
}

// Donne un nombre aléatoire
int randmot(){
return rand()%TAILLE_DIC;
}

int main(int argc, char* argv[]) {
    // Initialisation de la SDL video
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Erreur d'initialisation de la SDL : %s", SDL_GetError());
        return 1;
    }
    // Initialisation de la SDL Son
    // (!!! notre version final ne comporte pas de son suite au problème de lague rencontré)
    if (SDL_Init(SDL_INIT_AUDIO)<0){
        printf("Erreur lors de l'initialisation de la SDL : %s\n", SDL_GetError());
        return -1;
    }

    // Initialisation du mixage du son de la SDL
    if (Mix_OpenAudio(96000, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur initialisation SDL_mixer : %s", Mix_GetError());
        SDL_Quit();
        return -1;
    }
    
    
    Mix_AllocateChannels(2); // Allouer 2 cannaux 
    Mix_Volume(0, MIX_MAX_VOLUME/2); // Mets le son a 50% en volume pour le premier cannal
    Mix_Volume(1, MIX_MAX_VOLUME / 5); // Mets le son a 20% en volume pour le deuxièmme cannal
    
    // initialisation d'une voie d'écoute
   /* int channel0=0;

    // Initialisation de toutes les musiques
    
    Mix_Chunk* fondplay = Mix_LoadWAV("audio/bruitage/Un.mp3");
    Mix_Chunk* fondmenu = Mix_LoadWAV("audio/bruitage/on-the-road-to-the-eighties_59sec-177566.mp3");
    Mix_Chunk* debutjeu = Mix_LoadWAV("audio/bruitage/game-bonus-144751.mp3");
    Mix_Chunk* tapemot = Mix_LoadWAV("audio/bruitage/2007.mp3");
    Mix_Chunk* succesmot = Mix_LoadWAV("audio/bruitage/2018.mp3");
    Mix_Chunk* echecmot = Mix_LoadWAV("audio/bruitage/2019.mp3");
    Mix_Chunk* pertevie = Mix_LoadWAV("audio/bruitage/2020.mp3");
    Mix_Chunk* gameover = Mix_LoadWAV("audio/bruitage/gameover.mp3");
    */

    // Initialisation de la police
    if (TTF_Init() != 0) {
        SDL_Log("Erreur d'initialisation de SDL_ttf : %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Initialisation de la fenêtre
    SDL_Window* window = SDL_CreateWindow("WORDS WAR", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window) {
        SDL_Log("Erreur de création de la fenêtre : %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Initialisation du rendu
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Erreur de création du renderer : %s", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Charger une police TrueType
    TTF_Font* font = TTF_OpenFont("Police_OPENUP/arial.ttf", 100);
    if (!font) {
        SDL_Log("Erreur de chargement de la police : %s", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    
    // Création du bouton PLAY
    TTF_Font* font2 = TTF_OpenFont("Police_OPENUP/arial.ttf", 30);
    SDL_Color color =White;
    Button myButton = createButton(SCREEN_WIDTH, SCREEN_HEIGHT, 200, 80,color,"PLAY",font2);
    
    // Ouverture de notre dictionnaire avec tous nos mots
    FILE* file=fopen("dico.txt","r");
    if (!file){
        printf("dictionnaire impossible à ouvrir");
        return 1;
    }
    char mot[MAX_LEN];
    char* mots[TAILLE_DIC];
    size_t nb=0;
    while(fgets(mot,MAX_LEN, file)){
        mot[strcspn(mot, "\r")] = '\0';
        mots[nb]=malloc(sizeof(char)*(strlen(mot)+1));
        strcpy(mots[nb],mot);
        nb++;
    }
    fclose(file);
    
    // Initialisation de nos valeurs de départ
    int val=4;         // nombre de mot
    int allword=val+1; // paramètre pour utilisateur pour taper au clavier 
    int score=val+2;   // paramètre pour utilisateur score actuel
    srand(time(NULL)); // permet l'aléatoire dans notre noyau

    // Couleur du texte (jaune)
    SDL_Color cyellow = Jaune;  
    SDL_Color cwhite = White;
    SDL_Color cviolet = Violet;
    
    // temps actuelle
    Uint32 currentTime=SDL_GetTicks();
    
    // mot pour utilisateur qui tape
    char mottape[MAX_LEN]="";

    // Liste de mot, de surface, de texture, de valeur d'affichage, de hitbox
    char*Lmot[score];
    SDL_Surface* Lsurface[score];
    SDL_Texture* Lmessage[score];
    int LM[val];
    SDL_Rect LRect[score];

 // On laisse la boucle jeu
 int game =0;
 while(!game){
    // on initialise ou réinitialise les valeurs du jeu (point, vitesse, mot,...)
    int point=0;
    int vitesse=1;
    int craci=0;
    mottape[0]='\0';
    // on remplit les listes initiasées précédemment
    for (size_t i = 0; i < val; i++)
        {
            Lmot[i]=mots[randmot()];
            Lsurface[i]=TTF_RenderUTF8_Solid(font,Lmot[i], cyellow);
            Lmessage[i]=SDL_CreateTextureFromSurface(renderer,Lsurface[i]);
            LM[i]=0;
            LRect[i].x=SCREEN_WIDTH-strlen(Lmot[i])* TAILLE_LETTRE;
            LRect[i].y= (i+1)*100;
            LRect[i].w=strlen(Lmot[i])* TAILLE_LETTRE;
            LRect[i].h=100;   
        }
    Lmot[allword]=mottape;
    Lsurface[allword]=TTF_RenderUTF8_Solid(font,Lmot[allword],cwhite);
    Lmessage[allword]=SDL_CreateTextureFromSurface(renderer,Lsurface[allword]);
    LRect[allword].w=strlen(Lmot[allword])* TAILLE_LETTRE;
    LRect[allword].h=100; 
    LRect[allword].x=SCREEN_WIDTH/2-(strlen(Lmot[allword])* TAILLE_LETTRE)/2;
    LRect[allword].y= SCREEN_HEIGHT-100;

    char scoreText[50];
    sprintf(scoreText, "Score: %d", point);
    Lmot[score]=scoreText;
    Lsurface[score]=TTF_RenderUTF8_Solid(font,Lmot[score],cviolet);
    Lmessage[score]=SDL_CreateTextureFromSurface(renderer,Lsurface[score]);
    LRect[score].w=strlen(Lmot[score])* TAILLE_LETTRE;
    LRect[score].h=100; 
    LRect[score].x=SCREEN_WIDTH-(strlen(Lmot[score])* TAILLE_LETTRE);
    LRect[score].y= 0;

    // on crée les sous-boucles du menu et du jeu
    SDL_Event emenu;
    int menu = 0;
    SDL_Event event;
    int quit = 0;

    // On lance la musique du fond de menu
   // Mix_PlayChannel(0, fondmenu, -1);
        while(!menu){

            while (SDL_PollEvent(&emenu)) { // tous les évènements
                if (emenu.type == SDL_QUIT) { // event de sortie
                    menu = 1;
                    quit=1;
                    game=1;
                } else if (emenu.type == SDL_KEYDOWN) { // lorsque touche clavier
                    if (emenu.key.keysym.sym == SDLK_ESCAPE) { // lorsque touche escape
                        menu = 1;
                        quit=1;
                        game=1;}
                }
                // lorsqu'on appuie sur le bouton PLAY
                else if (emenu.type == SDL_MOUSEBUTTONDOWN) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                
                    // Vérifie si le clic de la souris est à l'intérieur du bouton
                    if (mouseX >= myButton.rect.x && mouseX <= myButton.rect.x + myButton.rect.w &&
                        mouseY >= myButton.rect.y && mouseY <= myButton.rect.y + myButton.rect.h) {
                        myButton.isPressed = SDL_TRUE; 
                        menu=1;
                        //  On lance la musique du début du jeu
                      //  Mix_PlayChannel(0, debutjeu, 0);
                        //SDL_Delay(3000);
                        myButton.isPressed = SDL_FALSE;
                        
                    }
                } 
                // Sinon on n'appuie pas sur le bouton PLAY, on ne fait rien
                else if (emenu.type == SDL_MOUSEBUTTONUP) {
                    myButton.isPressed = SDL_FALSE;
                
                }}
            
            // On initialise le rendu et on l'actualise
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            displayname(font,renderer);
            drawButton(renderer, &myButton);
            SDL_RenderPresent(renderer); 
            }

    // Met la musique en fond du jeu en marche
   // Mix_PlayChannel(0, fondplay, -1);
      
        while (!quit)
        {       // On initialise le temps courant.
                currentTime+=1;
                if (currentTime>=1000){
                    currentTime=0;
                }
            while (SDL_PollEvent(&event)) { // tous les évènements
                if (event.type == SDL_QUIT) { // event croix
                    quit = 1;
                    game=1;
                } else if (event.type == SDL_KEYDOWN) { // lorsque touche clavier
                    if (event.key.keysym.sym == SDLK_ESCAPE) { // lorsque touche escape
                        quit = 1;}
                else if (event.key.keysym.sym == SDLK_DELETE || event.key.keysym.sym == SDLK_BACKSPACE){ // touches supprimer ou backspace
                    int longueurmottape=strlen(mottape);
                    // On retire un terme du mot que l'utilisateur est en train de taper.
                    if (longueurmottape > 0){
                    // cas spécial si un caractère est tapé, on enlève 2 caractères
                    if( 0> mottape[longueurmottape-1] || 127 < mottape[longueurmottape-1] ){
                            mottape[longueurmottape - 2] = '\0';}
                    else{

                            mottape[longueurmottape - 1] = '\0';}
                    }
                    }
                }else if (event.key.keysym.sym == SDLK_RETURN) { // touches espace ou retour ligne
                    for (size_t i = 0; i < val; i++){
                        if (!strcmp(mottape,Lmot[i])) // vérification que mot est correct
                {
                        LM[i]=1;
                        point+=strlen(Lmot[i]);
                        // On lance la musique de mot lors de succès
                     //   Mix_PlayChannel(1, succesmot, 0);
                        if (point%10){
                            vitesse+=1;// on augmente la vitesse lorsque notre nombre de point est de modulo 10
                        }
                    }
                    }
                    mottape[0]='\0'; // On réinitialise le mot
                
                    
                }else if (event.type == SDL_TEXTINPUT) { // n'importe quelles touches du clavier transcrit 
                    // Afficher le caractère clavier (prend en charge les majuscules, les accents, etc.)
                    if (strlen(mottape)<=MAX_LEN){
                    strncat(mottape,event.text.text,sizeof(mottape)-strlen(mottape)-1);
                    // On lance la musique lorsque l'utilisateur tape au clavier
                  //  Mix_PlayChannel(1, tapemot, 0);
    
                    }
                }
                
            }

            // On actualise le mot que l'utilisateur est en train de taper.
            Lmot[allword]=mottape;
            Lsurface[allword]=TTF_RenderUTF8_Solid(font,Lmot[allword],cwhite);
            Lmessage[allword]=SDL_CreateTextureFromSurface(renderer,Lsurface[allword]);           
            LRect[allword].w=strlen(Lmot[allword])* TAILLE_LETTRE;
            LRect[allword].x=SCREEN_WIDTH/2-(strlen(Lmot[allword])* TAILLE_LETTRE)/2;
    
            // On actualise le score.
            sprintf(scoreText, "POINT: %d", point);
            Lmot[score]=scoreText;
            Lsurface[score]=TTF_RenderUTF8_Solid(font,Lmot[score],cviolet);
            Lmessage[score]=SDL_CreateTextureFromSurface(renderer,Lsurface[score]);
            LRect[score].w=strlen(Lmot[score])* TAILLE_LETTRE;
            LRect[score].h=100; 
            LRect[score].x=SCREEN_WIDTH-(strlen(Lmot[score])* TAILLE_LETTRE); 

            // Effacer l'écran
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            
            // On affiche le score et le mot tapé.
            SDL_RenderCopy(renderer,Lmessage[allword], NULL, &LRect[allword]);
            SDL_RenderCopy(renderer,Lmessage[score], NULL, &LRect[score]);

            // Met à jour l'affichage
            // Afficher le texte
            for (size_t i = 0; i < val; i++)
            {
                if (!LM[i]){
                SDL_RenderCopy(renderer,Lmessage[i], NULL, &LRect[i]);
                if ((currentTime*vitesse)%(strlen(Lmot[i])+1)==0) { 
                    LRect[i].x-=2;
                }
                if (LRect[i].x<=0){
                    LM[i]=1;
                    quit=1;}
                }else {
                    LM[i]=0;
                    Lmot[i]=mots[randmot()];
                    Lsurface[i]=TTF_RenderUTF8_Solid(font,Lmot[i], cyellow);
                    Lmessage[i]=SDL_CreateTextureFromSurface(renderer,Lsurface[i]);
                    LRect[i].x=SCREEN_WIDTH-strlen(Lmot[i]) * TAILLE_LETTRE;
                    LRect[i].w=strlen(Lmot[i]) * TAILLE_LETTRE;
                    } 
            }
             //  Mettre à jour l'écran    
             SDL_RenderPresent(renderer);  
    }
    // On affiche le game over et on actualise. On attend enfin 3s pour le rendu et la boucle recommence du début
    if (!game){
    displaygameover(font,renderer);
   // Mix_PlayChannel(0,gameover,0);
    SDL_RenderPresent(renderer);
    SDL_Delay(3000);
    }
 }


// Nettoyage des listes et tous les autres éléments de la SDL (police, son, fenêtre,...)
for (size_t i = 0; i < score; i++)
    {
    SDL_FreeSurface(Lsurface[i]); 
    SDL_DestroyTexture(Lmessage[i]);
    }
    
// Libére la mémoire allouer pour le son

TTF_CloseFont(font);
SDL_DestroyRenderer(renderer);
SDL_DestroyWindow(window);
TTF_Quit();
Mix_CloseAudio();
SDL_Quit();
return 0;
}