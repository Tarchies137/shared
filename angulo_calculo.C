#include <TFile.h>
#include <TTree.h>
#include <cmath>

void angulo_calculo() {
    // Parámetros
    const int N = 100; // Límite de los índices del ciclo for

    // Crear archivo ROOT y TTree
    TFile *file = new TFile("angulo.root", "RECREATE");
    TTree *tree = new TTree("Angulos", "Ángulos calculados");

    // Variables para almacenar los datos en el TTree
    int i, j, k, l;
    double angle;

    // Definir las ramas del TTree
    tree->Branch("i", &i, "i/I");
    tree->Branch("j", &j, "j/I");
    tree->Branch("k", &k, "k/I");
    tree->Branch("l", &l, "l/I");
    tree->Branch("angle", &angle, "angle/D");

    // Recorrer los índices con ciclos for
    for (i = -N; i <= N; ++i) {
        for (j = -N; j <= N; ++j) {
            for (k = -N; k <= N; ++k) {
                for (l = -N; l <= N; ++l) {
                    // Calcular la distancia euclidiana en el plano
                    double distance = sqrt((i - k) * (i - k) + (j - l) * (j - l));

                    // Calcular el ángulo usando la fórmula dada
                    if (N != 0) {
                        angle = atan((7.5 / N) * distance / 45.0); // Arcotangente
                    } else {
                        angle = 0.0; // Manejo de caso especial cuando N o distancia son 0
                    }

                    // Llenar el TTree con los valores
                    tree->Fill();
                }
            }
        }
    }

    // Escribir el TTree en el archivo y cerrarlo
    tree->Write();
    file->Close();

    delete file;

    std::cout << "Cálculo completado y guardado en 'angulo.root'" << std::endl;
}

