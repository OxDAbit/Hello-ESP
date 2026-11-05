<h1 align="center">Hello-ESP</h1>

<p align="center">
  Proyectos con ESP8266 y ESP32 para aprender a programar firmware y diseñar PCB<br>
  <sub>by <a href="https://oxdabit.com"><img src="assets/oxdabit.png" alt="" width="16" height="16"> <b>0xDA</b> bit</a></sub>
</p>

<p align="center">
  <img alt="Versión 0.0.1" src="https://img.shields.io/badge/versi%C3%B3n-0.0.1-f3e5ab?style=flat-square&labelColor=1c3f6e">
  <img alt="Licencia MIT" src="https://img.shields.io/badge/licencia-MIT-f3e5ab?style=flat-square&labelColor=1c3f6e">
</p>

Repositorio de aprendizaje con proyectos basados en microcontroladores ESP8266 y ESP32. Cada proyecto vive en su propia carpeta dentro de [`projects/`](projects/) y tiene su propio README.

## Proyectos

| Proyecto | Placa | Descripción |
| --- | --- | --- |
| [LED integrado ESP32-S3-N16R8](projects/led-onboard-ESP32-S3-N16R8/) | ESP32-S3-N16R8 | Parpadeo en tres colores del LED RGB integrado (WS2812) con ESP-IDF |
| [Detección de MAC](projects/mac-detection/) | ESP32-S3-N16R8 | Lee la MAC Wi-Fi de la placa, necesaria para comunicar placas con ESP-NOW |
| **[ESP-NOW](projects/ESP-Now/)** | ESP32-S3-N16R8 | Comunicación directa entre dos placas, sin router ni red Wi-Fi |
| &nbsp;&nbsp;└ [Emisor](projects/ESP-Now/src/emitter/) | ESP 01 | Envía cada 5 segundos un mensaje numerado a la placa ESP 02 |
| &nbsp;&nbsp;└ [Receptor](projects/ESP-Now/src/receiver/) | ESP 02 | Recibe los mensajes de la ESP 01 y los señala con parpadeos rojo, verde y azul del LED integrado |

## Entorno de trabajo

Todos los proyectos se programan en C++ con [ESP-IDF](https://docs.espressif.com/projects/esp-idf/), el framework oficial de Espressif, desde VS Code con la extensión **ESP-IDF** (`espressif.esp-idf-extension`). Están probados con ESP-IDF v6.1 y necesitan, como mínimo, la v5.5.

Para trabajar con un proyecto, abre su carpeta en VS Code (no la raíz del repositorio): así la extensión lo detecta como proyecto ESP-IDF. Después, **ESP-IDF: Select Port to Use** para elegir el puerto de la placa y **ESP-IDF: Build, Flash and Monitor** para compilar, grabar y ver la salida. Desde la terminal, lo mismo es:

```sh
idf.py -p /dev/cu.usbmodemXXXX build flash monitor
```

## LED integrado ESP32-S3-N16R8

El LED de la placa es un **WS2812**, un LED RGB direccionable que recibe el color por un único pin con una señal de tiempos muy precisos. El proyecto la genera con el periférico RMT a través del componente oficial `espressif/led_strip` y repite en bucle 3 parpadeos en verde, 3 en rojo y 1 en azul, con 300 ms encendido y 500 ms apagado.

- **Placa:** ESP32-S3-N16R8.
- **Pin del LED:** GPIO48 por defecto (GPIO38 en la DevKitC-1 v1.1), configurable en `menuconfig` sin tocar el código.
- **Personalizable:** tiempos, brillo y secuencia, al principio de `main/main.cpp`.

[Documentación completa →](projects/led-onboard-ESP32-S3-N16R8/)

## Detección de MAC

ESP-NOW identifica cada placa por su **MAC Wi-Fi en modo estación (STA)**, así que antes de comunicar dos placas hay que conocer la MAC de cada una. Este proyecto la lee de los eFuses del chip, sin arrancar el Wi-Fi, y la imprime cada 5 segundos en el monitor serie, también como array de C listo para copiar en el código. Al arrancar muestra además el chip, la flash y la PSRAM, para comprobar que la configuración coincide con la placa.

- **Placa:** ESP32-S3-N16R8; el código sirve para cualquier ESP32 cambiando el *target*.
- **Independiente:** se usa para preparar el proyecto [ESP-NOW](projects/ESP-Now/), pero no depende de él y tiene su propia configuración de la placa (`sdkconfig.defaults`).

[Documentación completa →](projects/mac-detection/)

## ESP-NOW

**ESP-NOW** es un protocolo de Espressif que envía paquetes directamente de una placa a otra usando la radio Wi-Fi, sin router ni conexión a una red. El proyecto usa dos placas, cada una con su programa:

| Placa | Programa | MAC Wi-Fi STA | Qué hace |
| --- | --- | --- | --- |
| ESP 01 | [Emisor](projects/ESP-Now/src/emitter/) | `14:C1:9F:26:D1:38` | Cada 5 segundos envía a la ESP 02 un mensaje con un número de secuencia y anota en el monitor si el receptor confirmó la recepción. |
| ESP 02 | [Receptor](projects/ESP-Now/src/receiver/) | `E8:3D:C1:F2:AC:3C` | Acepta solo los mensajes de la ESP 01 y, por cada uno, hace parpadear el LED RGB integrado una vez en rojo, otra en verde y otra en azul. |

- **Placas:** dos ESP32-S3-N16R8, con el Wi-Fi en modo estación en el canal 1 y sin conectarse a ninguna red.
- **Requisito:** conocer la MAC de cada placa, que se obtiene con [Detección de MAC](projects/mac-detection/). Si usas otras placas, cambia las MAC al principio de `emitter.cpp` y `receiver.cpp`.
- **LED:** el receptor usa el WS2812 del GPIO48 con `espressif/led_strip`, como el proyecto del LED integrado.

[Documentación completa →](projects/ESP-Now/) · [Emisor →](projects/ESP-Now/src/emitter/) · [Receptor →](projects/ESP-Now/src/receiver/)

## Licencia

El contenido de este repositorio está bajo licencia [MIT](LICENSE): puedes usarlo, modificarlo y redistribuirlo manteniendo el aviso de copyright.

Quedan fuera de esa licencia el nombre y el icono de **0xDA_bit** (`assets/oxdabit.png`).

---

<p align="center">
  <a href="https://oxdabit.com"><img src="assets/oxdabit.png" alt="Isotipo de 0xDA bit" width="56" height="56"></a>
</p>

<p align="center">
  <b>0xDA</b> bit · <i>Del boceto al plato</i><br>
  <sub><a href="https://oxdabit.com">oxdabit.com</a> · <a href="https://www.youtube.com/@0xDAbitLabs">YouTube</a> · <a href="https://github.com/OxDAbit">GitHub</a> · <a href="https://x.com/0xDA_bit">X</a> · <a href="https://www.instagram.com/oxdabitlabs/">Instagram</a></sub>
</p>
