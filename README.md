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

  ---
  Imagenes
  Capturas
  <img width="921" height="575" alt="image" src="https://github.com/user-attachments/assets/3e6371bf-f937-4378-801c-27081eea4e30" />
<img width="921" height="320" alt="image" src="https://github.com/user-attachments/assets/ac9b8ab2-c0ca-41c2-94bd-cd9e3bd0315b" />
<img width="921" height="354" alt="image" src="https://github.com/user-attachments/assets/3666637e-49b1-43dd-9c6b-dade506a7a34" />

Circuito en fisico 
<img width="5712" height="4284" alt="IMG_1204" src="https://github.com/user-attachments/assets/2020e1cc-07de-4d1f-b63d-d6eba83a8c36" />
<img width="5712" height="4284" alt="IMG_1205" src="https://github.com/user-attachments/assets/f9b203e6-a930-4886-96b5-fb897e263a77" />
<img width="5712" height="4284" alt="IMG_1203" src="https://github.com/user-attachments/assets/3a8a9d4f-20b7-4b97-9f48-2fe3832c7f53" />
<img width="5712" height="4284" alt="IMG_1206" src="https://github.com/user-attachments/assets/89d17947-ff6f-4572-8e36-fe9ba735a85b" />
<img width="3024" height="4032" alt="IMG_1207" src="https://github.com/user-attachments/assets/5fda7e71-7fd2-454b-8b9d-ced76bed9d45" />

---

Conclusión

En esta práctica se implementó un sistema multitarea utilizando FreeRTOS sobre una tarjeta STM32F401RE. Se desarrollaron tareas independientes para el parpadeo rápido y lento de un LED, así como una tarea encargada de realizar la lectura del ADC cuando el usuario presiona un botón.

Durante el desarrollo se comprobó el funcionamiento de mecanismos importantes de FreeRTOS, como la creación, suspensión y reanudación de tareas mediante TaskHandle_t, permitiendo controlar el flujo de ejecución del sistema de manera ordenada. También se utilizó el Idle Hook para detectar cuando no existen tareas activas y aprovechar ese tiempo para monitorear eventos externos sin necesidad de crear una tarea adicional.

El sistema logró cumplir con los requerimientos establecidos: ejecutar el parpadeo rápido durante 5 segundos, continuar con el parpadeo lento durante otros 5 segundos y posteriormente entrar en un estado de espera. En dicho estado, el sistema puede regresar automáticamente al modo rápido o realizar una lectura del ADC cuando se detecta la presión del botón, mostrando el resultado en la terminal mediante comunicación UART.

Esta práctica permitió reforzar conocimientos relacionados con sistemas operativos en tiempo real, planificación de tareas, manejo de GPIO, conversión analógica-digital, comunicación serial y administración eficiente de recursos en sistemas embebidos.
