#include <gtk/gtk.h>
#include <string>
#include <stdbool.h>
#include <cmath>
#include <iostream>
#include <fstream>

#define MAX_X 43
#define MAX_Y 33
#define W_SOR 1.8
#define PRECISAO_CONVERGENCIA 0.00001

using namespace std;

typedef struct {
	
	int x;
	int y;

} posicao;

typedef struct{

	float matPot[MAX_Y][MAX_X];
	bool matBoolPot[MAX_Y][MAX_X];
	
} campoPot;

campoPot CampoPotencial;

void guardaMatriz() {

	ofstream arquivo;

	arquivo.open("dados.txt",ios::out | ios::trunc);
	arquivo.precision(3);

	if (!arquivo.is_open()) {

		printf("\ndeu ruim !\n\n");
		return;
	}

	for(int i=0; i<MAX_Y; i++) {
		for (int j=0; j<MAX_X;j++) {

			arquivo << CampoPotencial.matPot[i][j] << ' ';
		}

		arquivo << '\n';
	}

	arquivo << "\n\n";

	for(int i=0; i<MAX_Y; i++) {
		for (int j=0; j<MAX_X;j++) {

			arquivo << CampoPotencial.matBoolPot[i][j] << ' ';
		}

		arquivo << '\n';
	}

	arquivo.close();
}

void geraCampo(posicao objetivo, posicao obstaculos[], int numObstaculos){


	for (int i=0;i<numObstaculos;i++) {

		CampoPotencial.matPot[obstaculos[i].x][obstaculos[i].y] = 1;
		CampoPotencial.matBoolPot[obstaculos[i].x][obstaculos[i].y] = true;
	}

	for (int i = 0; i < MAX_Y; i++) { // Parede da esquerda
	    
		for (int j=0; j<MAX_X; j++) {

			if (i==0 || i==MAX_Y-1 || j==0 || j==MAX_X-1) {

				CampoPotencial.matPot[i][j] = 1;
				CampoPotencial.matBoolPot[i][j] = true;
			}

			else {

				CampoPotencial.matPot[i][j] = 0;
				CampoPotencial.matBoolPot[i][j] = false;
			}
		}
	}

	CampoPotencial.matBoolPot[objetivo.x][objetivo.y] = true;

	float resultTemp;
	bool convergiu = true;

	do {

		convergiu = true;

		for (int i = 1; i < MAX_Y - 1; i++) {

			for (int j = 1; j < MAX_X - 1; j++) {

				if (CampoPotencial.matBoolPot[i][j] == false) {
					
					resultTemp =W_SOR* (CampoPotencial.matPot[i + 1][j]
								+ CampoPotencial.matPot[i - 1][j]
								+ CampoPotencial.matPot[i][j + 1]
								+ CampoPotencial.matPot[i][j - 1]
								- 4 * CampoPotencial.matPot[i][j]) / 4
								+ CampoPotencial.matPot[i][j];

					if ((CampoPotencial.matPot[i][j] - resultTemp > PRECISAO_CONVERGENCIA) || (resultTemp - CampoPotencial.matPot[i][j] > PRECISAO_CONVERGENCIA))
						
						convergiu = false;
					
					CampoPotencial.matPot[i][j] = resultTemp;
				}

			}
		}
	} while (!convergiu);
}

double anguloCelula(int i, int j) {

	double resul = atan2((double) (CampoPotencial.matPot[i][j - 1] - CampoPotencial.matPot[i][j + 1]),
				   (double) (CampoPotencial.matPot[i - 1][j] - CampoPotencial.matPot[i + 1][j]));

	return resul;
}

void desenhaCampo(cairo_t *cr) {

	int larguraCanvas = 800;
	int alturaCanvas = 600;
	int proporcao = 3;

	int grossuraParede = 3;

	int larguraCampo = 150 * proporcao;
	int alturaCampo = 130 * proporcao + 12;

	int grossuraGrade = 1;
	double larguraCelula = 3.75 * proporcao;
	double alturaCelula = 4.34 * proporcao;
	int numCelulasEmX = MAX_X;
	int numCelulasEmY = MAX_Y; 

	//Seção que cria o campo.
	int xCampo = (larguraCanvas - larguraCampo)/2;
	int yCampo = (alturaCanvas - alturaCampo)/2;
	double rCampo = 107.0/255;
	double gCampo = 142.0/255;
	double bCampo = 35.0/255;

	cairo_set_source_rgb(cr, rCampo, gCampo, bCampo);
	cairo_rectangle (cr, xCampo,yCampo ,larguraCampo,alturaCampo);
	cairo_fill(cr);
	cairo_set_source_rgb(cr, 0, 0, 0);

	//Seção que cria a parede ao redor do campo.
	int xParede = xCampo - grossuraParede;
	int yParede = yCampo - grossuraParede;
	int larguraParede = grossuraParede + larguraCampo + grossuraParede;
	int alturaParede = grossuraParede + alturaCampo + grossuraParede;
	double rParede = 165.0/255;
	double gParede = 128.0/255;
	double bParede = 100.0/255;

	cairo_set_line_width(cr,grossuraParede*2);
	cairo_set_source_rgb(cr, rParede, gParede, bParede);
	cairo_rectangle (cr, xParede,yParede,larguraParede,alturaParede);
	cairo_stroke(cr);
	cairo_set_line_width(cr,2.0);
	cairo_set_source_rgb(cr, 0, 0, 0);

	//Seção que cria a grade sobre o campo.
	int xCelula = xCampo - larguraCelula;
	int yCelula = yCampo - alturaCelula;

	cairo_set_line_width(cr,grossuraGrade*2);

	for (int i=0; i< MAX_Y; i++) {

		for (int j=0; j<MAX_X; j++) {

			cairo_rectangle (cr, xCelula,yCelula,larguraCelula,alturaCelula);
			xCelula += larguraCelula;
		}

		xCelula = xCampo - larguraCelula;
		yCelula += alturaCelula;
	}

	cairo_stroke(cr);
	cairo_set_line_width(cr,2.0);

	//Seção que desenha obstaculos
	double rObstaculo = 139.0/255;
	double gObstaculo = 0.0/255;
	double bObsatculo = 0.0/255;
	xCelula = xCampo - larguraCelula;
	yCelula = yCampo - alturaCelula;

	cairo_set_source_rgb(cr, rObstaculo, gObstaculo, bObsatculo);

	for (int i=0; i< MAX_Y; i++) {

		for (int j=0; j< MAX_X; j++) {

			if (CampoPotencial.matBoolPot[i][j] == true && CampoPotencial.matPot[i][j] == 1) {
				
				cairo_rectangle (cr, xCelula + 1, yCelula + 1, larguraCelula - 2, alturaCelula - 2);
			}

			xCelula += larguraCelula;
		}

		xCelula = xCampo - larguraCelula;
		yCelula += alturaCelula;
	}

	cairo_fill(cr);
	cairo_set_source_rgb(cr, 0, 0, 0);

	//Seção que desenha o objetivo
	double rObjetivo = 212.0/255;
	double gObjetivo = 175.0/255;
	double bObjetivo = 55.0/255;
	xCelula = xCampo - larguraCelula;
	yCelula = yCampo - alturaCelula;

	cairo_set_source_rgb(cr, rObjetivo, gObjetivo, bObjetivo);

	for (int i=0; i< MAX_Y; i++) {

		for (int j=0; j<MAX_X; j++) {

			if (CampoPotencial.matBoolPot[i][j] == true && CampoPotencial.matPot[i][j] == 0) {

				cairo_rectangle (cr, xCelula + 1, yCelula + 1, larguraCelula - 2, alturaCelula - 2);
			}

			xCelula += larguraCelula;
		}

		xCelula = xCampo - larguraCelula;
		yCelula += alturaCelula;
	}

	cairo_fill(cr);
	cairo_set_source_rgb(cr, 0, 0, 0);
}

void desenhaPotenciais(cairo_t *cr) {

	double angulo;
	double larguraCelula = 3.75 * 3;
	double alturaCelula = 4.34 * 3;

	double temp1,temp2;

	int xCampo = 175;
	int yCampo = 99;

	int xCelula = xCampo;
	int yCelula = yCampo;

	cairo_surface_t *seta = cairo_image_surface_create_from_png("seta.png");
	int larguraSeta = cairo_image_surface_get_width(seta);
	int alturaSeta = cairo_image_surface_get_height(seta);

	cairo_set_source_surface(cr,seta,0,0);

	guardaMatriz();

	for (int i = 1; i < MAX_Y - 1; i++) {
	   
		for (int j=1; j < MAX_X - 1; j++) {

			if (CampoPotencial.matBoolPot[i][j] == false) {

				angulo = anguloCelula(i,j);

				cairo_move_to(cr,0,0);

				cairo_save(cr);
				cairo_translate(cr, xCelula + larguraCelula/2, yCelula + alturaCelula/2);
				cairo_rotate(cr,angulo);
				cairo_scale(cr,0.6,0.6);
				cairo_translate(cr,- larguraSeta/2.0, -alturaSeta/2.0);
				cairo_set_source_surface(cr,seta, 0, 0);
				cairo_paint(cr);
				cairo_restore(cr);

			}

			xCelula += larguraCelula;

		}

		xCelula = xCampo;
		yCelula += alturaCelula;
	}
}

void drawHandler(GtkWidget *area, cairo_t *cr, gpointer dados) {

	desenhaCampo(cr);
	desenhaPotenciais(cr);
}

GtkWidget* preparaJanela() {

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GError *erro = NULL;

	gtk_window_set_title (GTK_WINDOW (window), "Carrossel Caipira - Controle");
	gtk_window_set_icon_from_file(GTK_WINDOW(window), "icone.png", &erro);

	if (erro != NULL) {

		cout << erro->message << '\n';
		g_clear_error (&erro);
	}

	gtk_window_set_resizable(GTK_WINDOW(window),false);
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER_ALWAYS);
	g_signal_connect (window, "destroy", gtk_main_quit, NULL);

	return window;
}

GtkWidget* preparaConteudo() {

	GtkWidget* boxLayout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	GtkWidget* canvas = gtk_drawing_area_new();

	gtk_widget_set_size_request (canvas, 800, 600);
	g_signal_connect (G_OBJECT (canvas), "draw", G_CALLBACK (drawHandler), NULL);

	gtk_box_pack_start(GTK_BOX(boxLayout),canvas,0,0,0);

	return boxLayout;
}

int main(int argc, char *argv[]) {

	gtk_init(&argc, &argv);

	GtkWidget *window = preparaJanela();
	GtkWidget *layoutPrincipal = preparaConteudo();
	posicao objetivo;
	int numObstaculos=0;
	posicao obstaculos[numObstaculos];

	objetivo.x=22;
	objetivo.y=17;

	geraCampo(objetivo,obstaculos,numObstaculos);

	gtk_container_add(GTK_CONTAINER(window),layoutPrincipal);

	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}