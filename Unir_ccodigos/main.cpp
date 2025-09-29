// Abrir, leer y escribir archivos.
#include <iostream>
#include <fstream> // Esta librería es para manejar archivos, como abrir y cerrar.
#include <cstdlib> // Para funciones de conversión, no estoy seguro de todo pero lo necesito.
#include <cctype> // Para isprint, que verifica si un carácter se puede imprimir.
#include <cstring> // Para memcpy, que copia memoria.
#include <cstdio> // Para snprintf, aunque no lo uso mucho aquí.
using namespace std;

// Función para descomprimir un archivo usando RLE. RLE significa Run-Length Encoding, que es como comprimir repitiendo caracteres.
// Estoy usando un ciclo para ver cada carácter en ASCII y descomprimirlo. Espero que funcione, es mi primer intento.

void descomprimir_rle() {
    ifstream archivo_entrada("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto comprimido.txt", ios::binary); // Abro el archivo de entrada en modo binario porque puede tener cualquier byte.
    ofstream archivo_salida("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto descomprimido.txt", ios::binary); // Y este es para escribir el descomprimido.

    if (!archivo_entrada.is_open()) { // Verifico si se abrió bien, si no, muestro un error.
        cout << "Error: No se pudo abrir el archivo de entrada." << endl;
        return; // Salgo de la función si hay error.
    }
    if (!archivo_salida.is_open()) {
        cout << "Error: No se pudo abrir el archivo de salida." << endl;
        return; // Igual aquí.
    }

    char caracter; // Variable para guardar cada carácter que leo.
    while (archivo_entrada.get(caracter)) { // Leo carácter por carácter hasta el final.
        if (!isalnum(static_cast<unsigned char>(caracter))) { // Si no es alfanumérico, lo ignoro. Aprendí que static_cast es para convertir tipos.
            continue; // Sigo con el siguiente.
        }

        // Ahora, el caracter es el que se repite.
        int cantidad = 0; // Cuántas veces se repite.
        bool tiene_cantidad = false; // Bandera para saber si hay un número después.

        while (true) { // Ciclo para leer los dígitos que indican la cantidad.
            int vistazo = archivo_entrada.peek(); // Miro el siguiente sin leerlo.
            if (vistazo == EOF || !isdigit(static_cast<unsigned char>(vistazo))) { // Si no es dígito o fin de archivo, salgo.
                break;
            }
            tiene_cantidad = true;
            char digito_caracter;
            archivo_entrada.get(digito_caracter);
            cantidad = cantidad * 10 + (digito_caracter - '0'); // Construyo el número, como en matemáticas básicas.
        }

        if (!tiene_cantidad) { // Si no había número.
            int vistazo = archivo_entrada.peek();
            if (vistazo != EOF && isalnum(static_cast<unsigned char>(vistazo))) {
                char cantidad_caracter;
                archivo_entrada.get(cantidad_caracter);
                cantidad = static_cast<unsigned char>(cantidad_caracter); // Uso el byte como cantidad.
            } else {
                cantidad = 1; // Si nada, solo una vez.
            }
        }

        // Ahora escribo el carácter tantas veces como la cantidad.
        for (int i = 0; i < cantidad; ++i) { // Ciclo for simple, lo aprendí en clase.
            archivo_salida << caracter;
        }
    }

    archivo_entrada.close(); // Cierro los archivos, importante para no dejarlos abiertos.
    archivo_salida.close();
}

void descomprimir_lz78() { // Esta es para LZ78, que es más complicada. LZ78 usa un diccionario para comprimir.
    ifstream archivo_entrada("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto comprimido.txt", ios::binary);
    if (!archivo_entrada.is_open()) {
        cout << "Archivo de entrada no encontrado. Creando uno vacío..." << endl;
        // Creo el archivo si no existe, como un truco que vi en internet.
        ofstream crear("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto comprimido.txt");
        crear.close();
        return; // Nada que hacer.
    }

    ofstream archivo_salida("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto descomprimido.txt", ios::binary); // Archivo de salida.

    // Diccionario para guardar cadenas. Uso punteros porque es dinámico.
    const int MAX_ENTRADAS = 10000; // Lo puse grande por si el archivo es grande, no quiero errores.
    char* diccionario[MAX_ENTRADAS]; // Arreglo de punteros a char.
    int tamanos[MAX_ENTRADAS]; // Tamaños de cada entrada.
    int total_entradas = 0; // Cuántas hay.

    while (true) { // Ciclo principal para leer el comprimido.
        char alto, bajo, caracter;
        if (!archivo_entrada.get(alto)) break; // Leo high byte.
        if (!archivo_entrada.get(bajo)) break; // Low byte.
        if (!archivo_entrada.get(caracter)) break; // El carácter.

        int indice = (static_cast<unsigned char>(alto) << 8) | static_cast<unsigned char>(bajo); // Construyo el índice de 16 bits.

        // Verifico no exceder el máximo.
        if (total_entradas >= MAX_ENTRADAS) {
            cout << "Error: Se excedió el tamaño máximo del diccionario." << endl;
            break;
        }

        // Construyo la nueva cadena.
        int nuevo_tamano = 1;
        char* nueva_cadena;

        if (indice == 0) { // Si es cero, solo el carácter.
            nueva_cadena = new char[2];
            nueva_cadena[0] = caracter;
            nueva_cadena[1] = '\0';
        } else {
            if (indice > total_entradas) { // Error si índice inválido.
                cout << "Error: Índice inválido en el diccionario." << endl;
                break;
            }
            // Copio del diccionario y agrego el carácter.
            nuevo_tamano = tamanos[indice - 1] + 1;
            nueva_cadena = new char[nuevo_tamano + 1];
            memcpy(nueva_cadena, diccionario[indice - 1], tamanos[indice - 1]);
            nueva_cadena[nuevo_tamano - 1] = caracter;
            nueva_cadena[nuevo_tamano] = '\0';
        }

        // Escribo en el archivo de salida.
        archivo_salida.write(nueva_cadena, nuevo_tamano);

        // Guardo en el diccionario.
        diccionario[total_entradas] = nueva_cadena;
        tamanos[total_entradas] = nuevo_tamano;
        total_entradas++;
    }

    // Libero la memoria, porque si no hay leaks. Aprendí que delete[] es para arreglos.
    for (int i = 0; i < total_entradas; i++) {
        delete[] diccionario[i];
    }

    archivo_entrada.close();
    archivo_salida.close();
}

void desencriptar(const char* nombre_archivo, int n, unsigned char clave) { // Función para desencriptar con XOR y rotación.
    ifstream archivo_entrada(nombre_archivo, ios::binary);
    ofstream archivo_salida("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto comprimido.txt", ios::binary);

    if (!archivo_entrada.is_open()) {
        cout << "Error: No se pudo abrir el archivo de entrada." << endl;
        return;
    }
    if (!archivo_salida.is_open()) {
        cout << "Error: No se pudo abrir el archivo de salida." << endl;
        return;
    }

    char byte_leido; // Leo byte por byte.
    while (archivo_entrada.get(byte_leido)) {
        unsigned char ubyte = static_cast<unsigned char>(byte_leido);
        ubyte ^= clave; // XOR con la clave.
        ubyte = (ubyte >> n) | (ubyte << (8 - n)); // Rotación a la derecha.
        archivo_salida.put(static_cast<char>(ubyte)); // Escribo.
    }
    archivo_entrada.close();
    archivo_salida.close();
}

int determinar_metodo() { // Intento adivinar si es RLE o LZ78 mirando el archivo.
    ifstream archivo("/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Texto comprimido.txt", ios::binary);
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir el archivo comprimido para determinación." << endl;
        return 0;
    }

    char caracter;
    bool tiene_bytes_nulos = false; // Para detectar LZ78.
    bool tiene_bytes_altos = false;
    bool parece_rle = false;
    int contador = 0;
    int digitos_consecutivos = 0;
    int pares_caracter_digito = 0;  // Contador de pares.
    bool ultimo_fue_imprimible = false;

    while (archivo.get(caracter) && contador < 500) {  // Miro los primeros 500 bytes.
        unsigned char u_caracter = static_cast<unsigned char>(caracter);

        if (u_caracter == 0) {
            tiene_bytes_nulos = true;
        }

        if (u_caracter > 127) {
            tiene_bytes_altos = true;
        }

        // Para detectar RLE: caracteres seguidos de números.
        if (isdigit(u_caracter)) {
            if (ultimo_fue_imprimible) {
                pares_caracter_digito++;
            }
            digitos_consecutivos++;
            ultimo_fue_imprimible = false;
            if (digitos_consecutivos > 10) {
                parece_rle = false;
            }
        } else {
            ultimo_fue_imprimible = isprint(u_caracter) && u_caracter != 127;
            digitos_consecutivos = 0;
        }

        contador++;
    }

    archivo.close();

    if (pares_caracter_digito > 5 && !tiene_bytes_nulos) {
        parece_rle = true;
    }

    if (tiene_bytes_nulos || (tiene_bytes_altos && !parece_rle)) {
        return 2; // LZ78
    }

    if (parece_rle) {
        return 1; // RLE
    }

    return 2; // Por defecto LZ78.
}

void descomprimir_auto() { // Descomprime automáticamente eligiendo el método.
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

int main() { // Función principal, donde empieza todo.
    const char* nombre_archivo = "/home/david/Documentos/Informatica_Desafios/Unir_ccodigos/Encriptado1.txt"; // El archivo encriptado.
    // Llamo a desencriptar con valores que probé, n=3 y clave=0x5A.
    desencriptar(nombre_archivo, 3, 0x5A); // Cambiar si es otro.

    // Luego descomprimo automáticamente.
    descomprimir_auto();

    cout << "Proceso completado. Revisa el archivo 'Texto descomprimido.txt'" << endl;

    return 0; // Todo bien.
}
