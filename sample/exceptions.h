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
        case 111:
            printf("Could not connect. Check that the server is up?\n"
                   "Expected arguments: ./mcc [<SERVER> [<PORT>]]\n");
            break;
        case 1000:
            printf("Invalid arguments.\n"
                   "Expected arguments: ./mcc [<SERVER> [<PORT>]]\n");
            break;
        case 1001:
            printf("Invalid port.\n"
                   "Expected arguments: ./mcc [<SERVER> [<PORT>]]\n");
            break;
        case 2000:
            printf("Did the server die?\n");
            break;
        case 3000:
            printf("Got a non-positive packet\n");
            break;
        case 3001:
            printf("Got packet of size 0.\n");
            break;
        case 3002:
            printf("Packet len isn't right.\n");
            break;
        default:
            printf("Undocumented error code :(\n");
    }
    exit(errno);
}
