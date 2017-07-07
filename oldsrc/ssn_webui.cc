

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ssn_webui.h"



static void br(FILE* fp) { fprintf(fp, "\n"); }
static void comma(FILE* fp) { fprintf(fp, ","); }
static void lcb(FILE* fp) { fprintf(fp, "{"); }
static void rcb(FILE* fp) { fprintf(fp, "}"); }
static void lsb(FILE* fp) { fprintf(fp, "["); }
static void rsb(FILE* fp) { fprintf(fp, "]"); }
static void space(FILE* fp, size_t n) { for (size_t i=0; i<n; i++) fprintf(fp, " "); }
static void attr(FILE* fp, const char* a) { fprintf(fp, "\"%s\" : ", a); }
static void val_int(FILE* fp, int n) { fprintf(fp, "%d", n); }
static void val_str(FILE* fp, const char* s) { fprintf(fp, "\"%s\"", s); }

void ssn_stats_vnf::dump(FILE* fp) const
{
  lcb(fp);  br(fp);
  attr(fp, "id"    ); val_int(fp, id          ); comma(fp); br(fp);
  attr(fp, "name"  ); val_str(fp, name.c_str()); comma(fp); br(fp);
  attr(fp, "stages"); val_int(fp, stages      );            br(fp);
  rcb(fp);
}

void ssn_stats::dump(FILE* fp) const
{
  lcb(fp); br(fp);
  attr(fp, "nb_threads"); val_int(fp, nb_threads);     comma(fp); br(fp);
  attr(fp, "nb_cpus"   ); val_int(fp, nb_cpus   );     comma(fp); br(fp);
  attr(fp, "nb_ports"  ); val_int(fp, nb_ports  );     comma(fp); br(fp);
  attr(fp, "author"    ); val_str(fp, author.c_str()); comma(fp); br(fp);
  attr(fp, "vnfs"); lsb(fp); br(fp);
  for (size_t i=0; i<vnfs.size(); i++) {
    vnfs[i].dump(fp);
    if (i+1 < vnfs.size()) comma(fp);
    br(fp);
  }
  rsb(fp); br(fp);
  rcb(fp); br(fp);
  fflush(fp);
}


