#include "args.h"

#define _GNU_SOURCE /* from man page */
#include <assert.h> /* assert */
#include <ctype.h>  /* isdigit */
#include <dirent.h> /* opendir DIR */
#include <stdint.h> /* uint */
#include <stdio.h>
#include <stdlib.h> /* malloc fscanf fopen fgets */
#include <string.h> /* memcpy */
#include <unistd.h> /* pid, chdir */

#define RED "\33[1;31m"
#define ORI "\33[0m"
#define LEN(_array) (sizeof(_array) / sizeof(_array[0]))

#define Assert(_con, _fmt, ...)                                                \
  do {                                                                         \
    if (!(_con)) {                                                             \
      fprintf(stderr, RED "Assertion failed:\nLine: %d" _fmt, __LINE__,        \
              ##__VA_ARGS__);                                                  \
      assert(0);                                                               \
    }                                                                          \
  } while (0)

#define safe_printf(_str, ...)                                                 \
  Assert(snprintf(_str, sizeof(_str) - 1, __VA_ARGS__) > 0,                    \
         "process's name is too long")

/*
 *      bro     bro    bro     bro
 * Proc*-->Proc*-->...-->Proc*|-->NULL(forbidden)
 *   ^                        |
 *   | son                    |
 *   |                        |
 * Proc*
 */
struct Proc {
  pid_t pid;
  uint8_t cnt;      // Used to merge
  char *name;       // The name of the Proc
  struct Proc *son; // The first son of the Proc
  struct Proc *bro; // The list of son of Proc's father
} * info[1 << 16];

typedef struct Proc Proc;

struct {
  int (*cmp)(const Proc *, const Proc *);
  unsigned int show_pids : 1;
} print_flag = {};

static int digit_judge(char *s) {
  while (isdigit(*s))
    ++s;
  return *s == '\0';
}

static void add_sonpro(Proc *pp, pid_t pid) {
  int (*const cmp)(const Proc *, const Proc *) = print_flag.cmp;
  if (pp->son == NULL) {
    pp->son = info[pid];
  } else {
    // Insert sort
    if (cmp(pp->son, info[pid]) > 0) {
      info[pid]->bro = pp->son;
      pp->son = info[pid];
      return;
    } else if (pp->son->cnt != 0 && cmp(pp->son, info[pid]) == 0) {
      ++pp->son->cnt;
      return;
    }
    Proc *l = pp->son, *r = l->bro;
    while (r && cmp(r, info[pid]) < 0) {
      l = r;
      r = l->bro;
    }
    if (r && r->cnt != 0 && cmp(r, info[pid]) == 0) {
      ++r->cnt;
      return;
    }
    info[pid]->bro = l->bro;
    l->bro = info[pid];
  }
}

static void init_pid(int pid) {
  Assert(info[pid] = malloc(sizeof(Proc)), "malloc size for Proc failed!");
  *info[pid] = (Proc){
      .son = NULL,
      .bro = NULL,
      .pid = pid,
      .name = NULL,
      .cnt = 0,
  };
}

int alpha(const Proc *p1, const Proc *p2) { return strcmp(p1->name, p2->name); }

int num(const Proc *p1, const Proc *p2) {
  if (print_flag.show_pids || strcmp(p1->name, p2->name)) {
    return p1->pid - p2->pid;
  }
  return 0;
}

void make_tree() {
  DIR *dp;
  struct dirent *entry;
  char filename[50], proname[50];
  FILE *fp = NULL;
  Assert((dp = opendir("/proc")), "cannot open /proc\n");
  Assert(chdir("/proc") == 0, "cannot cd to /proc\n");
  while ((entry = readdir(dp))) {
    if (digit_judge(entry->d_name)) {
      pid_t pid, ppid;
      safe_printf(filename, "%s/status", entry->d_name);

#ifdef IGNORE_PRO_EXIT
      if ((fp = fopen(filename, "r")) == NULL)
        continue;
#else
      Assert((fp = fopen(filename, "r")), "Can not open %s\n", filename);
#endif
      fscanf(fp, "Name:\t%s", proname);

#define MAXN 100
      char buf[MAXN];
      while (fscanf(fp, "Pid:\t%d", &pid) != 1)
        fgets(buf, MAXN, fp);
      if (info[pid] == NULL)
        init_pid(pid);
      if (info[pid]->name == NULL)
        asprintf(&info[pid]->name, "%s", proname);

      while (fscanf(fp, "PPid:\t%d", &ppid) != 1)
        fgets(buf, MAXN, fp);
      if (ppid > 0) {
        if (info[ppid] == NULL) {
          init_pid(ppid);
        }
        add_sonpro(info[ppid], pid);
      }
      fclose(fp);

      DIR *tasks;
      struct dirent *task_entry;
      safe_printf(filename, "%s/task", entry->d_name);
      Assert((tasks = opendir(filename)), "Can not open /proc/%s\n", filename);
      while ((task_entry = readdir(tasks))) {
        if (digit_judge(task_entry->d_name)) {
          safe_printf(filename, "%s/task/%s/status", entry->d_name,
                      task_entry->d_name);
#ifdef IGNORE_PRO_EXIT
          if ((fp = fopen(filename, "r")) == NULL)
            continue;
#else
          Assert(fp = fopen(filename, "r"), "Can not open %s\n", filename);
#endif
          fscanf(fp, "Name:\t%s", proname);
          pid_t ppid = pid;
          pid_t pid;
          while (fscanf(fp, "Pid:\t%d", &pid) != 1)
            fgets(buf, MAXN, fp);
          if (pid != ppid) {
            if (info[pid] == NULL) {
              init_pid(pid);
            }
            if (info[pid]->name == NULL) {
              asprintf(&info[pid]->name, "{%s}", proname);
              info[pid]->cnt = 1;
            }
            add_sonpro(info[ppid], pid);
            fclose(fp);
          }

        }

      }

    }
  }
  closedir(dp);
}

#define MAX_DEP 20
int blank_len[MAX_DEP] = {}, bar_exist[MAX_DEP] = {};
int depth = -1;

void print_tree(const Proc *const p, int is_first) {
  int len = 0;
#define output(...) (len += printf(__VA_ARGS__))
#define delete_bar() bar_exist[depth] = 0;
  if (is_first == 0) {
    for (int i = 0; i <= depth; ++i) {
      printf("%*s", blank_len[i], " ");
      if (i < depth)
        printf("%s", bar_exist[i] != 0 ? "│ " : "  ");
    }
    if (p->bro != NULL) {
      printf("├─");
    } else {
      printf("└─");
      delete_bar();
    }
  } else if (p->pid != 1) {
    if (p->bro != NULL) {
      printf("─┬─");
    } else {
      printf("───");
      delete_bar();
    }
  }
  if (p->cnt > 1) {
    output("%d*[", p->cnt);
  }
  output("%s", p->name);
  if (print_flag.show_pids) {
    output("(%d)", p->pid);
  }
  if (p->cnt > 1) {
    output("]");
  }

  if (p->son == NULL) {
    putchar('\n');
    return;
  }

  blank_len[++depth] = len + 1;
  bar_exist[depth] = (p->son->bro != NULL);
  print_tree(p->son, 1);
  for (Proc *cur = p->son->bro; cur; cur = cur->bro) {
    bar_exist[depth] = cur->bro != NULL;
    print_tree(cur, 0);
  }
  --depth;
  return;
}

int main(int argc, char *argv[]) {
  ArgParser *parser = ap_new();
  if (!parser) {
    exit(1);
  }

  ap_set_helptext(parser, "Usage: ./pstree -p | -n | -v");
  ap_set_version(parser, "1.0");

  ap_flag(parser, "show-pids p");
  ap_flag(parser, "numeric-sort n");

  if (!ap_parse(parser, argc, argv)) {
    exit(1);
  }

  int alpha(const Proc *, const Proc *);
  print_flag.cmp = alpha;

  if (ap_found(parser, "p")) {
    print_flag.show_pids = 1;
  }

  if (ap_found(parser, "n")) {
    print_flag.cmp = num;
  }

  make_tree();
  print_tree(info[1], 1);

  ap_free(parser);
  return 0;
}
