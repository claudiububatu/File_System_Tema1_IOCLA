#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

#define MAX_STRING_SIZE 100

#define MAX_COMMAND_SIZE 16

struct Dir;
struct File;

typedef struct Dir{
	char *name;
	struct Dir* parent; // pointer catre parintele directorului(null pentru radacina)
	struct File* head_children_files; // pointer catre primul element de tip File din interiorul directorului
	struct Dir* head_children_dirs; // pointer catre primul element de tip Dir din interiorul directorului
	struct Dir* next; 
} Dir;

typedef struct File {
	char *name;
	struct Dir* parent;
	struct File* next;
} File;


// functie pentru a seta toate legaturile unui director pe NULL
void make_null(Dir *aux) {
	aux->parent->head_children_dirs == NULL;
	aux->next = NULL;
	aux->head_children_dirs = NULL;
	aux->head_children_files = NULL;
	aux->parent = NULL;
}

// functie ce aloca spatiu pentru o structura de tip Dir
// si pentru numele acesteia
Dir *createdir(char *name) {
	Dir *dir = calloc(1, sizeof(Dir));
	dir->name = calloc(strlen(name) + 1, sizeof(char));
	strcpy(dir->name, name);
	make_null(dir);
	return dir;
}

// functie ce aloca spatiu pentru o structura de tip File
// si pentru numele acesteia
File *createfile(char *name) {
	File *f = calloc(1, sizeof(File));
	f->name = calloc(strlen(name) + 1, sizeof(char));
	strcpy(f->name, name);
	return f;
}

void touch (Dir* parent, char* name) {
	File* file = parent->head_children_files;

	// verific daca lista de fisiere este goala. Daca este, creez primul
	// fisier din lista de fisiere-copii a directorului parent

	if (file == NULL) {
		File *create = createfile(name);
		create->parent = parent;
		parent->head_children_files = create;
		return;
	}

	// declar o structura de tip File pentru a retine ultimul fisier din lista
	File *f;

	while (file != NULL) {
		if (strcmp(file->name,name) == 0) {
			printf("File already exists\n");
			return;
		}
		if (file->next != NULL)
			file = file->next;
		else {
			f = file;
			file = file->next;
		}
	}

	// adaug un fisier nou cu numele name la finalul listei
	File *create = createfile(name);
	create->parent = parent;
	f->next = create;
}

void mkdir (Dir* parent, char* name) {
	Dir *d = parent->head_children_dirs;
	Dir *create;

	// verific daca lista de directoare este goala. Daca este, creez primul
	// director din lista de directoare-copii a directorului parent
	if (d == NULL) {
		create = createdir(name);
		create->parent = parent;
		parent->head_children_dirs = create;
		return;
	}

	// declar o structura de tip Dir pentru a retine ultimul fisier din lista
	Dir *u;

	while (d != NULL) {
		if (strcmp(d->name,name) == 0) {
			printf("Directory already exists\n");
			return;
		}
		if (d->next != NULL)
			d = d->next;
		else {
			u = d;
			d = d->next;
		}
	}

	// adaug un director nou cu numele name la finalul listei
	create = createdir(name);
	create->parent = parent;
	u->next = create;
}

void ls (Dir* parent) {

	// iau head-urile lui parent pentru directoare si fisiere
	// si le salvez in d, respectiv f 
	Dir *d = parent->head_children_dirs;
	File *f = parent->head_children_files;

	// mai intai afisez lista de directoare-copii
	while (d != NULL) {
		printf("%s\n", d->name);
		d = d->next;
	}

	// apoi afisez lista de fisiere-copii
	while (f != NULL) {
		printf("%s\n", f->name);
		f = f->next;
	}

}

void rm (Dir* parent, char* name) {
	File *f = parent->head_children_files;
	File *prev_f = f; // pointer ce o sa indice cu o pozitie in spatele lui f
	File *aux; // voi salva fisierul ce trebuie sters in aux si il voi elibera
	File *err = f; // pointer pt a parcurge lista de fisiere si pentru a vedea
	// daca exista vreun fisier cu numele name in lista de fisiere-copii
	int ok = 0; // daca s-a gasit vreun fisier cu numele name, ok va deveni 1

	while (err != NULL) {
		if (!strcmp(err->name, name)) {
			ok = 1;
			break;
		} 
		err = err->next;
	}

	if (ok == 0) {
		printf("Could not find the file\n");
		return;
	}

	// eliberam la inceput pe prima pozitie
	if (!strcmp(f->name, name)) {
		aux = parent->head_children_files;
		parent->head_children_files = f->next;
		free(aux->name);
		free(aux);
	} else {
		// eliberam pe orice alta pozitie
		while (f != NULL && strcmp(f->name, name)) {
			prev_f = f;
			f = f->next;
		}
		
		prev_f->next = f->next;
		free(f->name);
		free(f);
	}

}

// functie ce sterge toate fisierele dintr-o lista
void rm_files(Dir *parent) {
	if (parent == NULL) {
		return;
	}
	while (parent->head_children_files != NULL) {
		// aici sterg mereu de pe prima pozitie pana nu mai am
		// ce sa sterg
		rm(parent, parent->head_children_files->name);
	}
}

// functie recursiva prin care sterg tot pe rutele
// ->next si ->head_children_dirs
void rmv(Dir *parent) {
	Dir* aux = NULL; // in aux o sa salvez directorul care trebuie sters

	// verific mereu daca parintele este NULL
	// daca este, dau return	
	if (parent == NULL) {
		return;
	}

	while (parent != NULL) {
		aux = parent;

		if (parent->head_children_dirs != NULL) {
			// sterg toate fisierele din folder-ul parent
			rm_files(parent);
			// ma duc recursiv in subdirectoare si sterg toate fisierele
			// din acestea
			rmv(parent->head_children_dirs);
			
		}

		// cand nu mai pot sa ma duc in alte subdirectoare, incep sa ma plimb
		// pe ruta ->next si verific ulterior daca pot sa ma duc iar in 
		// subidrectoarele lui ->next
		parent = parent->next;
		if (aux != NULL) {
			make_null(aux);
			// eliberez memoria pentru vechiul pointer al lui parent
			free(aux->name);
			free(aux);
		}
	}
}

void rmdir (Dir* parent, char* name) {
	Dir *d = parent->head_children_dirs; // head catre primul director al lui home
	Dir *err = d; // pointer pt a parcurge lista de directoare si pentru a vedea
	// daca exista vreun director cu numele name in lista de directoare-copii
	Dir *prev_d; // pointer ce o sa indice cu o pozitie in spatele lui d
	int ok = 0; // daca s-a gasit vreun director cu numele name, ok va deveni 1

	while (err != NULL) {
		if (strcmp(err->name, name) == 0) {
			ok = 1;
			break;
		} 
		err = err->next;
	}

	if (ok == 0) {
		printf("Could not find the dir\n");
		return;
	}

	while (d != NULL && strcmp(d->name, name)) {
		prev_d = d;
		d = d->next;
		// am ajuns in dreptul directorului pe care vreau sa il sterg
	}

	rmv(d->head_children_dirs);
	// apelez functia rmv in acest fel deoarece asa cum am mentionat mai devreme
	// functia rmv sterge TOATE directoarele pe rutele ->next si ->head_children_dirs
	// asadar, apeland functia in acest fel vom sterge tot ce contine
	// directorul d care se doreste a fi sters, urmand sa ne ocupam ulterior
	// si de stergerea directorului d si a fisierelor pe care acesta le contine

	// exact ca la rm, ne intereseaza cazurile in care stergem un director
	// de pe prima pozitie; celelalte cazuri pot fi incadrate la else
	if (d == parent->head_children_dirs) {
		parent->head_children_dirs = d->next;
	} else {
		prev_d->next = d->next;
	}
	rm_files(d); // functie pentru stergerea fisierelor din d
	free(d->name); // eliberez memoria alocata pentru numele directorului
	free(d); // eliberez memoria alocata pentru director
}

void cd(Dir** target, char *name) {
	Dir *d = (*target)->head_children_dirs;
	Dir *err = (*target)->head_children_dirs; // pointer pt a parcurge lista de directoare si pentru a vedea
	// daca exista vreun director cu numele name in lista de directoare-copii
	int ok = 0; // daca s-a gasit vreun director cu numele name, ok va deveni 1

	while (err != NULL) {
		if (!strcmp(err->name, name)) {
			ok = 1;
			break;
		} 
		err = err->next;
	}
	
	if (ok == 0) {
		// pentru cazul in care vrem sa mergem "inapoi" in lista de directoare
		// daca rulam comanda "cd .." programul va intra in if-ul de mai jos,
		// iar directorul-copil va deveni director-parinte
		if (strcmp(name, "..") == 0) {
			// trebuie sa ne asiguram ca o sa existe un parinte
			// pentru directorul curent pentru a nu patrunde in NULL
            if ((*target)->parent != NULL) {
			    (*target) = (*target)->parent;
			    return;
            }
		} else {
			printf("No directories found!\n");
			return;
		}
	} else if (ok == 1) {
		// daca s-a gasit un director cu numele name, ne ducem la el
		while (d != NULL && strcmp(d->name, name)) {
			d = d->next;
		}
		// si il accesam
		*target = d;
	}
}

char* pwd (Dir* target) {

	char *str = calloc(1, MAX_INPUT_LINE_SIZE); // aloc memorie unui string
	char *reverse = calloc(1, MAX_INPUT_LINE_SIZE); // aloc memorie unui string
	// ce va fi inversul lui str
    int len, i, index, wordStart, wordEnd;

	// creez calea catre directorul meu invers fata de cum as vrea sa fie
	strcpy(str, target->name);
	while (target->parent != NULL) {
		strcat(str, "/");
		strcat(str, target->parent->name);
		target = target->parent;
	}

	strcat(str, "/");

    len   = strlen(str);
    index = 0;

	// incep sa caut cuvinte de la sfarsitul lui str
    wordStart = len-1;
    wordEnd = len-1;

    while (wordStart > 0) {

		// daca gasesc un cuvant ce este delimitat de /
        if (str[wordStart] == '/') {

			// adaug cuvantul la reverse
            i = wordStart + 1;
            while (i <= wordEnd) {
                reverse[index] = str[i];
                i++;
                index++;
            }
            reverse[index++] = '/';

            wordEnd = wordStart - 1;
        }

        wordStart--;
    }

	// adaug si ultimul cuvant
    for (i = 0; i <= wordEnd; i++) {
        reverse[index] = str[i];
        index++;
    }

	// adaug terminatorul de sir la sfarsitul stringului reverse
    reverse[index] = '\0'; 

	free(str); // eliberez str dupa ce am construit reverse-ul lui cum doream
	return reverse;
}

// functie pentru a afisa toate fisierele pe nivel
void shw_files(File *target, int level) {
	if (target == NULL) {
		return;
	}

	int aux = level;

	while (aux != 0) {
		printf("    ");
		aux--;
	}

	printf("%s\n", target->name);

	while (target != NULL) {
		target = target->next;
		shw_files(target, level);
	}
}

void tree (Dir* target, int level) {
	// verific daca target e NULL
	if (target == NULL) {
		return;
	}

	// iau o valoare auxiliara in care sa salvez level
	int aux = level;

	// pun 4 spatii x level (aux)
	while (aux != 0) {
		printf("    ");
		aux--;
	}

	printf("%s\n", target->name);

	if (target->head_children_dirs != NULL) {
		// ma duc recursiv pe ruta ->head_children_dirs
		// si cresc level-ul la fiecare pas
		tree(target->head_children_dirs, level + 1);
	}

	while (target != NULL) {
		if (target->head_children_dirs != NULL) {
			// ma duc pe ruta ->next si pastrez valoarea level-ului
			// pentru a pastra spatierea corecta
			tree(target->next, level);
		}
		// apelez functia recursiva shw_files pentru a afisa corespunzator
		// fisierele de pe fiecare nivel
		shw_files(target->parent->head_children_files, level);
		target = target->next;
	}
}

// functie ce verifica daca exista in listele de directoare/fisiere
// un alt director/fisier cu acelasi nume; daca exista, atunci ok = 1
// altfel ok = 0
void err(Dir *parent, char *name, int *ok) {
	Dir *err_d = parent->head_children_dirs;
	File *err_f = parent->head_children_files;

	if (err_d != NULL || err_f != NULL) {
		*ok = 0;
		while (err_d != NULL) {
			if (strcmp(err_d->name, name) == 0) {
				*ok = 1;
				break;
			}
			err_d = err_d->next;
		}

		while (err_f != NULL) {
			if (strcmp(err_f->name, name) == 0) {
				*ok = 1;
				break;
			}
			err_f = err_f->next;
		}
	}
}

// functie care sterge un fisier din lista pentru mv
void mv_remove_file(Dir *parent, char *name) {
	File *f = parent->head_children_files;
	while (f != NULL && strcmp(f->name, name)) {
		f = f->next;
	}
	rm(parent, f->name);
}

// functie care adauga un fisier in lista pentru mv
void mv_add_file(Dir *parent, char *name) {
	File *f = parent->head_children_files;
	while (f != NULL) {
		f = f->next;
	}
	touch(parent, name);
}

// functie care sterge un director din lista pentru mv
void mv_remove_dir(Dir *parent, char *name) {
	Dir *d = parent->head_children_dirs;
	while (d != NULL && strcmp(d->name, name)) {
		d = d->next;
	}
	rmdir(parent, d->name);
}

// functie care adauga un director in lista pentru mv
void mv_add_dir(Dir *parent, char *name) {
	Dir *d = parent->head_children_dirs;
	while (d != NULL) {
		d = d->next;
	}
	mkdir(parent, name);
}

// functie care confirma faptul ca operatia de mv
// se va face asupra unui fisier (daca ok = 1)
int *ok_file(Dir *parent, char *name, int *ok) {
	File *f = parent->head_children_files;
	*ok = 0;
	while (f != NULL) {
		if (strcmp(f->name, name) == 0) {
			*ok = 1;
			break;
		}
		f = f->next;
	}
	return ok;
}

// functie care confirma faptul ca operatia de mv
// se va face asupra unui director (daca ok = 1)
int *ok_dir(Dir *parent, char *name, int *ok) {
	Dir *d = parent->head_children_dirs;
	*ok = 0;
	while (d != NULL) {
		if (strcmp(d->name, name) == 0) {
			*ok = 1;
			break;
		}
		d = d->next;
	}
	return ok;
}

void mv(Dir* parent, char *oldname, char *newname) {
	int ok;
	err(parent, oldname, &ok); // verific daca exista un fisier/director
	// cu numele oldname in directorul parent
	
	if (ok == 0) {
		printf("File/Director not found\n");
		return;
	}

	err(parent, newname, &ok); // verific daca exista un fisier/director
	// cu numele newname in directorul parent

	if (ok == 1) {
		printf("File/Director already exists\n");
		return;
	}
	
	// daca se gaseste in directorul parent un fisier cu numele oldname
	// atunci se efectueaza operatiile specifice unui fisier
	if (*ok_file(parent, oldname, &ok) == 1) {
		mv_remove_file(parent, oldname);
		mv_add_file(parent, newname);
	} else if (*ok_dir(parent, oldname, &ok) == 1) {
		// in caz contrar, se efectueaza operatiile specifice unui director
		mv_remove_dir(parent, oldname);
		mv_add_dir(parent, newname);
	}
}

void stop (Dir* target) {
	// sterg mai intai toate fisierele din target daca exista
	while (target->head_children_files != NULL) {
		rm(target, target->head_children_files->name);
	}

	// sterg toate directoarele din target (in caz ca exista)
	// si toate fisierele din subdirectoare (in caz ca exista)
	while (target->head_children_dirs != NULL) {
		rmdir(target, target->head_children_dirs->name);
	}
}

int main () {

	
	Dir *root = createdir("home"); // va pointa mereu catre home
	// este necesar sa pointeze astfel in momentul eliberarii memoriei

	Dir *root_2 = root;	// acesta este un director "plimbaret"
	// care pleaca din home

	do
	{
		char *command =  calloc(1, MAX_COMMAND_SIZE); // aloc memorie pentru comanda
		char *name = calloc(1, MAX_STRING_SIZE); // aloc memorie pentru numele pe care
		// il primesc ca parametru functiile de mai jos

		scanf("%s", command); // citesc comanda si o compar cu comenzile ce s-au cerut
		// a fi implementate in cerinta temei; in cazul in care comenzile sunt celelalte
		// fata de cele de mai jos, programul nu va face nimic
		if (!strcmp(command, "touch")) {
			scanf("%s", name);
			touch(root_2, name);
		}

		if (!strcmp(command, "mkdir")) {
			scanf("%s", name);
			mkdir(root_2, name);
		}


		if (!strcmp(command, "ls")) {
			ls(root_2);
		}

		if (!strcmp(command, "rm")) {
			scanf("%s", name);
			rm(root_2, name);
		}

		if (!strcmp(command, "cd")) {
			scanf("%s", name);
			cd(&root_2, name);
		}

		if (!strcmp(command, "pwd")) {
			char *res = pwd(root_2);
			printf("%s\n", res);
			free(res);
		}

		if (!strcmp(command, "rmdir")) {
			scanf("%s", name);
			rmdir(root_2, name);
		}

		if (!strcmp(command, "tree")) {
			tree(root_2->head_children_dirs, 0);
		}

		if (!strcmp(command, "mv")) {
			char *newname = calloc(sizeof(char), MAX_STRING_SIZE);
			scanf("%s", name); // joaca rol de oldname
			scanf("%s", newname);
			mv(root_2, name, newname);
			free(newname);
		}

		if (!strcmp(command, "stop")) {
			free(command);
			free(name);
			stop(root);
			break;
		}

		free(command);
		free(name);
		
	} while (1);
	
	free(root->name);
	free(root);

	return 0;
}