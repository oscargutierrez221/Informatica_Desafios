// Abrir, leer y escribir archivos

#include <iostream>
#include <fstream> // Librería para manejo de archivos
#include <cstdlib> // Librería para funciones de conversión
#include <cctype> // Para isprint
#include <cstring> // Para memcpy
#include <cstdio> // Para snprintf
using namespace std;

// Descomprimir un archivo usando RLE con codigo ASCII para ver caracter por caracter y descomprimirlo
// La idea es usar un ciclo for donde usaremos ASCII para ver caracter por caracter y descomprimirlo

void descomprimir_rle() {
    ifstream archivo("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto comprimido.txt", ios::binary);
    ofstream archivo2("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto descomprimido.txt", ios::binary);

    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir el archivo de entrada." << endl;
        return;
    }
    if (!archivo2.is_open()) {
        cout << "Error: No se pudo abrir el archivo de salida." << endl;
        return;
    }

    char ch;
    while (archivo.get(ch)) {
        if (isdigit(static_cast<unsigned char>(ch))) {
            // Tratar dígitos como caracteres normales si aparecen solos
            archivo2 << ch;
            continue;
        }

        // ch es el carácter a repetir
        int count = 0;
        bool has_count = false;

        while (true) {
            int peeked = archivo.peek();
            if (peeked == EOF || !isdigit(static_cast<unsigned char>(peeked))) {
                break;
            }
            has_count = true;
            char digit_ch;
            archivo.get(digit_ch);
            count = count * 10 + (digit_ch - '0');
        }

        if (!has_count) {
            count = 1;
        }

        // Escribir el carácter count veces
        for (int i = 0; i < count; ++i) {
            archivo2 << ch;
        }
    }

    archivo.close();
    archivo2.close();
}

void descomprimir_lz78() {
    ifstream archivo("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto comprimido.txt", ios::binary);
    if (!archivo.is_open()) {
        cout << "Archivo de entrada no encontrado. Creando uno vacío..." << endl;
        // se crea el archivo de entrada si no existe
        ofstream crear("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto comprimido.txt");
        crear.close();
        return; // No hay nada que descomprimir
    }

    // El archivo de salida se crea automáticamente si no existe
    ofstream archivo2("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto descomprimido.txt", ios::binary);

    // Diccionario dinámico de punteros a cadenas (arreglos char)
    const int MAX_ENTRADAS = 10000; // Aumentado para archivos más grandes
    // un arreglo estático apuntado hacia char
    char* diccionario[MAX_ENTRADAS];
    int tamanos[MAX_ENTRADAS];
    int total = 0; // cantidad de entradas en el diccionario

    while (true) {
        char high, low, c;
        if (!archivo.get(high)) break;
        if (!archivo.get(low)) break;
        if (!archivo.get(c)) break;

        int indice = (static_cast<unsigned char>(high) << 8) | static_cast<unsigned char>(low);

        // Validar que no excedamos el límite del diccionario
        if (total >= MAX_ENTRADAS) {
            cout << "Error: Se excedió el tamaño máximo del diccionario." << endl;
            break;
        }

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
        archivo2.write(nuevoStr, nuevoTam);

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
    ifstream archivo("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto comprimido.txt", ios::binary);
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir el archivo comprimido para determinación." << endl;
        return 0;
    }

    char ch;
    bool has_null_bytes = false;
    bool has_high_bytes = false;
    bool looks_like_rle = false;
    int count = 0;
    int consecutive_digits = 0;
    int char_digit_pairs = 0;  // Contador de pares carácter-dígito
    bool last_was_printable = false;

    while (archivo.get(ch) && count < 500) {  // Revisar más bytes para mejor detección
        unsigned char uch = static_cast<unsigned char>(ch);

        // Verificar si hay bytes nulos (común en LZ78)
        if (uch == 0) {
            has_null_bytes = true;
        }

        // Verificar si hay bytes altos (> 127) que son comunes en LZ78
        if (uch > 127) {
            has_high_bytes = true;
        }

        // Detectar patrones RLE: caracteres imprimibles seguidos de dígitos
        if (isdigit(uch)) {
            if (last_was_printable) {
                char_digit_pairs++;
            }
            consecutive_digits++;
            last_was_printable = false;
            if (consecutive_digits > 10) { // Más de 10 dígitos consecutivos es muy raro en RLE
                looks_like_rle = false;
            }
        } else {
            last_was_printable = isprint(uch) && uch != 127;
            consecutive_digits = 0;
        }

        count++;
    }

    archivo.close();

    // Si encontramos muchos pares carácter-dígito, probablemente es RLE
    if (char_digit_pairs > 5 && !has_null_bytes) {
        looks_like_rle = true;
    }

    // LZ78 típicamente tiene bytes nulos o altos debido a los índices de 16 bits
    if (has_null_bytes || (has_high_bytes && !looks_like_rle)) {
        return 2; // LZ78
    }

    // Si parece texto normal con patrones de números y letras, probablemente es RLE
    if (looks_like_rle) {
        return 1; // RLE
    }

    // Por defecto, intentar LZ78
    return 2;
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
    const char* filename = "/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Encriptado3.txt"; // Cambia este nombre para procesar otros archivos
    // Ejemplo: Llama a desencriptar con valores conocidos (ajusta n y K según el desafío)
    desencriptar(filename, 3, 0x5A); // Por ejemplo, n=3, K=0x5A - cámbialos por los correctos

    // Luego, descomprime automáticamente determinando el método
    descomprimir_auto();

    cout << "Proceso completado. Revisa el archivo 'Texto descomprimido.txt'" << endl;

    return 0;
}
