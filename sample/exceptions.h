#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

/*
 * Acts as an exception handler. Probably hacky, but works well. By declaring
 * the extern int errno and registering internal_error as the SIGINT signal
 * handler, this function prints nice error messages and writes to a log
 * before quitting.
 */

static void internal_error(int signal) {
    printf("\nCaught error. Code: %d\n", errno);
    switch errno {
        case 0:
            printf("Program terminated by user.\n");
            exit(EXIT_SUCCESS);
        case 1000:
            printf("Invalid arguments.\n"
                   "Expected arguments: ./mcc [<SERVER> [<PORT>]]\n");
            exit(EXIT_FAILURE);
            break;
        case 1001:
            printf("Invalid port.\n"
                   "Expected arguments: ./mcc [<SERVER> [<PORT>]]\n");
            exit(EXIT_FAILURE);
            break;
        case 3000:
            printf("Got a non-positive packet\n");
            exit(EXIT_FAILURE);
        case 3001:
            printf("Got packet of size 0.\n");
            exit(EXIT_FAILURE);
        case 3002:
            printf("Packet len isn't right.\n");
            exit(EXIT_FAILURE);
        default:
            printf("Undocumented error code :(\n");
            exit(EXIT_FAILURE);
            break;
    }

}
