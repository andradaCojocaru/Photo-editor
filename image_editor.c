//Cojocaru Andrada 312CA
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NMAX 300
typedef struct photo // structura fisier photo
{
	char magic_word[NMAX];
	int width, height, maxx;
	unsigned char **ph;
} photo;

typedef struct sel // structura selectie
{
	int x1, y1, x2, y2;
} sel;

int maxx(int a, int b) // functie de maxim
{
	if (a > b)
		return a;
	return b;
}

void swapp(int *a, int *b) // functie de interschimbare
{
	int aux;
	aux = *a;
	*a = *b;
	*b = aux;
}

unsigned char **aloc_matrix(int n, int m, int colour) // alocare dinamica
{
	unsigned char **mat;
	mat = (unsigned char **)malloc(n * sizeof(unsigned char *));
	if (!mat)
		return NULL;
	for (int i = 0; i < n; i++) {
		mat[i] = (unsigned char *)malloc(m * colour * sizeof(unsigned char));
		if (!mat[i])
			return NULL;
	}
	return mat;
}

void free_memory(unsigned char **mat, int m) // eliberare memorie
{
	for (int i = 0; i < m; i++)
		free(mat[i]);
	free(mat);
}

int load(photo *p, char *file1, int *colour) // functie citire din fisier
{
	int m;
	FILE *fin;
	fin = fopen(file1, "rb");
	if (!fin) {
		printf("Failed to load %s\n", file1);
		return 0;
	}
	fscanf(fin, "%s", p->magic_word);
	if (strcmp(p->magic_word, "P2") == 0 || strcmp(p->magic_word, "P5") == 0)
		*colour = 1; // variabila ce retine tipul pozei
	else
		*colour = 3;
	fscanf(fin, "%d %d\n%d\n", &p->width, &p->height, &p->maxx);
	m = maxx(p->width, p->height) + 5;
	p->ph = aloc_matrix(m, m, *colour);
	if (strcmp(p->magic_word, "P5") == 0 || strcmp(p->magic_word, "P6") == 0) {
		for (int i = 0; i < p->height; i++) {
			for (int j = 0; j < *colour * p->width; j++)
				fread(&p->ph[i][j], sizeof(unsigned char), 1, fin);
		}       //citire binar
	} else {
		for (int i = 0; i < p->height; i++)
			for (int j = 0; j < *colour * p->width; j++)
				fscanf(fin, "%hhu", &p->ph[i][j]);
	} // citire ascii
	printf("Loaded %s\n", file1);
	fclose(fin);
	return 1;
}

int selection(photo *p, sel *s, int x1, int y1, int x2, int y2, int okl)
{
	if (okl == 0) {
		printf("No image loaded\n");
		return 0;
	}
	if (x1 < 0 || x1 > p->width || x2 < 0 || x2 > p->width || y1 < 0 ||
		y1 > p->height || y2 < 0 || y2 > p->height || x1 == x2 || y1 == y2) {
		printf("Invalid set of coordinates\n");
		return 0;
	}
	s->x1 = x1; // implementarea valorilor
	s->y1 = y1;
	s->x2 = x2;
	s->y2 = y2;
	if (x1 > x2) { // schimbarea valorilor daca punctele sunt interschimbate
		s->x1 = x2;
		s->x2 = x1;
	}
	if (y1 > y2) {
		s->y1 = y2;
		s->y2 = y1;
	}
	printf("Selected %d %d %d %d\n", s->x1, s->y1, s->x2, s->y2);
	return 1;
}

int select_all(photo *p, sel *s, int okl)
{
	if (okl == 0) {
		printf("No image loaded\n");
		return 0;
	}
	s->x1 = 0; // selectia totala
	s->y1 = 0;
	s->y2 = p->height;
	s->x2 = p->width;
	return 1;
}

int rotate(photo *p, sel *s, int angle, int okl, int colour)
{
	int m, sign = 1, nr, selall = 0, height, width;
	unsigned char **mat, **matc;
	if (okl == 0) {
		printf("No image loaded\n");
		return 0;
	}
	if (angle % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return 0;
	}
	height = s->y2 - s->y1; // dimensiunile selectiei
	width = s->x2 - s->x1;
	if (!(s->x1 == 0 && s->y1 == 0 && s->x2 == p->width &&
		  s->y2 == p->height)) {
		selall = 0;
		if (height != width) {
			printf("The selection must be square\n");
			return 0;
		} // verificarea selectiei partiale
	} else {
		selall = 1;
	}
	if (angle < 0)
		sign = -1;
	angle = angle * sign;
	nr = angle / 90;
	nr = nr % 4;
	if (sign == -1)
		nr = 4 - nr;
	m = maxx(p->width, p->height) + 5;
	mat = aloc_matrix(m, m, colour);
	matc = aloc_matrix(m, m, colour);           // alocare copii matrice
	for (int i = s->y1; i < s->y2; i++)         // copiere in matricea copie
		for (int j = colour * s->x1; j < colour * s->x2; j++)
			mat[i - s->y1][j - colour * s->x1] = p->ph[i][j];
	while (nr) { // algoritm repetitiv rotire
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < colour * width; j = j + colour) {
				for (int k = 0; k < colour; k++) {
					matc[j / colour][colour * (height - i - 1) + k] =
						mat[i][j + k];
				}
			}
		}
		if (selall == 1)
			swapp(&height, &width);
		for (int i = 0; i < height; i++)
			for (int j = 0; j < colour * width; j++)
				mat[i][j] = matc[i][j];
		nr--;
	}
	if (selall == 1) {             // interschimbare dimensiuni matrice
		if (p->height != height) { // in cazul selectiei totale
			swapp(&p->height, &p->width);
			swapp(&s->x2, &s->y2);
		}
	}
	for (int i = s->y1; i < s->y2; i++)
		for (int j = colour * s->x1; j < colour * s->x2; j++)
			p->ph[i][j] = mat[i - s->y1][j - colour * s->x1];
	free_memory(mat, m); // eliberare memorie matrici
	free_memory(matc, m);
	printf("Rotated %d\n", sign * angle);
	return 1;
}

int crop(photo *p, sel *s, int okl, int colour)
{
	int m, mm;
	unsigned char **mat;
	m = maxx(p->width, p->height) + 5;
	p->height = s->y2 - s->y1; // noile dimensiuni ale matricei
	p->width = s->x2 - s->x1;
	mm = maxx(p->width, p->height) + 5;
	if (okl == 0) {
		printf("No image loaded\n");
		return 0;
	}
	mat = aloc_matrix(mm, mm, colour); // alocare copie matrice
	for (int i = s->y1; i < s->y2; i++)     // copiere valori
		for (int j = colour * s->x1; j < colour * s->x2; j++)
			mat[i - s->y1][j - colour * s->x1] = p->ph[i][j];
	free_memory(p->ph, m); // eliberare memorie
	p->ph = aloc_matrix(mm, mm, colour);
	for (int i = 0; i < p->height; i++)
		for (int j = 0; j < colour * p->width; j++)
			p->ph[i][j] = mat[i][j];
			// copiere inapoi in vechea matrice realocata
	printf("Image cropped\n");
	free_memory(mat, mm);
	return 1;
}

int grayscale(photo *p, sel *s, int okl, int colour)
{
	if (okl == 0) {
		printf("No image loaded\n");
		return 0;
	}
	if (colour == 1) {
		printf("Grayscale filter not available\n");
		return 0;
	}
	for (int i = s->y1; i < s->y2; i++) { // aplicare filtru de culori
		for (int j = colour * s->x1; j < colour * s->x2; j = j + colour) {
			double sum = ((double)p->ph[i][j] + (double)p->ph[i][j + 1] +
						  (double)p->ph[i][j + 2]) /
						 3;
			p->ph[i][j] = (unsigned char)round(sum);
			p->ph[i][j + 1] = (unsigned char)round(sum);
			p->ph[i][j + 2] = (unsigned char)round(sum);
		}
	}
	printf("Grayscale filter applied\n");
	return 1;
}

int sepia(photo *p, sel *s, int okl, int colour)
{
	if (okl == 0) {
		printf("No image loaded\n");
		return 0;
	}
	if (colour == 1) {
		printf("Sepia filter not available\n");
		return 0;
	}
	for (int i = s->y1; i < s->y2; i++) { // aplicare filtru de culori
		for (int j = colour * s->x1; j < colour * s->x2; j = j + colour) {
			double r, g, b, nr, ng, nb;
			r = (double)p->ph[i][j];
			g = (double)p->ph[i][j + 1];
			b = (double)p->ph[i][j + 2];
			nr = 0.393 * r + 0.769 * g + 0.189 * b;
			ng = 0.349 * r + 0.686 * g + 0.168 * b;
			nb = 0.272 * r + 0.534 * g + 0.131 * b;
			p->ph[i][j] = (unsigned char)round(nr);
			p->ph[i][j + 1] = (unsigned char)round(ng);
			p->ph[i][j + 2] = (unsigned char)round(nb);
			if (nr > p->maxx)
				p->ph[i][j] = (unsigned char)p->maxx;
			if (ng > p->maxx)
				p->ph[i][j + 1] = (unsigned char)p->maxx;
			if (nb > p->maxx)
				p->ph[i][j + 2] = (unsigned char)p->maxx;
		}
	}
	printf("Sepia filter applied\n");
	return 1;
}

int save(photo *p, char *file2, int as, int colour)
{
	char m[5];
	strcpy(m, ".");
	strcpy(m, p->magic_word);
	FILE *fout;
	fout = fopen(file2, "wb");
	if (!fout)
		return 0;
	if (as == 0) { // scriere in binar
		if (strcmp(m, "P2") == 0 || strcmp(m, "P5") == 0)
			strcpy(m, "P5");
		else if (strcmp(m, "P3") == 0 || strcmp(m, "P6") == 0)
			strcpy(m, "P6");
		fprintf(fout, "%s\n%d %d\n%d\n", m, p->width, p->height, p->maxx);
		for (int i = 0; i < p->height; i++)
			for (int j = 0; j < colour * p->width; j++)
				fwrite(&p->ph[i][j], sizeof(unsigned char), 1, fout);
	} else { // scriere in ascii
		if (strcmp(m, "P2") == 0 || strcmp(m, "P5") == 0)
			strcpy(m, "P2");
		else if (strcmp(m, "P3") == 0 || strcmp(m, "P6") == 0)
			strcpy(m, "P3");
		fprintf(fout, "%s\n%d %d\n%d\n", m, p->width, p->height, p->maxx);
		for (int i = 0; i < p->height; i++) {
			for (int j = 0; j < colour * p->width; j++)
				fprintf(fout, "%d ", p->ph[i][j]);
			fprintf(fout, "\n");
		}
	}
	fclose(fout);
	printf("Saved %s\n", file2);
	return 1;
}

void exitp(photo *p)
{
	int m = maxx(p->width, p->height) + 5;
	free_memory(p->ph, m); // dealocare de memorie
}

int main(void)
{
	photo p;
	sel s;
	char line[NMAX], file1[NMAX], file2[NMAX], type[NMAX], *t, command[NMAX],
		line_new[NMAX];
	int x1, x2, y1, y2, angle, okl = 0, ok = 0, colour = 0, count, as = -1, m;
	fgets(line, NMAX, stdin); // parsare string comenzi
	strcpy(line_new, line);
	t = strtok(line, "\n ");
	strcpy(command, t);
	while (strcmp(command, "EXIT") != 0) {
		if (strcmp(command, "LOAD") == 0) {
			count = sscanf(line_new, "%s %s", command, file1);
			if (okl != 0) {
				m = maxx(p.width, p.height) + 5;
				free_memory(p.ph, m);
			}
			if (count != 2) {
				printf("Invalid command\n");
			} else {
				okl = load(&p, file1, &colour);
				if (okl != 0)
					select_all(&p, &s, okl);
			}
		} else if (strcmp(command, "SELECT") == 0) {
			t = strtok(NULL, "\n ");
			strcpy(command, t);
			if (strcmp(command, "ALL") == 0) {
				select_all(&p, &s, okl);
				if (okl == 1)
					printf("Selected ALL\n");
			} else {
				count = sscanf(line_new, "%s %d %d %d %d", command, &x1, &y1,
							   &x2, &y2);
				if (count != 5)
					printf("Invalid command\n");
				else
					selection(&p, &s, x1, y1, x2, y2, okl);
			}
		} else if (strcmp(command, "ROTATE") == 0) {
			count = sscanf(line_new, "%s %d", command, &angle);
			if (count != 2)
				printf("Invalid command\n");
			else
				ok = rotate(&p, &s, angle, okl, colour);
		} else if (strcmp(command, "CROP") == 0) {
			ok = crop(&p, &s, okl, colour);
			if (ok == 1)
				select_all(&p, &s, okl);
		} else if (strcmp(command, "GRAYSCALE") == 0) {
			ok = grayscale(&p, &s, okl, colour);
		} else if (strcmp(command, "SEPIA") == 0) {
			ok = sepia(&p, &s, okl, colour);
		} else if (strcmp(command, "SAVE") == 0) {
			count = sscanf(line_new, "%s %s %s", command, file2, type);
			if (count == 3 && strcmp(type, "ascii") == 0)
				as = 1;
			else
				as = 0;
			if (okl == 1)
				save(&p, file2, as, colour);
			else
				printf("No image loaded\n");
		} else {
			printf("Invalid command\n");
		}
		fgets(line, NMAX, stdin);
		strcpy(line_new, line);
		t = strtok(line, "\n ");
		strcpy(command, t);
	}
	if (okl != 0)
		exitp(&p);
	else
		printf("No image loaded");
	return 0;
}
