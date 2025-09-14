#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>

int main() {
    printf("=== Monitor de Procesos ===\n");
    printf("PID del proceso padre: %d\n\n", getpid());
   
    // Comandos simples para ejecutar
    char *comandos[5][3] = {
        {"ls", NULL, NULL},        // Listar archivos
        {"pwd", NULL, NULL},       // Mostrar directorio actual
        {"whoami", NULL, NULL},    // Mostrar usuario actual
        {"date", NULL, NULL},      // Mostrar fecha
        {"echo", "Hola!", NULL}    // Mostrar mensaje
    };
   
    int num_comandos = 5;
    pid_t pids_hijos[5];
   
    struct timeval inicio, fin;
    gettimeofday(&inicio, NULL);
   
    printf("Creando procesos hijos...\n");
   
    // Crear los procesos hijos
    for (int i = 0; i < num_comandos; i++) {
        pid_t proceso = fork();
       
        if (proceso < 0) {
            printf("❌ Error al crear proceso hijo %d\n", i);
            exit(1);
        }
        else if (proceso == 0) {
            // PROCESO HIJO
            printf("👶 Hijo %d (PID: %d) ejecutando: %s\n",
                   i, getpid(), comandos[i][0]);
           
            // Ejecutar el comando
            execvp(comandos[i][0], comandos[i]);
           
            // Si llegamos aquí, execvp falló
            printf("❌ Error ejecutando: %s\n", comandos[i][0]);
            exit(1);
        }
        else {
            // PROCESO PADRE
            pids_hijos[i] = proceso;
            printf("👨 Padre creó hijo %d con PID: %d\n", i, proceso);
        }
    }
   
    printf("\n--- Esperando que terminen los procesos hijos ---\n");
   
    // Esperar a todos los hijos
    int completados = 0;
    int exitosos = 0;
   
    for (int i = 0; i < num_comandos; i++) {
        int status;
        pid_t proceso_terminado = wait(&status);
       
        if (proceso_terminado > 0) {
            completados++;
           
            // Buscar qué hijo era
            int indice = -1;
            for (int j = 0; j < num_comandos; j++) {
                if (pids_hijos[j] == proceso_terminado) {
                    indice = j;
                    break;
                }
            }
           
            if (WIFEXITED(status)) {
                int codigo_salida = WEXITSTATUS(status);
                if (codigo_salida == 0) {
                    printf("✅ Hijo %d (PID: %d, Comando: %s) terminó correctamente\n",
                           indice, proceso_terminado, comandos[indice][0]);
                    exitosos++;
                } else {
                    printf("⚠️  Hijo %d (PID: %d, Comando: %s) terminó con error (código: %d)\n",
                           indice, proceso_terminado, comandos[indice][0], codigo_salida);
                }
            } else {
                printf("💥 Hijo %d (PID: %d) terminó de manera anormal\n",
                       indice, proceso_terminado);
            }
        }
    }
   
    gettimeofday(&fin, NULL);
   
    // Calcular tiempo total
    double tiempo_total = (fin.tv_sec - inicio.tv_sec) +
                         (fin.tv_usec - inicio.tv_usec) / 1000000.0;
   
    printf("\n=== RESUMEN ===\n");
    printf("📊 Total de procesos creados: %d\n", num_comandos);
    printf("✅ Procesos exitosos: %d\n", exitosos);
    printf("❌ Procesos con error: %d\n", completados - exitosos);
    printf("⏱️  Tiempo total de ejecución: %.3f segundos\n", tiempo_total);
   
    return 0;
}
