// test de triangulation
#include<iomanip>
#include<string>
#include"maillage_subdivision.hpp"

using namespace std;

//////////////////////////////////////////////////////
// partie d�formation
int partie_deformation()
{
	char nom_fichier_donnees[] = "DONNEES/cube.off";
	
	//// A - le mod�le initial est subdivis� 6 fois
	MaillageSubdivision M0(nom_fichier_donnees);
	for (int i=1; i<=6; i++)
		M0.subdivision_catmull_clark();
	Point3D B = M0.barycentre();
	
	// centrer le maillage
	M0.translater(-B);
	M0.write_geomview("res0.off.o");
	
	//// B - le mod�le initial est subdivis� k fois
	//// puis les coordonn�es d'un sommet sont modifi�es
	//// en enfin le maillage est subdivis� 6-k fois
	int k=0;
	Point3D C(1,1,1); // le point � modifier
	Point3D T(0.5,0.5,0.5); // la translation � effectuer
	
	// k subdivisions
	MaillageSubdivision M1(nom_fichier_donnees);
	for (int i=1; i<=k; i++)
		M1.subdivision_catmull_clark();
	
	// r�cup�rer le sommet s le plus proche de C	
	PtrSommet s = M1.sommet(C);
	// et le modifier
	s->translater(T);
	
	// 6-k subdivisions	
	for (int i=k+1; i<=6; i++)
		M1.subdivision_catmull_clark();
	
	// recentrer le maillage par rapport � M0
	M1.translater(-B);
	M1.write_geomview("res1.off.o");	
}

//////////////////////////////////////////////////////
// partie m�thodes subdivision
const char *nom_methode_subdiv[] = {
  "doo_sabin",
  "catmull_clark",
  "midedge",
  "sqrt3",
  "butterfly",
  "loop"
};

void partie_subdivision()
{
	char nom[200], nom_res[200];
	
	// choix du fichier � lire
	cout << "Nom du fichier OFF : ";
	cin >> nom;
	
	// cr�ation du maillage initial
	MaillageSubdivision M(nom);
	M.centrer();
	
	// choix de la m�thode
	int choix;
	cout << "Choix de la m�thode de subdivision :" << endl;
	cout << " Doo-Sabin     : 0" << endl;
	cout << " Catmull-Clark : 1" << endl;
	cout << " MidEdge       : 2" << endl;
	cout << " Sqrt(3)       : 3" << endl;
	cout << " Butterfly     : 4" << endl;
	cout << " Loop          : 5" << endl;
	do {
		cout << "Entrer votre choix : "; 
		cin >> choix;
	} while (choix<0 || choix>5);
	
	// triangulation si n�cessaire et sauvegarde du maillage initial
	if (choix>=3 && choix<=5) M.trianguler();
	sprintf(nom_res, "res%02d.off.o", 0);
	M.write_geomview(nom_res);
	
	sprintf(nom_res, "res%02d.off.o", 0);
	M.write_geomview(nom_res);
	unsigned int nb_subdivision;
	cout << "Entrer le nombre de subdivision : ";
	cin >> nb_subdivision;
	
	for (unsigned int i=1; i<=nb_subdivision; i++)
	{
		cout << " subdivision " << i << " ..." << endl;
		switch(choix)
		{
			case 0 : M.subdivision_doo_sabin()     ; break;
			case 1 : M.subdivision_catmull_clark() ; break;
			case 2 : M.subdivision_midedge()       ; break;
			case 3 : M.subdivision_sqrt3()         ; break;
			case 4 : M.subdivision_butterfly()     ; break;
			case 5 : M.subdivision_loop()          ; break;
		}
		sprintf(nom_res, "res-%s-%02d.off.o", nom_methode_subdiv[choix], i);
		M.write_geomview(nom_res);
	}	
}


int main()
{
	partie_subdivision();
//	partie_deformation();
	return 0;	
}
