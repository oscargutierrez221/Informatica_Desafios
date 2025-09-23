// Abrir, leer y escribir archivos

#include <iostream>
#include <fstream> // Librería para manejo de archivos
#include <cstdlib> // Librería para funciones de conversión
using namespace std;

int main(){
    ifstream archivo("Texto comprimido.txt"); // Abrir un archivo para lectura
    char linea[501]; // Buffer para almacenar cada línea (ajusta el tamaño si es necesario)
    while (archivo.getline(linea, 501)) {
        cout << linea << endl; // Mostrar la línea leída en la consola
    }
    archivo.close();

    // Escribir en un archivo usando char[]
    ofstream archivo2("Texto descomprimido.txt");
    const char texto[] = "Quiero probar si algo cambio.";
    archivo2 << texto;
    archivo2.close();

    return 0;
}