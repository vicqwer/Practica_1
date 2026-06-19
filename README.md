Integrantes

- Victor Hugo Barrera García
- Sergio García Hernández

---
Descripción

Práctica desarrollada en la tarjeta STM32F401RE utilizando FreeRTOS.

El sistema implementa tres tareas principales:

- Parpadeo rápido de LED.
- Parpadeo lento de LED.
- Lectura de ADC mediante un botón.

Además, se utiliza el Idle Hook de FreeRTOS para gestionar el estado de espera del sistema.

---

Funcionamiento

1. El LED parpadea rápidamente durante 5 segundos.
2. Posteriormente el LED parpadea lentamente durante 5 segundos.
3. Al finalizar, el sistema entra en estado IDLE.
4. Si durante el estado IDLE se presiona el botón:
   - Se realiza una lectura ADC.
   - Se calcula el voltaje correspondiente.
   - Se muestra la información por UART.
   - El sistema regresa al modo de parpadeo rápido.
5. Si no se presiona el botón durante 5 segundos:
   - El sistema regresa automáticamente al modo de parpadeo rápido.

---

Hardware utilizado

- STM32F401RE Nucleo
- Potenciómetro conectado al ADC1 Canal 0 (PA0)
- Push Button en PA6
- LED en PA9
- Comunicación UART2 a 115200 bps

---

Conceptos aplicados

- FreeRTOS
- Creación de tareas
- Suspensión y reanudación de tareas
- Idle Hook
- Lectura ADC
- Comunicación UART
- GPIO
- Planificación de tareas en tiempo real

---
Preguntas de análisis

1. ¿Por qué la variable g_ledRapido debe declararse como volatile? ¿Qué ocurre si se omite esa palabra clave?

Una variable compartida debe declararse como `volatile` cuando puede ser modificada por más de una tarea o por una 
interrupción. Esto le indica al compilador que el valor de la variable puede cambiar en cualquier momento y que no debe optimizar su lectura.

Si se omite `volatile`, el compilador podría guardar el valor de la variable en un registro interno y no leerlo nuevamente desde memoria.
Esto puede provocar que una tarea no detecte los cambios realizados por otra tarea, generando fallas en la lógica del sistema.

En esta práctica, aunque se usó la bandera `IDLE_HOOK`, el principio es el mismo: al ser una variable compartida entre tareas y el Idle Hook, 
debe declararse como `volatile`.

---

2. ¿En qué momento exacto aparece el mensaje [IDLE] en la terminal? Describe el estado de las cuatro tareas en ese instante.

El mensaje `[IDLE]` aparece cuando FreeRTOS ejecuta la Idle Task, es decir, cuando no existe ninguna tarea de usuario lista para ejecutarse.

En esta práctica ocurre después de que termina el parpadeo rápido y el parpadeo lento, cuando el sistema entra al estado de espera. En ese instante, 
la tarea del LED rápido está suspendida, la tarea del LED lento también está suspendida y la tarea del ADC permanece suspendida hasta que se presione 
el botón. Como no hay tareas activas ejecutándose, FreeRTOS entra a la Idle Task.

Durante ese estado, el sistema espera dos posibles eventos: que se presione el botón para leer el ADC o que pasen 5 segundos para regresar automáticamente 
al modo rápido.

---

3. ¿Qué diferencia existe entre vTaskDelay() y vTaskDelayUntil()? ¿En cuál de las tareas de esta práctica sería más apropiado usar vTaskDelayUntil?

`vTaskDelay()` bloquea una tarea durante un tiempo relativo, contado desde el momento en que se llama la función. Por ejemplo, si se usa
`vTaskDelay(pdMS_TO_TICKS(100))`, la tarea se detiene aproximadamente 100 ms desde ese punto.

`vTaskDelayUntil()` permite ejecutar una tarea con un periodo fijo, tomando como referencia un tiempo anterior. Esto ayuda a mantener una
frecuencia más constante, ya que evita que pequeños retrasos acumulados afecten el periodo de ejecución.

En esta práctica sería más apropiado usar `vTaskDelayUntil()` en las tareas de parpadeo del LED, tanto en `vTaskLedRapido` como en `vTaskLedLento`,
porque ambas necesitan mantener tiempos constantes de 100 ms y 500 ms respectivamente.

---

4. ¿Por qué vTaskLedRapido tiene prioridad menor que vTaskMonitor? Describe qué ocurriría si se invirtieran esas prioridades.

La tarea de monitoreo debe tener mayor prioridad porque se encarga de detectar eventos importantes del sistema, como la presión del botón. Al 
tener mayor prioridad, puede responder más rápido cuando ocurre un evento externo.

Si `vTaskLedRapido` tuviera mayor prioridad que la tarea de monitoreo, el sistema seguiría funcionando, pero la detección del botón podría tardar 
más o sentirse menos inmediata, especialmente si el LED rápido se ejecuta con mucha frecuencia.

En nuestra implementación no se usó una tarea llamada `vTaskMonitor`, pero esa función la realiza el Idle Hook al revisar el botón durante el estado 
de espera. La idea sigue siendo la misma: la detección del botón debe atenderse de forma rápida para que el sistema pueda pasar a la lectura del ADC.

---

5. ¿Qué riesgo existe al leer una variable volatile desde dos tareas distintas sin protección? Investiga el concepto de sección crítica.

El riesgo principal es que se produzca una condición de carrera. Esto ocurre cuando dos tareas acceden a la misma variable compartida al mismo tiempo 
y al menos una de ellas la modifica. Como resultado, el sistema puede tomar decisiones con datos inconsistentes.

La palabra `volatile` evita que el compilador optimice la lectura de la variable, pero no protege contra accesos simultáneos. Para proteger una variable
compartida se pueden usar mecanismos como secciones críticas, semáforos o mutex.

Una sección crítica es una parte del código donde se accede a un recurso compartido y debe ejecutarse sin interrupciones o sin que otra tarea modifique 
ese recurso al mismo tiempo. En FreeRTOS pueden utilizarse funciones como `taskENTER_CRITICAL()` y `taskEXIT_CRITICAL()` para proteger esas zonas del programa.

Compilación

Desarrollado con:

- STM32CubeIDE
- FreeRTOS

Microcontrolador:

- STM32F401RE
