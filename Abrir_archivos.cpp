// Abrir, leer y escribir archivos

#include <iostream>
#include <fstream> // Librería para manejo de archivos
#include <cstdlib> // Librería para funciones de conversión
using namespace std;

//    void archivos(){
//        ifstream archivo("Texto comprimido.txt"); // Abrir un archivo para lectura
//        char linea[501]; // Buffer para almacenar cada línea (ajusta el tamaño si es necesario)
//        while (archivo.getline(linea, 501)) {
//            cout << linea << endl; // Mostrar la línea leída en la consola
//        }
//        archivo.close();
//
//        // Escribir en un archivo usando char[]
//        ofstream archivo2("Texto descomprimido.txt");
//        const char texto[] = "Quiero probar si algo cambio.";
//        archivo2 << texto;
//        archivo2.close();
//    }

// Descomprimir un archivo usando RLE con codigo ASCII para ver caracter por caracter y descomprimirlo
// La idea es usar un ciclo for donde usaremos ASCII para ver caracter por caracter y descomprimirlo


void descomprimir_rle() {
    ifstream archivo("Texto de comprimido.txt");
    ofstream archivo2("Texto descomprimido.txt");

    char linea[501];
    while (archivo.getline(linea, 501)) {
        int i = 0;
        while (linea[i] != '\0') {
            int count = 0;
            // Leer todos los dígitos del número
            while (linea[i] >= '0' && linea[i] <= '9') {
                count = count * 10 + (linea[i] - '0');
                i++;
            }
            char caracter = linea[i]; // El carácter a repetir
            for (int j = 0; j < count; ++j) {
                archivo2 << caracter;
            }
            i++; // Avanzar al siguiente grupo
        }
        archivo2 << endl;
    }
    archivo.close();
    archivo2.close();
}

int main(){
    descomprimir_rle();
    return 0;
}