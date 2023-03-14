**Introducción**

 
En esta práctica hemos implementado un servidor http capaz de procesar y responder peticiones. Utilizando el picohttpparser como parseador de peticiones.

**Requisitos**

* [x] Recibir peticiones.
* [x] Parsear peticiones.
* [x] Creacion de respuesta dependiendo del verbo.
* [x] GET, POST, OPTIONS.
* [x] Ejecucion de scripts.
* [x] Gestion de peticiones y respuestas de forma concurrente.

**Compilación** 

Para la compilación solo será necesario ejecutar el comando make en la terminal. Esto generara los archivos objetos **(.o)** ne la carpeta **/obj** en el directorio raiz del proyecto.

**Ejecución** 

Para ejecutar sirve con escrivir **./server** en el terminal y el servidor estará operativo. La terminal se quedara bloqueada durante la ejecución del servidor y este podrá ser parado enviando la señal **SIGINT** o ctrl+C en la terminal, matando asi el proceso. Existe en el archivo server_utils.c una funcion que demoniza el proceso del servidor pero no es utilizada porque aparentemente no funciona en Ubuntu.

**Comprovación** 

Para testear el proyecto nosotros utilizamos tanto el comando `curl` en la terminal para hacer peticiones al servidor, como un navegador web ya que es muy visual el que recibe o no, además, las heramientas para desarrolladores que incorporan navegadores como Mozilla Firefox o Google Chrome son extremadamente útiles. Asi mismo instalamos una extensión de Google Chrome llamada **Espía http** que te dice toda las cabeceras de peticiones y respuestas.

Usando el navegador simplemente acedemos a la IP por defecto, en nuestro caso **127.0.0.1** y al puerto asignado en el archivo de configuración, inicialmente 8080. El resultado para cargar la página principar seria este.

`http://127.0.0.1:8080/index.html`

**Desarrollo** 

Durante el desarrollo de el servidor hemos tomado varias decisiones de diseño. La primera fue no utilizar el libconfuse e implementar el parseo nosotros mismos, esto lo hicimos ya que el entorno donde hemos desarrollado la práctica es MACOS yno era compatible esta libreria y puesto que había que parsear muy pocas lineas lo hicimos a mano. Para almacenar la configuración del servidor acudimos al uso de variables globales ya que asi podria ser accesibles para tanto el proceso padre como los procesos hijos, y al no modificarse nunca, no habria problemas de concurrencia.

La siguiente decisión fue hacer el servidor concurrente con procesos en vez de con hilos ya que consideramos que la manipulación de procesos es más sencillo de implementar y es más facil de debugear en caso de error.

Para trasmitir los datos de una peticion de función en función creamos una estructura que almacenara toda la información de la petición.

El proceso padre tras parsear la configuración y configurar el socket hace un `accept`, lo que devuelve otro socket. El proceso padre crea un hijo usando la función `fork` y este empieza a procesar la petición.

Procesar una petición consiste en parsearla y responderla construyendo la cabecera con los datos adecuados como el tipo de respuesta, el tipo de contenido, la fecha de la respuesta o la fecha de modificación y se enviará por el socket creado por el `accept` y se cerraría el proceso hijo.


**Diagrama de flujo**


```flow
st=>start: Setup
op=>operation: Parsear conf
op2=>operation: accept
op3=>condition: fork(es el padre?)
op4=>operation: Parsear peticion
op5=>operation: Responder
e=>end: End hijo 

st->op->op2->op3
op3(yes)->op2
op3(no)->op4
op4->op5->e
```

**Estructura de htmlfiles/** 

Para los archivos almacenados en nuestro servidor utilizamos la siguiente estructura:

**htmlfiles
    ├── IMPORTANTE.txt
    ├── index.html
    ├── media
    │   ├── animacion.gif
    │   ├── img1.jpg
    │   ├── img10.jpg
    │   ├── img2.jpg
    │   ├── img3.jpg
    │   ├── img4.jpg
    │   ├── img5.jpg
    │   ├── img6.jpg
    │   ├── img7.jpg
    │   ├── img8.jpg
    │   ├── img9.jpg
    │   ├── img_big.jpeg
    │   ├── img_small.jpeg
    │   └── texto.txt
    └── scripts
    │ ├── test.php
   │ └── test.py**

\###Conclusión

Esta práctica aunque haya sido larga nos ha ensañado mucho de como funcionan los sockets y sobretodo la comunicación mediante http y esta la primera vez que partimos de 0, sin código fuente.
