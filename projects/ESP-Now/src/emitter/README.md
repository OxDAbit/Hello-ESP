<h1 align="center">Emisor ESP-NOW</h1>

<p align="center">
  Envía cada 5 segundos un mensaje por ESP-NOW de una placa ESP32-S3 a otra<br>
  <sub>by <a href="https://oxdabit.com"><img src="../../../../assets/oxdabit.png" alt="" width="16" height="16"> <b>0xDA</b> bit</a></sub>
</p>

<p align="center">
  <img alt="Versión 0.0.1" src="https://img.shields.io/badge/versi%C3%B3n-0.0.1-f3e5ab?style=flat-square&labelColor=1c3f6e">
  <img alt="Licencia MIT" src="https://img.shields.io/badge/licencia-MIT-f3e5ab?style=flat-square&labelColor=1c3f6e">
</p>

Forma parte del proyecto [ESP-NOW](../../) y es su mitad emisora. Se graba en la **placa ESP 01** y, cada 5 segundos, envía un mensaje a la **placa ESP 02**, que lleva el [receptor](../receiver/). El mensaje es un número de secuencia que empieza en 1 y aumenta en cada envío, así se ve en los dos monitores qué mensaje sale y cuál llega.

ESP-NOW es un protocolo de Espressif que envía paquetes directamente de una placa a otra usando la radio Wi-Fi, sin router ni conexión a una red. Cada placa se identifica por su MAC Wi-Fi en modo estación (STA), que se obtiene con [Detección de MAC](../../../mac-detection/).

## Cómo funciona

1. Arranca el Wi-Fi en modo estación, sin conectarse a ningún punto de acceso, y lo fija en el **canal 1**. ESP-NOW necesita la radio encendida, pero no una red.
2. Inicia ESP-NOW y registra la placa ESP 02 como destino (*peer*), con su MAC y el mismo canal.
3. Cada 5 segundos envía el mensaje con `esp_now_send()`.
4. Al terminar cada envío, ESP-NOW llama a un *callback* que indica si el receptor confirmó la recepción. Esa confirmación (ACK) la da la radio del receptor, no su programa, así que solo garantiza que el paquete llegó.

## Placas

| Placa | Papel | MAC Wi-Fi STA |
| --- | --- | --- |
| ESP 01 | Emisor (este programa) | `14:C1:9F:26:D1:38` |
| ESP 02 | [Receptor](../receiver/) | `E8:3D:C1:F2:AC:3C` |

Si usas otras placas, cambia `MAC_RECEPTOR` al principio de `main/emitter.cpp` por la MAC de tu receptor, y `MAC_EMISOR` en el receptor por la de tu emisor.

## Hardware

- Dos placas **ESP32-S3-N16R8** (16 MB de flash y 8 MB de PSRAM octal).
- Dos cables USB-C de datos, o uno solo si alimentas el receptor por otro lado.

## Estructura

```text
emitter/
├── CMakeLists.txt      proyecto ESP-IDF; carga ../sdkconfig.n16r8
└── main/
    ├── CMakeLists.txt  código y componentes que usa
    └── emitter.cpp     el programa
```

La configuración de la placa está en [`../sdkconfig.n16r8`](../sdkconfig.n16r8), compartida con el [receptor](../receiver/).

## Personalizar

Al principio de `main/emitter.cpp`:

| Constante | Valor | Qué controla |
| --- | --- | --- |
| `PERIODO_MS` | `5000` | Tiempo entre envíos, en milisegundos |
| `CANAL_WIFI` | `1` | Canal Wi-Fi; tiene que ser el mismo en el receptor |
| `MAC_RECEPTOR` | ESP 02 | MAC Wi-Fi STA de la placa de destino |
| `Mensaje` | `secuencia` | Contenido del mensaje; tiene que ser idéntico en `receiver.cpp` |

## Compilar y grabar

Necesitas [ESP-IDF](https://docs.espressif.com/projects/esp-idf/) v5.5 o superior (probado con v6.1).

Desde VS Code con la extensión ESP-IDF: abre esta carpeta, elige el puerto de la placa ESP 01 con **ESP-IDF: Select Port to Use** y ejecuta **ESP-IDF: Build, Flash and Monitor**.

Desde la terminal:

```sh
idf.py set-target esp32s3
idf.py -p /dev/cu.usbmodemXXXX build flash monitor
```

Con las dos placas conectadas al mismo ordenador, cada una tiene su puerto. Comprueba cuál es cuál antes de grabar, por ejemplo con el programa de [Detección de MAC](../../../mac-detection/), para no grabar el emisor en la placa equivocada. Para salir del monitor, `Ctrl+]`, o `Ctrl+T` y después `Ctrl+X`.

## Salida esperada

Con el receptor encendido:

```text
I (650) emitter: Emisor 14:c1:9f:26:d1:38 -> receptor e8:3d:c1:f2:ac:3c (canal 1)
I (655) emitter: Enviado mensaje #1
I (660) emitter: Entregado a e8:3d:c1:f2:ac:3c
I (5660) emitter: Enviado mensaje #2
I (5665) emitter: Entregado a e8:3d:c1:f2:ac:3c
```

Con el receptor apagado o fuera de alcance, el envío sale igual, pero no llega la confirmación:

```text
I (5660) emitter: Enviado mensaje #2
W (5690) emitter: Sin confirmación de e8:3d:c1:f2:ac:3c
```

## Si algo falla

| Síntoma | Causa probable |
| --- | --- |
| Siempre «Sin confirmación» | El receptor no está encendido o no tiene el Wi-Fi arrancado, la MAC de `MAC_RECEPTOR` no es la suya, o los dos programas usan canales distintos. |
| Llega la confirmación, pero el receptor no reacciona | La radio del receptor confirma cualquier paquete dirigido a su MAC: comprueba que tiene grabado el [receptor](../receiver/) y que su `MAC_EMISOR` es la de esta placa. |
| `ESP_ERR_ESPNOW_NOT_FOUND` al enviar | El destino no está registrado como *peer*: revisa `MAC_RECEPTOR`. |
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
