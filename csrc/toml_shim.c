
#include "tomlc17.h"
#include <stdlib.h>
#include <string.h>

/*parsing*/

toml_result_t *zt_parse(const char *src, int len) {
  toml_result_t *r = (toml_result_t *)malloc(sizeof(toml_result_t));
  if (!r) {
    return NULL;
  }
  *r = toml_parse(src, len);
  return r;
}

toml_result_t *zt_parse_file(const char *fname) {
  toml_result_t *r = (toml_result_t *)malloc(sizeof(toml_result_t));
  if (!r) {
    return NULL;
  }
  *r = toml_parse_file_ex(fname);
  return r;
}

int zt_ok(toml_result_t *r) { return r && r->ok ? 1 : 0; }

const char *zt_errmsg(toml_result_t *r) { return r ? r->errmsg : "out of memory"; }

void zt_free(toml_result_t *r) {
  if (!r) {
    return;
  }
  toml_free(*r);
  free(r);
}

/* The top-level table lives inside the result; return a pointer into it. */
toml_datum_t *zt_root(toml_result_t *r) { return r ? &r->toptab : NULL; }

/*datum inspection */

int zt_type(toml_datum_t *d) { return d ? (int)d->type : (int)TOML_UNKNOWN; }

const char *zt_string(toml_datum_t *d) {
  return (d && d->type == TOML_STRING) ? d->u.s : NULL;
}

long long zt_int(toml_datum_t *d) {
  return (d && d->type == TOML_INT64) ? (long long)d->u.int64 : 0;
}

double zt_fp(toml_datum_t *d) {
  return (d && d->type == TOML_FP64) ? d->u.fp64 : 0.0;
}

int zt_bool(toml_datum_t *d) {
  return (d && d->type == TOML_BOOLEAN && d->u.boolean) ? 1 : 0;
}

/* table access */

/* Look up a key in a table. Returns a pointer into the tree or NULL. */
toml_datum_t *zt_get(toml_datum_t *tab, const char *key) {
  if (!tab || tab->type != TOML_TABLE) {
    return NULL;
  }
  for (int i = 0; i < tab->u.tab.size; i++) {
    if (strcmp(tab->u.tab.key[i], key) == 0) {
      return &tab->u.tab.value[i];
    }
  }
  return NULL;
}

/* Dot-separated lookup, e.g. "server.host". Returns NULL if any part misses. */
toml_datum_t *zt_seek(toml_datum_t *tab, const char *path) {
  if (!tab || !path) {
    return NULL;
  }
  toml_datum_t *cur = tab;
  const char *p = path;
  char part[256];
  while (*p) {
    int n = 0;
    while (*p && *p != '.' && n < 255) {
      part[n++] = *p++;
    }
    part[n] = '\0';
    if (*p == '.') {
      p++;
    }
    cur = zt_get(cur, part);
    if (!cur) {
      return NULL;
    }
  }
  return cur;
}

int zt_table_size(toml_datum_t *d) {
  return (d && d->type == TOML_TABLE) ? d->u.tab.size : 0;
}

const char *zt_table_key(toml_datum_t *d, int i) {
  if (!d || d->type != TOML_TABLE || i < 0 || i >= d->u.tab.size) {
    return NULL;
  }
  return d->u.tab.key[i];
}

toml_datum_t *zt_table_value(toml_datum_t *d, int i) {
  if (!d || d->type != TOML_TABLE || i < 0 || i >= d->u.tab.size) {
    return NULL;
  }
  return &d->u.tab.value[i];
}

/* array access */

int zt_array_size(toml_datum_t *d) {
  return (d && d->type == TOML_ARRAY) ? d->u.arr.size : 0;
}

toml_datum_t *zt_array_get(toml_datum_t *d, int i) {
  if (!d || d->type != TOML_ARRAY || i < 0 || i >= d->u.arr.size) {
    return NULL;
  }
  return &d->u.arr.elem[i];
}

/* datetime access */
/* Components are exposed individually to avoid struct-by-value FFI. */

int zt_ts_year(toml_datum_t *d) { return d ? d->u.ts.year : 0; }
int zt_ts_month(toml_datum_t *d) { return d ? d->u.ts.month : 0; }
int zt_ts_day(toml_datum_t *d) { return d ? d->u.ts.day : 0; }
int zt_ts_hour(toml_datum_t *d) { return d ? d->u.ts.hour : 0; }
int zt_ts_minute(toml_datum_t *d) { return d ? d->u.ts.minute : 0; }
int zt_ts_second(toml_datum_t *d) { return d ? d->u.ts.second : 0; }
int zt_ts_usec(toml_datum_t *d) { return d ? d->u.ts.usec : 0; }
int zt_ts_tz(toml_datum_t *d) { return d ? d->u.ts.tz : 0; }
