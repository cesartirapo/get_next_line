$(NAME): Define el nombre del ejecutable final.
all: Compila el proyecto completo generando el ejecutable ($(NAME)).
clean: Elimina archivos intermedios generados en la compilación (como objetos .o).
fclean: Elimina todos los archivos generados, incluyendo el ejecutable.
re: Ejecuta fclean y luego all, recompilando todo desde cero.