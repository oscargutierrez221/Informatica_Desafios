// Abrir, leer y escribir archivos

#include <iostream>
#include <fstream> // Librería para manejo de archivos
#include <cstdlib> // Librería para funciones de conversión
#include <cctype> // Para isprint
#include <cstring> // Para memcpy
#include <cstdio> // Para snprintf
using namespace std;

// Descomprimir un archivo usando RLE con codigo ASCII para ver caracter por caracter y descomprimirlo

void descomprimir_rle() {
    ifstream archivo("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto comprimido.txt");
    ofstream archivo2("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto descomprimido.txt");

    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir el archivo de entrada." << endl;
        return;
    }
    if (!archivo2.is_open()) {
        cout << "Error: No se pudo abrir el archivo de salida." << endl;
        return;
    }

    char linea[10001];
    while (archivo.getline(linea, 10001)) {
        int i = 0;
        while (linea[i] != '\0') {
            int count = 0;
            if (linea[i] >= '0' && linea[i] <= '9') {
                while (linea[i] >= '0' && linea[i] <= '9') {
                    count = count * 10 + (linea[i] - '0');
                    i++;
                }
            } else {
                count = 1;
            }
            char caracter = linea[i];
            for (int j = 0; j < count; ++j) {
                archivo2 << caracter;
            }
            i++;
        }
        archivo2 << endl;
    }
    archivo.close();
    archivo2.close();
}

void descomprimir_lz78() {
    ifstream archivo("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto comprimido.txt", ios::binary);
    if (!archivo.is_open()) {
        cout << "Archivo de entrada no encontrado. Creando uno vacío..." << endl;
        // se crea el archivo de entrada si no exciste
        ofstream crear("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto comprimido.txt");
        crear.close();
        return; // No hay nada que descomprimir
    }

    // El archivo de salida se crea automáticamente si no existe
    ofstream archivo2("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto descomprimido.txt");

    // Diccionario dinámico de punteros a cadenas (arreglos char)
    const int MAX_ENTRADAS = 1000;
    // un arreglo estatico apuntado hacia char
    char* diccionario[MAX_ENTRADAS];
    int tamanos[MAX_ENTRADAS];
    int total = 0; // cantidad de entradas en el diccionario

    while (true) {
        char high, low, c;
        if (!archivo.get(high)) break;
        if (!archivo.get(low)) break;
        if (!archivo.get(c)) break;

        int indice = (static_cast<unsigned char>(high) << 8) | static_cast<unsigned char>(low);

        // Construir la nueva cadena S = prefijo + c
        int nuevoTam = 1;
        char* nuevoStr;

        if (indice == 0) {
            // Solo el carácter
            nuevoStr = new char[2];
            nuevoStr[0] = c;
            nuevoStr[1] = '\0';
        } else {
            if (indice > total) {
                cout << "Error: Índice inválido en el diccionario." << endl;
                break;
            }
            // Copiar la cadena del diccionario y añadir el carácter
            nuevoTam = tamanos[indice - 1] + 1;
            nuevoStr = new char[nuevoTam + 1];
            memcpy(nuevoStr, diccionario[indice - 1], tamanos[indice - 1]);
            nuevoStr[nuevoTam - 1] = c;
            nuevoStr[nuevoTam] = '\0';
        }

        // Escribir la nueva cadena en el archivo de salida
        archivo2 << nuevoStr;

        // Guardar en el diccionario
        diccionario[total] = nuevoStr;
        tamanos[total] = nuevoTam;
        total++;
    }

    // Liberar memoria
    for (int i = 0; i < total; i++) {
        delete[] diccionario[i];
    }

    archivo.close();
    archivo2.close();
}

void desencriptar(const char* filename, int n, unsigned char K) {
    ifstream archivo(filename, ios::binary);
    ofstream archivo2("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto comprimido.txt", ios::binary);

    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir el archivo de entrada." << endl;
        return;
    }
    if (!archivo2.is_open()) {
        cout << "Error: No se pudo abrir el archivo de salida." << endl;
        return;
    }

    char byte;
    while (archivo.get(byte)) {
        unsigned char ub = static_cast<unsigned char>(byte);
        ub ^= K;
        ub = (ub >> n) | (ub << (8 - n));
        archivo2.put(static_cast<char>(ub));
    }
    archivo.close();
    archivo2.close();
}

int determinar_metodo() {
    ifstream archivo("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto comprimido.txt");
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir el archivo comprimido para determinación." << endl;
        return 0;
    }

    char ch;
    bool is_printable = true;
    int count = 0;
    while (archivo.get(ch) && count < 100) {  // Revisar solo los primeros 100 bytes para eficiencia
        unsigned char uch = static_cast<unsigned char>(ch);
        if (uch == 0 || (!isprint(uch) && uch != '\n' && uch != '\r' && uch != '\t')) {
            is_printable = false;
            break;
        }
        count++;
    }
    archivo.close();

    if (is_printable) {
        return 1; // RLE (texto printable como dígitos y caracteres)
    } else {
        return 2; // LZ78 (binario con posibles bytes no printables)
    }
}

void descomprimir_auto() {
    int metodo = determinar_metodo();
    switch (metodo) {
    case 1:
        cout << "Método detectado: RLE" << endl;
        descomprimir_rle();
        break;
    case 2:
        cout << "Método detectado: LZ78" << endl;
        descomprimir_lz78();
        break;
    default:
        cout << "No se pudo determinar el método de compresión." << endl;
        break;
    }
}

int main() {
    const char* filename = "/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Encriptado1.txt"; // Es necesario cambiar esta ruta para procesar otros archivos
    desencriptar(filename, 3, 0x5A); // Por ejemplo, n=3, K=0x5A

    descomprimir_auto();

    return 0;
}
