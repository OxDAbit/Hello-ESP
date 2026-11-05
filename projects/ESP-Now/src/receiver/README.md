<h1 align="center">Receptor ESP-NOW</h1>

<p align="center">
  Recibe mensajes por ESP-NOW y los señala con parpadeos rojo, verde y azul del LED integrado<br>
  <sub>by <a href="https://oxdabit.com"><img src="../../../../assets/oxdabit.png" alt="" width="16" height="16"> <b>0xDA</b> bit</a></sub>
</p>

<p align="center">
  <img alt="Versión 0.0.1" src="https://img.shields.io/badge/versi%C3%B3n-0.0.1-f3e5ab?style=flat-square&labelColor=1c3f6e">
  <img alt="Licencia MIT" src="https://img.shields.io/badge/licencia-MIT-f3e5ab?style=flat-square&labelColor=1c3f6e">
</p>

Forma parte del proyecto [ESP-NOW](../../) y es su mitad receptora. Se graba en la **placa ESP 02** y espera los mensajes que envía cada 5 segundos la **placa ESP 01** con el [emisor](../emitter/). Por cada mensaje recibido, el LED RGB integrado parpadea una vez en **rojo**, otra en **verde** y otra en **azul**.

Solo acepta los mensajes de la placa ESP 01. Los que lleguen de cualquier otra placa los ignora y lo deja anotado en el monitor.

## Cómo funciona

1. Inicia el LED RGB integrado, un **WS2812** en el GPIO48, con el componente `espressif/led_strip`, igual que en [LED integrado ESP32-S3-N16R8](../../../led-onboard-ESP32-S3-N16R8/).
2. Arranca el Wi-Fi en modo estación en el **canal 1**, el mismo que el emisor, sin conectarse a ninguna red.
3. Inicia ESP-NOW y registra un *callback* de recepción. Para recibir no hace falta registrar al emisor como *peer*; eso solo lo necesita quien envía.
4. Cuando llega un paquete, el *callback* comprueba que viene de la MAC de la placa ESP 01 y que tiene el tamaño de un mensaje, y lo mete en una cola.
5. El bucle principal espera en esa cola. Por cada mensaje escribe su número en el monitor y ejecuta los tres parpadeos.

El *callback* no hace los parpadeos directamente porque se ejecuta en la tarea del Wi-Fi: si se quedara 2,4 segundos esperando al LED, bloquearía la radio. Por eso pasa el mensaje por una cola y el parpadeo lo hace el bucle principal.

## Placas

| Placa | Papel | MAC Wi-Fi STA |
| --- | --- | --- |
| ESP 01 | [Emisor](../emitter/) | `14:C1:9F:26:D1:38` |
| ESP 02 | Receptor (este programa) | `E8:3D:C1:F2:AC:3C` |

Si usas otras placas, cambia `MAC_EMISOR` al principio de `main/receiver.cpp` por la MAC de tu emisor, y `MAC_RECEPTOR` en el emisor por la de tu receptor. Las MAC se obtienen con [Detección de MAC](../../../mac-detection/).

## Hardware

- Dos placas **ESP32-S3-N16R8** (16 MB de flash y 8 MB de PSRAM octal).
- Dos cables USB-C de datos, o uno solo si alimentas el emisor por otro lado.

El LED integrado está en el GPIO48 en la ESP32-S3-DevKitC-1 v1.0 y en la mayoría de clones N16R8. En la v1.1 está en el GPIO38: cambia `GPIO_LED` en el código.

## Estructura

```text
receiver/
├── CMakeLists.txt          proyecto ESP-IDF; carga ../sdkconfig.n16r8
├── dependencies.lock       versiones exactas de los componentes descargados
└── main/
    ├── CMakeLists.txt      código y componentes que usa
    ├── idf_component.yml   dependencia de espressif/led_strip
    └── receiver.cpp        el programa
```

La configuración de la placa está en [`../sdkconfig.n16r8`](../sdkconfig.n16r8), compartida con el [emisor](../emitter/). El componente `led_strip` se descarga solo la primera vez que compilas, en `managed_components/`.

## Personalizar

Al principio de `main/receiver.cpp`:

| Constante | Valor | Qué controla |
| --- | --- | --- |
| `MAC_EMISOR` | ESP 01 | MAC Wi-Fi STA de la única placa cuyos mensajes se aceptan |
| `CANAL_WIFI` | `1` | Canal Wi-Fi; tiene que ser el mismo en el emisor |
| `GPIO_LED` | `48` | Pin del LED RGB integrado |
| `T_ENCENDIDO_MS` / `T_APAGADO_MS` | `300` / `500` | Tiempo encendido y apagado de cada parpadeo |
| `BRILLO` | `32` | Intensidad de cada color (0-255) |
| `SECUENCIA` | rojo, verde, azul | Colores y orden de los parpadeos |
| `Mensaje` | `secuencia` | Contenido del mensaje; tiene que ser idéntico en `emitter.cpp` |

Con los valores por defecto, la secuencia completa dura 2,4 segundos, menos que los 5 segundos entre mensajes. Si la alargas por encima del periodo del emisor, los mensajes se acumulan en la cola (caben 4) y, cuando se llena, los nuevos se descartan.

## Compilar y grabar

Necesitas [ESP-IDF](https://docs.espressif.com/projects/esp-idf/) v5.5 o superior (probado con v6.1).

Desde VS Code con la extensión ESP-IDF: abre esta carpeta, elige el puerto de la placa ESP 02 con **ESP-IDF: Select Port to Use** y ejecuta **ESP-IDF: Build, Flash and Monitor**.

Desde la terminal:

```sh
idf.py set-target esp32s3
idf.py -p /dev/cu.usbmodemXXXX build flash monitor
```

Con las dos placas conectadas al mismo ordenador, cada una tiene su puerto. Comprueba cuál es cuál antes de grabar, por ejemplo con el programa de [Detección de MAC](../../../mac-detection/). Para salir del monitor, `Ctrl+]`, o `Ctrl+T` y después `Ctrl+X`.

## Salida esperada

```text
I (660) receiver: Receptor e8:3d:c1:f2:ac:3c esperando a 14:c1:9f:26:d1:38 (canal 1, LED en GPIO48)
I (3210) receiver: Recibido mensaje #1 de la placa ESP 01
I (8210) receiver: Recibido mensaje #2 de la placa ESP 01
```

Con cada línea «Recibido», el LED hace un parpadeo rojo, uno verde y uno azul. Si llega un paquete de otra placa:

```text
W (9120) receiver: Ignorado: paquete de aa:bb:cc:dd:ee:ff (no es la placa ESP 01)
```

## Si algo falla

| Síntoma | Causa probable |
| --- | --- |
| No llega ningún mensaje | El emisor no está encendido, la MAC de `MAC_EMISOR` no es la suya, o los dos programas usan canales distintos. |
| Llegan mensajes, pero el LED no se enciende | El LED no está en el GPIO48 de tu placa: prueba con el GPIO38 en `GPIO_LED`. |
| «Ignorado: tamaño…» | El `Mensaje` del emisor y el del receptor no son iguales. |
| «Failed to connect» al grabar | Pon la placa en modo descarga: mantén BOOT, pulsa y suelta RST, suelta BOOT. |
| El monitor no muestra nada | Pulsa RST con el monitor abierto. |

---

<p align="center">
  <a href="https://oxdabit.com"><img src="../../../../assets/oxdabit.png" alt="Isotipo de 0xDA bit" width="56" height="56"></a>
</p>

<p align="center">
  <b>0xDA</b> bit · <i>Del boceto al plato</i><br>
  <sub><a href="https://oxdabit.com">oxdabit.com</a> · <a href="https://www.youtube.com/@0xDAbitLabs">YouTube</a> · <a href="https://github.com/OxDAbit">GitHub</a> · <a href="https://x.com/0xDA_bit">X</a> · <a href="https://www.instagram.com/oxdabitlabs/">Instagram</a></sub>
</p>
