#include "../include/Commands.h"

int cd(int argc, char **argv) {
	char *s;

	if (argc == 1) {
		s = getenv("HOME");

		if (s == NULL) {
            Common::Error("Where is your home, stranger?");
            return 1;
        }
		else {
            chdir(s);
        }
	}
	else if (argc > 2) {
        Common::Error("Cd command accepts only 1 argument");
        return 2;
    }
    else if (chdir(argv[1])) {
        perror(argv[1]);
        return 3;
    }

    return 0;
}

void cmp(int argc, char **argv) {
    if (argc == 1) {
        Common::Error("Shell_cmp: missing operand after 'cmp'");
        exit(1);
    }

    if (argc == 2) {
        argv[argc] = (char*)malloc(2 * sizeof(char));
        strcpy(argv[argc], "-");

        argc++;
    }

    if (argc > 3) {
        Common::Error("Shell_cmp: only two first operands are used");
    }

    if (!strcmp(argv[1], argv[2])) {
        exit(0);
    }

    FILE *f1, *f2;
    f1 = !strcmp(argv[1], "-") ? stdin : fopen(argv[1], "r");

    if (f1 == NULL) {
        Common::Error("Shell_cmp: error opening file");
        exit(1);
    }

    f2 = !strcmp(argv[2], "-") ? stdin : fopen(argv[2], "r");

    if (f2 == NULL) {
        Common::Error("Shell_cmp: error opening file");
        exit(1);
    }
    
    char c1 = fgetc(f1), c2 = fgetc(f2);
    size_t lines_cnt = 1, bytes_cnt = 1;

    while (!feof(f1) && !feof(f2)) {
        if (c1 != c2) {
            printf("%s %s differ: byte %zd, line %zd\n", argv[1], argv[2], bytes_cnt, lines_cnt);
            exit(0);
        }

        if (c1 == '\n') {
            lines_cnt++;
            bytes_cnt = 0;
        }

        bytes_cnt++;

        c1 = fgetc(f1);
        c2 = fgetc(f2);
    }

    if (feof(f1) && feof(f2)) exit(0);

    printf("%s %s differ: byte %zd, line %zd\n", argv[1], argv[2], ++bytes_cnt, lines_cnt);
    exit(0);
}

void cat(int argc, char **argv) {
    if (argc == 1) {
        argv[argc] = (char*)malloc(2 * sizeof(char));
        strcpy(argv[argc], "-");
    }

    bool number_lines = false;
    int i, lines_cnt = 1;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-n")) {
            number_lines = true;
        }
    }

    for (i = 1; i <= argc; i++) {
        if (argv[i] == NULL) {
            break;
        }

        if (argv[i][0] == '-' && strcmp(argv[i], "-")) {
            continue;
        }

        FILE *f;
        if (argv[i][0] == '-') {
            f = stdin;
        }
        else {
            int test = open(argv[i], O_RDONLY);
            if (test == -1) {
                Common::Error("Shell_cat: No such file or directory");
                continue;
            }

            int same = Builtins::SameFile(STDOUT_FILENO, test);
            if (same == -1) {
                Common::Error("Shell_cat: error while checking file");
            }
        
            if (same == 1) {
                Common::Error("Shell_cat: input file is output file");
                close(test);
                continue;
            }

            close(test);

            f = fopen(argv[i], "r");
        }

        size_t sz = 256, it = 0;
        int c;
        float mult = 1.5;
        char *buf = (char*)malloc(sz);

        do {
            while ((c = fgetc(f)) != EOF) {
                buf[it++] = (char)c;
                if (it == sz) {
                    sz *= mult;
                    buf = (char*)realloc(buf, sz);
                }

                if ((char)c == '\n') break;
            }

            if (it == 0) {
                break;
            }

            if (number_lines) {
                printf("%d\t", lines_cnt++);
            }

            size_t j;
            for (j = 0; j < it; j++) {
                putc(buf[j], stdout);
            }

            it = 0;
        } while (c != EOF);

        fclose(f);
        free(buf);
    }

    exit(0);
}

void mv(int argc, char **argv) {
    if (argc != 3) {
        Common::Error("Shell_mv: exactly 2 arguments are needed");
        exit(1);
    }

    FILE *from = fopen(argv[1], "r");
    FILE *to = fopen(argv[2], "w");

    if (from == NULL) {
        Common::Error("Shell_mv: No such file or directory or read protected");
        exit(1);
    }

    if (to == NULL) {
        Common::Error("Shell_mv: No such file or directory or read protected");
        exit(1);
    }

    int test1 = open(argv[1], O_RDONLY), test2 = open(argv[2], O_RDONLY);
    int same = Builtins::SameFile(test1, test2);

    close(test1);
    close(test2);

    if (same == 1) {
        Common::Error("Shell_mv: are the same file");
        exit(0);
    }

    if (same == -1) {
        Common::Error("Shell_mv: error while checking files");
        exit(1);
    }

    char c = fgetc(from);
    while (!feof(from)) {
        putc(c, to);
        c = fgetc(from);
    }

    fclose(from);
    fclose(to);
    
    remove(argv[1]);
    exit(0);
}