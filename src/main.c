#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

const int incr = 5;
const char token = '$';
const char escape = '\\';

struct string {
    char *buf;
    int size;
    int cap;
};

void reset(struct string *str) {
    str->size = 0;
}

void append(struct string *str, char ch) {
    if (str->size == str->cap) {
        char *new_buf = malloc(str->cap + incr);
        memcpy(new_buf, str->buf, str->size);
        free(str->buf);
        str->buf = new_buf;
        str->cap += incr;
    }

    str->buf[str->size] = ch;
    str->size++;
}

const char *get_env(char *buf, int size) {
    char *name = malloc(size + 1);
    memcpy(name, buf, size);
    name[size] = 0;
    const char* s = getenv((const char *) name);
    free(name);
    return s;
}

void print_char(char ch) {
    write(STDOUT_FILENO, &ch, 1);
}

void print_cstr(const char *s) {
    if (s != 0) write(STDOUT_FILENO, s, strlen(s));
}

void print_env(struct string *str) {
    print_cstr(get_env(str->buf, str->size));
}

void print_expr(struct string *str) {
    for (int i = 0; i < str->size; i++) {
        if (str->buf[i] == '-') {
            const char *env = get_env(str->buf, i);
            if (env == 0) {
                if (str->size > i++ && str->buf[i++] == token) {
                    env = get_env(str->buf + i, str->size - i);
                } else {
                    i--;
                    write(STDOUT_FILENO, str->buf + i, str->size - i);
                    return;
                }
            }

            print_cstr(env);
            return;
        }
    }

    print_env(str);
}

bool is_token(char ch) {
    return ch == token;
}

bool is_escape(char ch) {
    return ch == escape;
}

bool is_var_start_char(char ch) {
    return ch == '_' || 'A' <= ch && ch <= 'Z' || 'a' <= ch && ch <= 'z';
}

bool is_var_char(char ch) {
    return '0' <= ch && ch <= '9';
}

int main(int argc, char *args[]) {
    struct string name = {
        .buf = 0,
        .size = 0,
        .cap = 0,
    };

    bool is_esc = false
       , is_var = false
       , is_exp = false;

    char ch;
    while (read(STDIN_FILENO, &ch, 1) > 0) {
        if (is_esc) {
            is_esc = false;
            if (is_token(ch)) {
                print_char(token);
                continue;
            } else {
                print_char(escape);
            }
        } else if (is_exp) {
            if (ch == '}') {
                print_expr(&name);
                reset(&name);
                is_exp = false;
                is_var = false;
                continue;
            } else {
                append(&name, ch);
            }
        } else if (is_var) {
            if (ch == '{') {
                is_exp = true;
            } else if (is_var_start_char(ch) || name.size > 0 && is_var_char(ch)) {
                append(&name, ch);
            } else {
                is_var = false;
                if (name.size == 0) {
                    print_char(token);
                    if (is_token(ch)) {
                        continue;
                    }
                } else {
                    print_env(&name);
                    reset(&name);
                }

                if (is_token(ch)) {
                    is_var = true;
                }
            }
        }

        if (!is_var) {
            if (is_token(ch)) {
                is_var = true;
            } else if (is_escape(ch)) {
                is_esc = true;
            } else {
                print_char(ch);
            }
        }
    }

    return 0;
}
