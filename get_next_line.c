
#include "get_next_line.h"

static char	*function_name(int fd, char *buf, char *backup)
{
	int		read_line;
	char	*char_temp;

	read_line = 1;
	while (read_line != '\0')//OJO NECESITA MODIFICACION. Se inicia un bucle que continúa mientras read_line no sea igual a '\0' (lo cual es incorrecto, ya que read_line es un entero, no un carácter). Lo más apropiado es: while (!ft_strchr(buf, '\n')), ya que eso interrumpiría el bucle cuando se encuentre un salto de línea en buf, evitando que siga leyendo innecesariamente después de encontrar la línea completa.
	{
		read_line = read(fd, buf, BUFFER_SIZE);//Se lee el archivo hasta BUFFER_SIZE bytes. La función read devuelve el número de bytes que efectivamente ha leído, por lo que read_line valdrá un número # de bytes leídos. Si tiene un BUFFER_SIZE de 3 valdrá 3. En cada iteración, buf se actualiza cuando se vuelve a llamar a read(fd, buf, BUFFER_SIZE), lo que carga un nuevo bloque de datos en buf, reemplazando el contenido anterior.
		if (read_line == -1)//Es necesario verificar read_line == -1 para manejar errores en la lectura, ya que si ocurre un error al leer, la función read devuelve -1, lo que indica un fallo, y no solo el final del archivo (que sería read_line == 0).
			return (0);
		else if (read_line == 0)//Si ya hemos terminado de leer todo el archivo
			break ;
		buf[read_line] = '\0';//Se asigna terminador nulo. Recuerda que read_line es un numero, # de bytes leidos. Por ejempo, "Hol\0"
		if (!backup)//Si el backup está vacío
			backup = ft_strdup("");//Mira en utilities cuáles son los paramentros de strdup? char	*ft_strdup(const char *s). Este paso asegura que backup apunte a una cadena vacía ("") en caso de que sea NULL, lo que permite concatenar correctamente los datos leídos en las siguientes iteraciones sin causar errores al intentar concatenar con un puntero nulo.
		char_temp = backup;//Este paso es necesario para guardar el valor actual de backup en char_temp antes de modificar backup con ft_strjoin. De esta forma, se preserva el contenido de backup (que luego se usará en la concatenación) y se evita sobrescribirlo prematuramente.
		backup = ft_strjoin(char_temp, buf);//ME CUESTIONO LA NECESIDAD DE USAR CHAR_TEMP. Unimos a char_temp lo que hemos leído y lo almacenamos en backup
		//En una segunda iteración, considerando que nuestro archivo texto.txt contiene: "Hola\nmundo." backup = ft_strjoin("Hol", "a\nm") → backup = "Hola\nm". Se concatena el contenido de backup con lo leído en el nuevo bloque
		free(char_temp);//Es necesario liberar la memoria de char_temp para evitar fugas de memoria, ya que char_temp apunta a una cadena que ya no se necesita después de ser utilizada en ft_strjoin.
		char_temp = NULL;//Asignar char_temp = NULL previene el uso accidental de un puntero que ahora apunta a memoria liberada.
		if (ft_strchr (buf, '\n'))//OJO SI APLICAMOS LA MODIFICACION DEL BUCLE WHILE. Si en buf encontramos un salto de línea terminamos el bucle. Esta linea se volvería redundante, ya que el bucle ya se interrumpiría automáticamente cuando se detectara el salto de línea con ft_strchr como esta escrito dentro de la condicion del while.
			break ;
	}
	return (backup);//Una vez encontrado el salto de línea salimos del bucle y devolvemos backup que tiene la línea entera
}

static char	*extract(char *line)
{
	size_t	count;
	char	*backup;

	count = 0;
	while (line[count] != '\n' && line[count] != '\0')//con line = "Hola\nm" Itera sobre los caracteres de line mientras no sean \n ni \0. Resultado: line[0] = 'H', line[1] = 'o', line[2] = 'l', line[3] = 'a' → Incrementa count en cada iteración. line[4] = '\n' → El bucle se detiene aquí. Final: count = 4.
		count++;
	if (line[count] == '\0' || line[1] == '\0')//OJO NECESITA MODIFICACION. Comprueba si no hay \n o si la línea es muy corta. La comprobación de si line[1] == '\0' es innecesaria. Line sería una cadena con un solo carácter, y el primer chequeo line[count] == '\0' ya cubriría ese caso. La comprobación de si hemos llegado al caracter nulo si es interesante. Haber llegado sin haber encontrado un salto de línea significa que ya hemos llegado al final del texto. Backup además no recibiría ya ningun sobrante con substr
		return (0);//Si line[4] fuera el final de la cadena (\0), entonces return (0) indicaría a la función get_next_line que hemos llegado al final del texto, que no hay necesidad de realizar un backup ni extraer más datos, y que podemos retornar line directamente a la función main.
	backup = ft_substr(line, count + 1, ft_strlen(line) - count);//Cortamos la cadena despues del salto de linea enviando a la funcion la linea, la posicion de \n y la longitud de la subcadena a la funcion ft_substr
	if (*backup == '\0')
	{
		free(backup);
		backup = NULL;
	}
	line[count + 1] = '\0';
	return (backup);
}

char	*get_next_line(int fd)
{
	char		*line;
	char		*buf;
	static char	*backup;

	if (fd < 0 || BUFFER_SIZE <= 0)//Si fd es negativo o BUFFER_SIZE es menor o igual a cero, la función devuelve NULL.
		return (0);
	buf = malloc(sizeof(char) * (BUFFER_SIZE + 1));//Se asigna memoria para buf de tamaño BUFFER_SIZE + 1 (4 bytes si BUFFER_SIZE = 3).
	if (!buf)//Si no se pudo asignar memoria para buf, devuelve NULL.
		return (0);
	line = function_name(fd, buf, backup);//Aqui almacenamos la línea completa que hemos leído hasta el salto de línea devuelta por function_name
	free(buf);//Se libera buf porque malloc asigna memoria dinámica para el buffer, y es importante liberar esa memoria después de usarla para evitar fugas de memoria. buf se utiliza para almacenar temporalmente los datos leídos, pero una vez que ya no se necesita, se debe liberar.
	buf = NULL;//El puntero buf se pone a NULL después de liberarlo para evitar que quede apuntando a una dirección de memoria inválida, lo que podría causar errores si intentamos acceder a él más tarde. Sin embargo, este paso no es estrictamente necesario, ya que buf no se usa más después de esta función.
	if (!line)//verifica si el puntero line es NULL. Si es NULL, significa que no se ha podido leer una línea válida y la función retorna NULL. En caso contrario, continúa con la ejecución normal.
		return (NULL);
	backup = extract(line);//Enviamos la línea a la función extract. En nuestro ejemplo, enviamos line = "Hola\nm"
	return (line);//Devuelve a main la linea
}


/*
function_name y extract son static char porque se declaran como funciones internas a un archivo, lo que significa que solo pueden ser utilizadas dentro del mismo archivo y no son accesibles desde otros archivos del programa.
Quiza por eso no hace falta tenerlas en el header.
En C, una variable estática es aquella que mantiene su valor entre llamadas a la función y que solo es accesible dentro del bloque donde fue declarada. Se declara con la palabra clave static. También puede limitar el alcance de variables globales a un solo archivo, útil en modularización.
Una variable global en C es una que se declara fuera de cualquier función y es accesible por todas las funciones del programa. En este contexto, un "bloque" es un conjunto de instrucciones delimitado por llaves { }, como en una función o en estructuras de control (if, for, etc.).
Una variable estática declarada dentro de una función solo puede usarse en esa función; otras funciones no pueden acceder a ella. La variable persiste entre llamadas, pero sigue siendo "local" a su bloque de declaración (es decir, a esa función).
Las flags (-Wall, -Wextra, -Werror) son opciones que se pasan al compilador (cc en este caso) para ajustar el nivel de advertencias o errores al compilar. Estas flags significan:

-Wall: Activa todas las advertencias básicas del compilador.
-Wextra: Activa advertencias adicionales para código potencialmente problemático.
-Werror: Convierte todas las advertencias en errores, obligando a corregirlas para compilar.
Relink significa recompilar un programa incluso si no hay cambios en el código. Tu Makefile debe evitar recompilar innecesariamente si no hay cambios.

Un compilador es un programa que traduce código fuente (como C) a código máquina ejecutable por la computadora. No solo junta archivos, sino que convierte el código de alto nivel en instrucciones comprensibles por el hardware, generando un archivo ejecutable a partir de los archivos fuente.
El compilador transforma variables, funciones y otras estructuras en lenguaje ensamblador y luego en código máquina (binario).
En la práctica, read() es la función que lee los datos de un archivo. El archivo se abre con un descriptor de archivo (fd), y read() extrae los bytes del archivo en bloques.

Tu función get_next_line() usará read() para leer el archivo y luego procesará esos bloques para extraer una línea completa. Así que, read() lee el archivo, y get_next_line() organiza los datos en líneas.
El archivo que lees con read() puede contener cualquier tipo de datos: texto, números, imágenes, binarios, etc. read() simplemente lee bytes del archivo sin preocuparse por su tipo. Es tu responsabilidad interpretar esos bytes, ya sea como texto (con caracteres y líneas) o como otro tipo de datos según lo que necesites.
read() lee bytes del archivo. Los archivos son secuencias de bytes y, al usar read(), estás leyendo estos bytes en un buffer (un área de memoria temporal). Luego, tú decides cómo interpretar esos bytes, por ejemplo, como caracteres de texto o datos binarios.
El proyecto te pide crear una función que lea archivos de texto (como .txt), no archivos binarios como un documento de Word (.doc o .docx). Los archivos de Word contienen datos binarios complejos que requieren un tratamiento especial para ser leídos y entendidos. En cambio, la función que debes crear solo se encargará de leer archivos de texto donde los datos son representados como caracteres y líneas, devolviendo una línea cada vez.
El valor añadido que le das al ordenador al crear esta función radica en que le proporcionas una capacidad específica y controlada para leer líneas de texto de manera eficiente, en lugar de leer el archivo completo de una vez. Esto es útil cuando:

El archivo es grande y no quieres cargarlo todo en memoria de una sola vez (lo cual puede ser ineficiente o inviable en archivos muy grandes).
Necesitas procesar o trabajar con los datos de forma incremental, es decir, leer una línea, procesarla y luego leer la siguiente.
El manejo de memoria y recursos se hace más efectivo, al leer solo lo necesario en cada momento.
En resumen, la función agrega un control preciso sobre cómo leer un archivo línea por línea, sin tener que cargar todo el archivo de una vez, lo cual es más eficiente y flexible.
fd es un número que identifica un archivo o recurso abierto.
No solo se usa para archivos, sino también para otros recursos como el teclado (entrada estándar) o pantalla (salida estándar).
Cuando abres un archivo, el sistema te da un fd que representa ese archivo. Ese número es lo que pasas a get_next_line() para leer el archivo.

Ejemplo:

fd = 0: Entrada estándar (teclado).
fd = 1: Salida estándar (pantalla).
fd = 3: Un archivo abierto
La funcion puede leer tanto desde un archivo como desde la entrada estándar (el teclado), dependiendo del descriptor de archivo (fd) que se le pase. El fd es el número que representa el archivo o recurso de donde queremos leer.
Si ejecutas el programa desde la terminal y usas get_next_line(0), leerá lo que escribas en el teclado (entrada estándar).
Si abres un archivo con open("archivo.txt", O_RDONLY), obtienes un fd (por ejemplo, 3), y luego puedes leer de ese archivo.
Lo que necesitamos hacer en get_next_line():
Leer desde el archivo o la entrada estándar usando read().
Almacenar temporalmente los datos leídos hasta encontrar una nueva línea (\n) o el final del archivo.
Devolver la línea completa que hemos leído, incluyendo el salto de línea si es necesario.
Manejar memoria adecuadamente para almacenar las líneas leídas.

line:
Objetivo del proyecto: Retornar una línea de texto leída desde el archivo o entrada. Se retorna a la funcion main que quiza la imprima.
Cómo ayuda: Almacena la línea completa que get_next_line() va a devolver. Esta línea incluye el texto hasta el primer salto de línea (\n) o hasta el final del archivo.
buf:
Objetivo del proyecto: Leer el archivo en bloques para manejar archivos grandes de forma eficiente.
Cómo ayuda: buf es el buffer temporal que almacena cada bloque de datos leídos del archivo. La función lee en trozos de tamaño BUFFER_SIZE en buf, y luego usa estos datos para construir la línea.
backup (variable estática):
Objetivo del proyecto: Conservar datos parciales entre llamadas consecutivas a get_next_line().
Cómo ayuda: backup almacena cualquier contenido sobrante de una lectura anterior que aún no se ha devuelto. Esto permite que la función continúe desde el punto en que quedó en cada llamada, facilitando la lectura línea por línea sin perder datos.
El objetivo de extract es truncar line hasta el salto de línea y guardar en backup el contenido sobrante, si lo hay.
#include <fcntl.h>
#include <stdio.h>
#include "get_next_line.h"

int main(void)
{
    int fd = open("archivo.txt", O_RDONLY);
    char *line;

    if (fd < 0)
        return (1);
    while ((line = get_next_line(fd)) != NULL)
    {
        printf("%s", line);
        free(line);
    }
    close(fd);
    return (0);
}