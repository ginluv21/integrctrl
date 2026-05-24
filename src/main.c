#include "integrctrl.h"

int main(int argc, char *argv[]) {
    prog_opts opts;
    parse_args(argc, argv, &opts);

    if (opts.mode == 's')
        run_save(&opts);
    else
        run_check(&opts);

    return 0;
}
