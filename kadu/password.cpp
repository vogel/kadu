#include "misc.h"
#include "kadu.h"
#include "password.h"

void remindPassword() {
    struct gg_http *h = gg_remind_passwd(config.uin, 0);
    struct gg_pubdir *dupa;

    dupa = (gg_pubdir *) h->data;

    if (h && dupa->success)
        fprintf(stderr,"KK remindPassword(): Done\n");
    else
        fprintf(stderr,"KK remindPassword(): Problem!\n");

    gg_free_remind_passwd(h);
}

