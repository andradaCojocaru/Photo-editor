build:
	gcc -Wall -Wextra -g -s image_editor.c -o image_editor -lm
clean:
	rm -rf image_editor
